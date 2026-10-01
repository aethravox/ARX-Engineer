// arx/format/aex_builder.c — Constructor del .aex
//
// Construye un .aex desde:
//   - un manifest JSON
//   - una lista de assets (path + datos)
//   - un bloque de scripts
//   - una clave privada Ed25519 del creador
//
// Layout final:
//   [64 bytes header]
//   [manifest bytes]
//   [292 bytes signature block]
//   [assets bytes (tar crudo por ahora; zstd en v1.1)]
//   [scripts bytes]

#include "arx/format/aex.h"
#include "arx/crypto/ed25519.h"
#include "arx/crypto/sha256.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ============================================================
// Estructura interna del builder
// ============================================================

typedef struct AexAssetEntry {
    char* path;
    uint8_t* data;
    size_t size;
    struct AexAssetEntry* next;
} AexAssetEntry;

struct AexBuilder {
    char* manifest_json;
    AexAssetEntry* assets_head;
    AexAssetEntry* assets_tail;
    size_t assets_count;

    uint8_t* scripts_data;
    size_t scripts_size;
    uint8_t scripts_type;

    uint8_t creator_priv[32];
    int has_creator_key;

    int compressed;
};

// ============================================================
// new / free
// ============================================================

AexBuilder* aex_builder_new(void) {
    AexBuilder* b = calloc(1, sizeof(AexBuilder));
    if (!b) return NULL;
    // Compresion desactivada por defecto hasta que se integre zstd.
    // El caller puede activarla con aex_builder_set_compressed(b, true)
    // siempre que la lib se haya compilado con soporte zstd.
    b->compressed = 0;
    return b;
}

void aex_builder_free(AexBuilder* b) {
    if (!b) return;
    free(b->manifest_json);
    free(b->scripts_data);

    AexAssetEntry* e = b->assets_head;
    while (e) {
        AexAssetEntry* next = e->next;
        free(e->path);
        free(e->data);
        free(e);
        e = next;
    }
    free(b);
}

// ============================================================
// Setters
// ============================================================

AexResult aex_builder_set_manifest(AexBuilder* b, const char* json) {
    if (!b || !json) return AEX_ERR_NULL_ARG;
    free(b->manifest_json);
    b->manifest_json = strdup(json);
    return AEX_OK;
}

AexResult aex_builder_add_asset(AexBuilder* b, const char* path,
                                  const uint8_t* data, size_t size) {
    if (!b || !path || (!data && size > 0)) return AEX_ERR_NULL_ARG;

    AexAssetEntry* e = calloc(1, sizeof(AexAssetEntry));
    if (!e) return AEX_ERR_IO;
    e->path = strdup(path);
    e->size = size;
    if (size > 0) {
        e->data = malloc(size);
        if (!e->data) { free(e->path); free(e); return AEX_ERR_IO; }
        memcpy(e->data, data, size);
    }

    if (b->assets_tail) {
        b->assets_tail->next = e;
    } else {
        b->assets_head = e;
    }
    b->assets_tail = e;
    b->assets_count++;
    return AEX_OK;
}

AexResult aex_builder_set_scripts(AexBuilder* b, uint8_t script_type,
                                    const uint8_t* data, size_t size) {
    if (!b || (!data && size > 0)) return AEX_ERR_NULL_ARG;
    free(b->scripts_data);
    b->scripts_data = NULL;
    b->scripts_size = 0;

    if (size > 0) {
        b->scripts_data = malloc(size);
        if (!b->scripts_data) return AEX_ERR_IO;
        memcpy(b->scripts_data, data, size);
    }
    b->scripts_size = size;
    b->scripts_type = script_type;
    return AEX_OK;
}

AexResult aex_builder_set_creator_key(AexBuilder* b, const uint8_t private_key[32]) {
    if (!b || !private_key) return AEX_ERR_NULL_ARG;
    memcpy(b->creator_priv, private_key, 32);
    b->has_creator_key = 1;
    return AEX_OK;
}

AexResult aex_builder_set_compressed(AexBuilder* b, bool compressed) {
    if (!b) return AEX_ERR_NULL_ARG;
    b->compressed = compressed ? 1 : 0;
    return AEX_OK;
}

