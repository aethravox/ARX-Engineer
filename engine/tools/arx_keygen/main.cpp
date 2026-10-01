// ==============================================================================
// tools/arx_keygen/main.cpp — CLI para generar par de claves Ed25519.
//
// Uso:
//   arx_keygen [--output <prefix>] [--quiet]
//
// Genera:
//   <prefix>.priv  (64 bytes: seed(32) || pubkey(32))
//   <prefix>.pub   (32 bytes)
//
// Si no se especifica --output, usa "arx_key" como prefix.
//
// Ejemplo:
//   arx_keygen --output myproject
//   → myproject.priv, myproject.pub
//
// ==============================================================================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "arx/crypto/ed25519.h"

static void print_hex(const uint8_t* data, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        printf("%02x", data[i]);
    }
}

static bool write_file(const char* path, const uint8_t* data, size_t len) {
    FILE* f = fopen(path, "wb");
    if (!f) {
        fprintf(stderr, "Error: no se pudo escribir '%s'\n", path);
        return false;
    }
    fwrite(data, 1, len, f);
    fclose(f);
    return true;
}

static void usage(const char* prog) {
    fprintf(stderr,
        "ARX Keygen — Genera par de claves Ed25519 para firmar .aex\n\n"
        "Uso:\n"
        "  %s [--output <prefix>] [--quiet]\n\n"
        "Opciones:\n"
        "  --output <prefix>  Prefix de los archivos (default: arx_key)\n"
        "  --quiet            No imprimir las claves en stdout\n\n"
        "Genera:\n"
        "  <prefix>.priv  (64 bytes binario)\n"
        "  <prefix>.pub   (32 bytes binario)\n",
        prog);
}

int main(int argc, char** argv) {
    const char* prefix = "arx_key";
    bool quiet = false;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--output") == 0 || strcmp(argv[i], "-o") == 0) {
            if (i + 1 < argc) {
                prefix = argv[++i];
            } else {
                fprintf(stderr, "Error: --output requiere un argumento\n");
                return 1;
            }
        } else if (strcmp(argv[i], "--quiet") == 0 || strcmp(argv[i], "-q") == 0) {
            quiet = true;
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            usage(argv[0]);
            return 0;
        } else {
            fprintf(stderr, "Error: opción desconocida '%s'\n", argv[i]);
            usage(argv[0]);
            return 1;
        }
    }

    // Generar par de claves
    uint8_t priv[ARX_ED25519_PRIVKEY_SIZE];
    uint8_t pub[ARX_ED25519_PUBKEY_SIZE];

    if (!arx_ed25519_keygen(priv, pub)) {
        fprintf(stderr, "Error: fallo al generar claves Ed25519\n");
        return 1;
    }

    // Construir paths
    char priv_path[1024], pub_path[1024];
    snprintf(priv_path, sizeof(priv_path), "%s.priv", prefix);
    snprintf(pub_path, sizeof(pub_path), "%s.pub", prefix);

    // Escribir archivos
    if (!write_file(priv_path, priv, sizeof(priv))) return 1;
    if (!write_file(pub_path, pub, sizeof(pub))) return 1;

    if (!quiet) {
        printf("Claves Ed25519 generadas:\n");
        printf("  Privada: %s (%d bytes)\n", priv_path, ARX_ED25519_PRIVKEY_SIZE);
        printf("  Pública: %s  (%d bytes)\n\n", pub_path, ARX_ED25519_PUBKEY_SIZE);
        printf("Clave pública (hex): ");
        print_hex(pub, ARX_ED25519_PUBKEY_SIZE);
        printf("\n\n");
        printf("Para firmar un .aex:\n");
        printf("  arx_pack --input <assets_dir> --output game.aex --priv %s\n", priv_path);
        printf("Para verificar:\n");
        printf("  arx_verify --input game.aex --pub %s\n", pub_path);
    }

    return 0;
}
