// arx_sign/main.c — CLI para firmar .aex
//
// Uso:
//   arx_sign <input.aex.unsigned> <output.aex> <creator_privkey.bin>
//
// Lee un .aex ya construido (sin firma), lee la clave privada Ed25519
// de 64 bytes desde <creator_privkey.bin>, recalcula la firma sobre el
// sig_payload, y escribe el .aex final con la firma en el signature block.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arx/crypto/ed25519.h"
#include "arx/format/aex.h"

static int read_file(const char* path, uint8_t** out_data, size_t* out_size) {
    FILE* f = fopen(path, "rb");
    if (!f) { fprintf(stderr, "Error: no se pudo abrir %s\n", path); return -1; }
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    uint8_t* buf = malloc((size_t)sz);
    if (!buf) { fclose(f); return -1; }
    if (fread(buf, 1, (size_t)sz, f) != (size_t)sz) { free(buf); fclose(f); return -1; }
    fclose(f);
    *out_data = buf;
    *out_size = (size_t)sz;
    return 0;
}

static int write_file(const char* path, const uint8_t* data, size_t size) {
    FILE* f = fopen(path, "wb");
    if (!f) { fprintf(stderr, "Error: no se pudo escribir %s\n", path); return -1; }
    if (fwrite(data, 1, size, f) != size) { fclose(f); return -1; }
    fclose(f);
    return 0;
}

int main(int argc, char** argv) {
    if (argc != 4) {
        fprintf(stderr, "Uso: %s <input.aex.unsigned> <output.aex> <creator_privkey.bin>\n", argv[0]);
        return 1;
    }
    const char* in_path = argv[1];
    const char* out_path = argv[2];
    const char* key_path = argv[3];

    // Leer .aex sin firma
    uint8_t* aex_data = NULL;
    size_t aex_size = 0;
    if (read_file(in_path, &aex_data, &aex_size) != 0) return 1;
    printf("Leido %s: %zu bytes\n", in_path, aex_size);

    // Leer clave privada (64 bytes seed||pub)
    uint8_t priv[64];
    uint8_t* key_data = NULL;
    size_t key_size = 0;
    if (read_file(key_path, &key_data, &key_size) != 0) { free(aex_data); return 1; }
    if (key_size != 64) {
        fprintf(stderr, "Clave privada tamano incorrecto: %zu (esperado 64)\n", key_size);
        free(aex_data); free(key_data); return 1;
    }
    memcpy(priv, key_data, 64);
    free(key_data);

    // Parsear cabecera para encontrar offsets
    if (aex_size < AEX_HEADER_SIZE) {
        fprintf(stderr, "Archivo demasiado pequeno para ser .aex\n");
        free(aex_data); return 1;
    }
    AexHeader* hdr = (AexHeader*)aex_data;
    if (hdr->magic != AEX_MAGIC) {
        fprintf(stderr, "Magic no es AEX1\n");
        free(aex_data); return 1;
    }

    int64_t sig_off = hdr->manifest_offset + hdr->manifest_size;
    if (sig_off + AEX_SIG_BLOCK_SIZE > (int64_t)aex_size) {
        fprintf(stderr, "No hay espacio para signature block\n");
        free(aex_data); return 1;
    }
    AexSignatureBlock* sigblk = (AexSignatureBlock*)(aex_data + sig_off);

    // Recalcular hashes
    arx_sha256(aex_data + hdr->manifest_offset, (size_t)hdr->manifest_size,
                sigblk->manifest_sha256);
    arx_sha256(aex_data + hdr->assets_offset, (size_t)hdr->assets_size,
                sigblk->assets_sha256);
    arx_sha256(aex_data + hdr->scripts_offset, (size_t)hdr->scripts_size,
                sigblk->scripts_sha256);

    // Construir sig_payload
    uint8_t payload[112];
    size_t off = 0;
    memcpy(payload + off, &hdr->magic, 4); off += 4;
    memcpy(payload + off, &hdr->format_version, 2); off += 2;
    memcpy(payload + off, &hdr->flags, 2); off += 2;
    memcpy(payload + off, &hdr->created_unix, 8); off += 8;
    memcpy(payload + off, sigblk->manifest_sha256, 32); off += 32;
    memcpy(payload + off, sigblk->assets_sha256, 32); off += 32;
    memcpy(payload + off, sigblk->scripts_sha256, 32); off += 32;

    // Copiar pubkey al sigblock
    memcpy(sigblk->creator_pubkey, priv + 32, 32);

    // Firmar
    if (!arx_ed25519_sign(payload, sizeof(payload), priv, sigblk->creator_signature)) {
        fprintf(stderr, "Error: firma Ed25519 fallo\n");
        free(aex_data); return 1;
    }
    sigblk->sig_block_version = 1;

    // Escribir .aex final
    if (write_file(out_path, aex_data, aex_size) != 0) {
        free(aex_data); return 1;
    }
    printf("OK: %s firmado correctamente (%zu bytes)\n", out_path, aex_size);
    free(aex_data);
    return 0;
}