// ============================================================
// Builder interno: TAR simplificado
//
// Cada archivo en el TAR sigue el formato POSIX ustar:
//   512 bytes header + ceil(size/512)*512 data
// Header (relevant fields):
//   offset 0:    name (100 bytes)
//   offset 124:  size (12 bytes, octal)
//   offset 148:  typeflag (1 byte, '0' = file)
//   offset 156:  magic "ustar\0" (6 bytes)
//   offset 257:  version "00" (2 bytes)
//
// Final: 2 bloques de 512 bytes de ceros.
// ============================================================

static size_t build_tar(AexAssetEntry* head, uint8_t** out_buf) {
    // 1a pasada: calcular tamano total
    size_t total = 0;
    for (AexAssetEntry* e = head; e; e = e->next) {
        total += 512;  // header
        size_t padded = (e->size + 511) & ~511ULL;
        total += padded;
    }
    total += 1024;  // 2 bloques final

    uint8_t* buf = calloc(1, total);
    if (!buf) return 0;

    size_t off = 0;
    for (AexAssetEntry* e = head; e; e = e->next) {
        // Header: 512 bytes
        memset(buf + off, 0, 512);

        // name (100)
        strncpy((char*)(buf + off), e->path, 100);

        // mode (8) "0000644\0"
        memcpy(buf + off + 100, "0000644\0", 8);

        // uid, gid (8 each) "0000000\0"
        memcpy(buf + off + 108, "0000000\0", 8);
        memcpy(buf + off + 116, "0000000\0", 8);

        // size (12 octal)
        char size_oct[13];
        snprintf(size_oct, sizeof(size_oct), "%011o", (unsigned)e->size);
        memcpy(buf + off + 124, size_oct, 12);

        // mtime (12 octal)
        memcpy(buf + off + 136, "00000000000", 12);

        // checksum placeholder (8 spaces)
        memset(buf + off + 148, ' ', 8);

        // typeflag '0' = regular file
        buf[off + 156] = '0';

        // magic "ustar\0"
        memcpy(buf + off + 257, "ustar\0", 6);

        // version "00"
        memcpy(buf + off + 263, "00", 2);

        // Calcular checksum (suma de todos los bytes del header)
        unsigned chk = 0;
        for (int i = 0; i < 512; i++) chk += buf[off + i];
        snprintf(size_oct, sizeof(size_oct), "%06o", chk);
        memcpy(buf + off + 148, size_oct, 6);
        buf[off + 154] = 0;
        buf[off + 155] = ' ';

        off += 512;

        // Data padded to 512
        if (e->size > 0) {
            memcpy(buf + off, e->data, e->size);
            size_t padded = (e->size + 511) & ~511ULL;
            // memset ya hecho por calloc
            off += padded;
        }
    }
    // 2 bloques final ya son ceros (calloc)
    // off == total - 1024, los 1024 finales ya son zero

    *out_buf = buf;
    return total;
}

// ============================================================
// aex_builder_write
// ============================================================

