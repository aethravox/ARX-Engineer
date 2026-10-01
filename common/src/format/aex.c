// arx/format/aex.c — Lector del formato .aex
//
// Implementa la API declarada en arx/format/aex.h:
//   - aex_open: lee cabecera + manifest + signature block
//   - aex_verify_signature: valida hashes + firma Ed25519
//   - aex_get_manifest_json: devuelve el manifest como string
//   - aex_read_asset / aex_read_scripts: acceso a bloques
//   - aex_close: cleanup

#include "arx/format/aex.h"
#include "arx/format/tar.h"
#include "arx/crypto/ed25519.h"
#include "arx/crypto/sha256.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ============================================================
// Estructura interna
// ============================================================

struct AexPackage {
    char* path;
    uint8_t* raw_data;        // archivo completo mapeado en memoria
    size_t raw_size;

    AexHeader header;
    char* manifest_json;      // null-terminated, owned
    AexSignatureBlock sig_block;

    // Cache del manifest parseado
    ArxManifest* parsed_manifest;

    // Cache del assets block descomprimido (si esta comprimido con zstd)
    uint8_t* assets_decompressed;
    size_t assets_decompressed_size;

    int verified;
};

// ============================================================
// Helpers I/O
// ============================================================

static int read_file_to_memory(const char* path, uint8_t** out_data, size_t* out_size) {
    FILE* f = fopen(path, "rb");
    if (!f) return -1;
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    if (sz < 0) { fclose(f); return -1; }
    fseek(f, 0, SEEK_SET);
    uint8_t* buf = malloc((size_t)sz + 1);
    if (!buf) { fclose(f); return -1; }
    size_t got = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    if (got != (size_t)sz) { free(buf); return -1; }
    buf[sz] = 0;  // null-terminar por seguridad
    *out_data = buf;
    *out_size = (size_t)sz;
    return 0;
}

// ============================================================
// aex_open
// ============================================================

AexResult aex_open(const char* path, AexPackage** out_pkg) {
    if (!path || !out_pkg) return AEX_ERR_NULL_ARG;
    *out_pkg = NULL;

    AexPackage* pkg = calloc(1, sizeof(AexPackage));
    if (!pkg) return AEX_ERR_IO;
    pkg->path = strdup(path);
    pkg->verified = 0;

    uint8_t* data = NULL;
    size_t size = 0;
    if (read_file_to_memory(path, &data, &size) != 0) {
        aex_close(pkg);
        return AEX_ERR_IO;
    }
    pkg->raw_data = data;
    pkg->raw_size = size;

    if (size < AEX_HEADER_SIZE) {
        aex_close(pkg);
        return AEX_ERR_CORRUPT_HEADER;
    }

    // Parse cabecera (little-endian)
    memcpy(&pkg->header, data, sizeof(AexHeader));
    if (pkg->header.magic != AEX_MAGIC) {
        aex_close(pkg);
        return AEX_ERR_MAGIC_MISMATCH;
    }
    if (pkg->header.format_version > AEX_FORMAT_VERSION) {
        aex_close(pkg);
        return AEX_ERR_VERSION_UNSUPPORTED;
    }

    // Validar offsets
    int64_t man_off = pkg->header.manifest_offset;
    int64_t man_sz  = pkg->header.manifest_size;
    if (man_off < AEX_HEADER_SIZE || man_off + man_sz > (int64_t)size) {
        aex_close(pkg);
        return AEX_ERR_CORRUPT_HEADER;
    }
    if (pkg->header.assets_offset + pkg->header.assets_size > (int64_t)size) {
        aex_close(pkg);
        return AEX_ERR_CORRUPT_HEADER;
    }
    if (pkg->header.scripts_offset + pkg->header.scripts_size > (int64_t)size) {
        aex_close(pkg);
        return AEX_ERR_CORRUPT_HEADER;
    }

    // Copiar manifest como string null-terminated
    pkg->manifest_json = malloc((size_t)man_sz + 1);
    if (!pkg->manifest_json) { aex_close(pkg); return AEX_ERR_IO; }
    memcpy(pkg->manifest_json, data + man_off, (size_t)man_sz);
    pkg->manifest_json[man_sz] = 0;

    // Signature block: esta entre manifest y assets
    int64_t sig_off = man_off + man_sz;
    int64_t sig_sz = AEX_SIG_BLOCK_SIZE;
    if (sig_off + sig_sz > pkg->header.assets_offset) {
        aex_close(pkg);
        return AEX_ERR_SIGNATURE_BLOCK_INVALID;
    }
    memcpy(&pkg->sig_block, data + sig_off, sizeof(AexSignatureBlock));

    *out_pkg = pkg;
    return AEX_OK;
}

