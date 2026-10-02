// ==============================================================================
// tools/arx_verify/main.cpp — CLI para verificar un .aex firmado.
//
// Uso:
//   arx_verify --input <file.aex> --pub <pubkey.pub>
//   arx_verify -i game.aex -p arx_key.pub
//
// Verifica:
//   1. Magic number "AEX1"
//   2. Hashes SHA-256 del manifest, assets y scripts
//   3. Firma Ed25519 del creador
//
// Exit codes:
//   0 = válido
//   1 = inválido (firma no coincide, hashes corruptos, etc)
//   2 = error de I/O o argumentos
//
// ==============================================================================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "arx/format/aex.h"

static bool read_file(const char* path, uint8_t** out_data, size_t* out_size) {
    FILE* f = fopen(path, "rb");
    if (!f) return false;
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    *out_data = (uint8_t*)malloc(sz);
    if (!*out_data) { fclose(f); return false; }
    *out_size = fread(*out_data, 1, sz, f);
    fclose(f);
    return true;
}

static void usage(const char* prog) {
    fprintf(stderr,
        "ARX Verify — Verifica firma Ed25519 de un .aex\n\n"
        "Uso:\n"
        "  %s --input <file.aex> --pub <pubkey.pub>\n\n"
        "Opciones:\n"
        "  --input, -i <file>    Archivo .aex a verificar\n"
        "  --pub, -p <file>      Clave pública Ed25519 (32 bytes)\n"
        "  --verbose, -v         Mostrar info detallada del paquete\n\n"
        "Exit codes:\n"
        "  0 = válido\n"
        "  1 = inválido\n"
        "  2 = error\n",
        prog);
}

int main(int argc, char** argv) {
    const char* input_path = NULL;
    const char* pub_path = NULL;
    bool verbose = false;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--input") == 0 || strcmp(argv[i], "-i") == 0) {
            if (i + 1 < argc) input_path = argv[++i];
        } else if (strcmp(argv[i], "--pub") == 0 || strcmp(argv[i], "-p") == 0) {
            if (i + 1 < argc) pub_path = argv[++i];
        } else if (strcmp(argv[i], "--verbose") == 0 || strcmp(argv[i], "-v") == 0) {
            verbose = true;
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            usage(argv[0]);
            return 0;
        }
    }

    if (!input_path || !pub_path) {
        fprintf(stderr, "Error: faltan argumentos\n");
        usage(argv[0]);
        return 2;
    }

    // Leer clave pública
    uint8_t* pub_data = NULL;
    size_t pub_size = 0;
    if (!read_file(pub_path, &pub_data, &pub_size)) {
        fprintf(stderr, "Error: no se pudo leer '%s'\n", pub_path);
        return 2;
    }
    if (pub_size != AEX_PUBKEY_SIZE) {
        fprintf(stderr, "Error: clave pública debe ser %d bytes, no %zu\n",
                AEX_PUBKEY_SIZE, pub_size);
        free(pub_data);
        return 2;
    }

    // Abrir paquete .aex
    AexPackage* pkg = NULL;
    AexResult result = aex_open(input_path, &pkg);
    if (result != AEX_OK) {
        fprintf(stderr, "Error abriendo %s: %s\n", input_path, aex_strerror(result));
        free(pub_data);
        return 1;
    }

    if (verbose) {
        printf("Paquete: %s\n", input_path);
        // No podemos acceder a pkg->header porque AexPackage es incomplete type
        // (forward declaration). Solo usamos la API pública.
        const char* manifest = aex_get_manifest_json(pkg);
        if (manifest) {
            printf("  Manifest: %s\n", manifest);
        }
        printf("\n");
    }

    // Verificar que la clave pública del paquete coincide con la del archivo
    const uint8_t* pkg_pub = aex_get_creator_pubkey(pkg);
    if (!pkg_pub || memcmp(pkg_pub, pub_data, AEX_PUBKEY_SIZE) != 0) {
        fprintf(stderr, "FAIL: la clave pública del paquete no coincide con la proporcionada\n");
        free(pub_data);
        return 1;
    }

    // Verificar firma (aex_verify_signature verifica hashes + firma Ed25519)
    result = aex_verify_signature(pkg);
    if (result != AEX_OK) {
        fprintf(stderr, "FAIL: verificación falló: %s\n", aex_strerror(result));
        free(pub_data);
        return 1;
    }

    printf("✓ Hashes SHA-256 válidos\n");
    printf("✓ Firma Ed25519 válida\n");
    printf("\nPaquete VÁLIDO: %s\n", input_path);

    free(pub_data);
    return 0;
}