AexResult aex_builder_write(AexBuilder* b, const char* path) {
    if (!b || !path) return AEX_ERR_NULL_ARG;
    if (!b->manifest_json) return AEX_ERR_MANIFEST_MISSING_FIELD;
    if (!b->has_creator_key) return AEX_ERR_SIGNATURE_INVALID;

    // 1. Construir assets block (TAR)
    uint8_t* assets_block = NULL;
    size_t assets_block_size = 0;
    if (b->assets_head) {
        assets_block_size = build_tar(b->assets_head, &assets_block);
        if (!assets_block) return AEX_ERR_IO;
    }

    // 2. Hashes
    size_t man_size = strlen(b->manifest_json);
    uint8_t h_manifest[32], h_assets[32], h_scripts[32];
    arx_sha256((const uint8_t*)b->manifest_json, man_size, h_manifest);
    // IMPORTANTE: usar SHA-256 de cadena vacia (NULL, 0) cuando el bloque es
    // vacio, para que el reader (que siempre recalcula SHA-256 del bloque
    // apuntado por el offset) obtenga el mismo resultado.
    if (assets_block_size > 0)
        arx_sha256(assets_block, assets_block_size, h_assets);
    else
        arx_sha256(NULL, 0, h_assets);
    if (b->scripts_size > 0)
        arx_sha256(b->scripts_data, b->scripts_size, h_scripts);
    else
        arx_sha256(NULL, 0, h_scripts);

    // 3. Header
    AexHeader hdr;
    memset(&hdr, 0, sizeof(hdr));
    hdr.magic = AEX_MAGIC;
    hdr.format_version = AEX_FORMAT_VERSION;
    hdr.flags = b->compressed ? AEX_FLAG_COMPRESSED : 0;
    hdr.created_unix = (int64_t)time(NULL);
    hdr.manifest_offset = AEX_HEADER_SIZE;
    hdr.manifest_size = (int64_t)man_size;
    // signature block va justo despues del manifest
    int64_t sig_off = hdr.manifest_offset + hdr.manifest_size;
    hdr.assets_offset = sig_off + AEX_SIG_BLOCK_SIZE;
    hdr.assets_size = (int64_t)assets_block_size;
    hdr.scripts_offset = hdr.assets_offset + hdr.assets_size;
    hdr.scripts_size = (int64_t)b->scripts_size;

    // 4. Sig payload
    size_t payload_size = 4 + 2 + 2 + 8 + 32 + 32 + 32;  // 112
    uint8_t* payload = malloc(payload_size);
    if (!payload) { free(assets_block); return AEX_ERR_IO; }
    size_t off = 0;
    memcpy(payload + off, &hdr.magic, 4); off += 4;
    memcpy(payload + off, &hdr.format_version, 2); off += 2;
    memcpy(payload + off, &hdr.flags, 2); off += 2;
    memcpy(payload + off, &hdr.created_unix, 8); off += 8;
    memcpy(payload + off, h_manifest, 32); off += 32;
    memcpy(payload + off, h_assets, 32);   off += 32;
    memcpy(payload + off, h_scripts, 32);  off += 32;

    // 5. Firmar
    uint8_t sig[64];
    // b->creator_priv es solo el seed (32 bytes). Construimos priv64 = seed||pub.
    uint8_t priv64[64];
    memset(priv64, 0, 64);  // importante: para que derive_pub detecte "solo seed"
    memcpy(priv64, b->creator_priv, 32);
    // Derivar pub desde el seed (llenara priv64[32..64])
    if (!arx_ed25519_derive_pub(priv64, priv64 + 32)) {
        free(assets_block); free(payload);
        return AEX_ERR_SIGNATURE_INVALID;
    }
    if (!arx_ed25519_sign(payload, payload_size, priv64, sig)) {
        free(assets_block); free(payload);
        return AEX_ERR_SIGNATURE_INVALID;
    }

    // 6. Signature block
    AexSignatureBlock sigblk;
    memset(&sigblk, 0, sizeof(sigblk));
    memcpy(sigblk.creator_pubkey, priv64 + 32, 32);
    memcpy(sigblk.creator_signature, sig, 64);
    // engine_pubkey/sig quedan en cero (opcional)
    memcpy(sigblk.manifest_sha256, h_manifest, 32);
    memcpy(sigblk.assets_sha256, h_assets, 32);
    memcpy(sigblk.scripts_sha256, h_scripts, 32);
    sigblk.sig_block_version = 1;

    free(payload);

    // 7. Escribir archivo
    FILE* f = fopen(path, "wb");
    if (!f) { free(assets_block); return AEX_ERR_IO; }

    size_t total = (size_t)(hdr.scripts_offset + hdr.scripts_size);
    uint8_t* out = malloc(total);
    if (!out) { fclose(f); free(assets_block); return AEX_ERR_IO; }

    size_t w = 0;
    memcpy(out + w, &hdr, sizeof(hdr)); w += sizeof(hdr);
    memcpy(out + w, b->manifest_json, man_size); w += man_size;
    memcpy(out + w, &sigblk, sizeof(sigblk)); w += sizeof(sigblk);
    if (assets_block_size > 0) {
        memcpy(out + w, assets_block, assets_block_size); w += assets_block_size;
    }
    if (b->scripts_size > 0) {
        memcpy(out + w, b->scripts_data, b->scripts_size); w += b->scripts_size;
    }

    if (fwrite(out, 1, total, f) != total) {
        fclose(f); free(out); free(assets_block);
        return AEX_ERR_IO;
    }
    fclose(f);
    free(out);
    free(assets_block);
    return AEX_OK;
}