// ============================================================
// aex_verify_signature
// ============================================================

AexResult aex_verify_signature(const AexPackage* pkg) {
    if (!pkg) return AEX_ERR_NULL_ARG;
    if (!pkg->raw_data) return AEX_ERR_IO;

    // 1. Recalcular sha256(manifest)
    uint8_t h_manifest[32];
    arx_sha256((const uint8_t*)pkg->manifest_json,
               strlen(pkg->manifest_json),
               h_manifest);
    if (memcmp(h_manifest, pkg->sig_block.manifest_sha256, 32) != 0) {
        return AEX_ERR_HASH_MISMATCH;
    }

    // 2. Recalcular sha256(assets_block)
    uint8_t h_assets[32];
    arx_sha256(pkg->raw_data + pkg->header.assets_offset,
               (size_t)pkg->header.assets_size,
               h_assets);
    if (memcmp(h_assets, pkg->sig_block.assets_sha256, 32) != 0) {
        return AEX_ERR_HASH_MISMATCH;
    }

    // 3. Recalcular sha256(scripts_block)
    uint8_t h_scripts[32];
    arx_sha256(pkg->raw_data + pkg->header.scripts_offset,
               (size_t)pkg->header.scripts_size,
               h_scripts);
    if (memcmp(h_scripts, pkg->sig_block.scripts_sha256, 32) != 0) {
        return AEX_ERR_HASH_MISMATCH;
    }

    // 4. Reconstruir sig_payload
    // sig_payload = magic || format_version || flags || created_unix
    //            || sha256(manifest) || sha256(assets) || sha256(scripts)
    size_t payload_size = 4 + 2 + 2 + 8 + 32 + 32 + 32;  // = 112
    uint8_t* payload = malloc(payload_size);
    if (!payload) return AEX_ERR_IO;

    size_t off = 0;
    uint32_t magic = pkg->header.magic;
    uint16_t fv = pkg->header.format_version;
    uint16_t fl = pkg->header.flags;
    int64_t cu = pkg->header.created_unix;

    memcpy(payload + off, &magic, 4); off += 4;
    memcpy(payload + off, &fv, 2);    off += 2;
    memcpy(payload + off, &fl, 2);    off += 2;
    memcpy(payload + off, &cu, 8);    off += 8;
    memcpy(payload + off, h_manifest, 32);  off += 32;
    memcpy(payload + off, h_assets, 32);    off += 32;
    memcpy(payload + off, h_scripts, 32);   off += 32;

    // 5. Verificar firma Ed25519
    bool ok = arx_ed25519_verify(payload, payload_size,
                                  pkg->sig_block.creator_pubkey,
                                  pkg->sig_block.creator_signature);
    free(payload);

    if (!ok) return AEX_ERR_SIGNATURE_INVALID;

    ((AexPackage*)pkg)->verified = 1;
    return AEX_OK;
}

// ============================================================
// Getters
// ============================================================

const char* aex_get_manifest_json(const AexPackage* pkg) {
    return pkg ? pkg->manifest_json : NULL;
}

// ============================================================
// Manifest parsing
// ============================================================

AexResult aex_parse_manifest(AexPackage* pkg) {
    if (!pkg) return AEX_ERR_NULL_ARG;
    if (pkg->parsed_manifest) return AEX_OK;  // ya parseado

    ArxManifest* m = calloc(1, sizeof(ArxManifest));
    if (!m) return AEX_ERR_IO;

    if (!arx_manifest_parse(m, pkg->manifest_json)) {
        free(m);
        return AEX_ERR_MANIFEST_INVALID_JSON;
    }
    pkg->parsed_manifest = m;
    return AEX_OK;
}

const ArxManifest* aex_get_manifest(const AexPackage* pkg) {
    return pkg ? pkg->parsed_manifest : NULL;
}

const uint8_t* aex_get_creator_pubkey(const AexPackage* pkg) {
    return pkg ? pkg->sig_block.creator_pubkey : NULL;
}

const uint8_t* aex_get_raw_bytes(const AexPackage* pkg, size_t* out_size) {
    if (!pkg) { if (out_size) *out_size = 0; return NULL; }
    if (out_size) *out_size = pkg->raw_size;
    return pkg->raw_data;
}

// ============================================================
// aex_read_asset / aex_read_scripts
// ============================================================
// Nota: la descompresion zstd y el parseo TAR se implementaran cuando
// se integre zstd. Por ahora, devolvemos el bloque crudo.

AexResult aex_read_scripts(const AexPackage* pkg,
                            uint8_t** out_data,
                            size_t* out_size) {
    if (!pkg || !out_data || !out_size) return AEX_ERR_NULL_ARG;
    *out_data = NULL;
    *out_size = 0;

    size_t sz = (size_t)pkg->header.scripts_size;
    if (sz == 0) return AEX_ERR_SCRIPTS_BLOCK_CORRUPT;

    uint8_t* buf = malloc(sz);
    if (!buf) return AEX_ERR_IO;
    memcpy(buf, pkg->raw_data + pkg->header.scripts_offset, sz);

    *out_data = buf;
    *out_size = sz;
    return AEX_OK;
}

AexResult aex_read_asset(const AexPackage* pkg,
                          const char* asset_path,
                          uint8_t** out_data,
                          size_t* out_size) {
    if (!pkg || !asset_path || !out_data || !out_size) return AEX_ERR_NULL_ARG;
    *out_data = NULL;
    *out_size = 0;

    AexPackage* pkg_mut = (AexPackage*)pkg;

    // Obtener el assets block (con soporte de descompresion zstd en el futuro)
    size_t block_size = (size_t)pkg->header.assets_size;
    if (block_size == 0) return AEX_ERR_ASSET_BLOCK_CORRUPT;
    const uint8_t* block = pkg->raw_data + pkg->header.assets_offset;

    // Parsear el TAR para buscar el asset por path
    TarReader reader;
    tar_reader_init(&reader, block, block_size);

    TarEntry entry;
    if (!tar_reader_find(&reader, asset_path, &entry)) {
        return AEX_ERR_ASSET_BLOCK_CORRUPT;  // no encontrado
    }

    if (entry.size == 0) {
        *out_data = malloc(1);
        *out_size = 0;
        return AEX_OK;
    }

    uint8_t* buf = malloc(entry.size);
    if (!buf) return AEX_ERR_IO;
    memcpy(buf, entry.data, entry.size);

    *out_data = buf;
    *out_size = entry.size;
    (void)pkg_mut;
    return AEX_OK;
}

// ============================================================
// aex_close / aex_free
// ============================================================

void aex_close(AexPackage* pkg) {
    if (!pkg) return;
    free(pkg->path);
    free(pkg->raw_data);
    free(pkg->manifest_json);
    free(pkg->parsed_manifest);
    free(pkg->assets_decompressed);
    free(pkg);
}

void aex_free(void* ptr) { free(ptr); }

// ============================================================
// strerror
// ============================================================

const char* aex_strerror(AexResult code) {
    switch (code) {
        case AEX_OK: return "OK";
        case AEX_ERR_IO: return "error de E/S";
        case AEX_ERR_MAGIC_MISMATCH: return "magic no es AEX1";
        case AEX_ERR_VERSION_UNSUPPORTED: return "version no soportada";
        case AEX_ERR_CORRUPT_HEADER: return "cabecera corrupta";
        case AEX_ERR_MANIFEST_INVALID_JSON: return "manifest JSON invalido";
        case AEX_ERR_MANIFEST_MISSING_FIELD: return "manifest sin campo requerido";
        case AEX_ERR_HASH_MISMATCH: return "hash SHA256 no coincide";
        case AEX_ERR_SIGNATURE_INVALID: return "firma Ed25519 invalida";
        case AEX_ERR_PERMISSION_DENIED: return "permiso denegado por el usuario";
        case AEX_ERR_ASSET_BLOCK_CORRUPT: return "bloque de assets corrupto";
        case AEX_ERR_SCRIPTS_BLOCK_CORRUPT: return "bloque de scripts corrupto";
        case AEX_ERR_SIGNATURE_BLOCK_INVALID: return "signature block invalido";
        case AEX_ERR_NULL_ARG: return "argumento nulo";
    }
    return "error desconocido";
}
