// ==============================================================================
// tools/arx_pack/main.cpp — CLI para empaquetar un directorio en .aex firmado.
//
// Uso:
//   arx_pack --input <assets_dir> --output <file.aex> --priv <key.priv>
//             [--manifest <manifest.json>] [--scripts <scripts_dir>]
//             [--compressed] [--name <pkg_name>] [--version <ver>]
//
// Lee todos los archivos del assets_dir y los empaqueta en un .aex firmado
// con la clave privada Ed25519.
//
// El manifest se genera automáticamente si no se proporciona --manifest,
// con permisos mínimos (solo filesystem sandbox).
//
// Ejemplo:
//   arx_keygen --output myproject
//   arx_pack -i ./assets -o game.aex --priv myproject.priv --name "My Game" --version 1.0
//   arx_verify -i game.aex -p myproject.pub
//
// ==============================================================================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/stat.h>
#include <dirent.h>

#include "arx/format/aex.h"
#include "arx/crypto/ed25519.h"

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

// Recorrer directorio recursivamente y añadir archivos al builder
static AexResult add_directory_recursive(AexBuilder* b, const char* base_dir, const char* rel_prefix) {
    DIR* dir = opendir(base_dir);
    if (!dir) {
        fprintf(stderr, "Error: no se pudo abrir '%s'\n", base_dir);
        return AEX_ERR_IO;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char full_path[2048];
        snprintf(full_path, sizeof(full_path), "%s/%s", base_dir, entry->d_name);

        char rel_path[2048];
        if (rel_prefix[0])
            snprintf(rel_path, sizeof(rel_path), "%s/%s", rel_prefix, entry->d_name);
        else
            snprintf(rel_path, sizeof(rel_path), "%s", entry->d_name);

        struct stat st;
        if (stat(full_path, &st) != 0) continue;

        if (S_ISDIR(st.st_mode)) {
            // Recursión
            AexResult r = add_directory_recursive(b, full_path, rel_path);
            if (r != AEX_OK) { closedir(dir); return r; }
        } else if (S_ISREG(st.st_mode)) {
            // Leer archivo completo
            uint8_t* file_data = NULL;
            size_t file_size = 0;
            if (!read_file(full_path, &file_data, &file_size)) {
                fprintf(stderr, "Error leyendo '%s'\n", full_path);
                closedir(dir);
                return AEX_ERR_IO;
            }
            // Añadir al builder
            AexResult r = aex_builder_add_asset(b, rel_path, file_data, file_size);
            free(file_data);
            if (r != AEX_OK) {
                fprintf(stderr, "Error añadiendo '%s': %s\n", rel_path, aex_strerror(r));
                closedir(dir);
                return r;
            }
            printf("  + %s (%zu bytes)\n", rel_path, file_size);
        }
    }
    closedir(dir);
    return AEX_OK;
}

// Generar manifest JSON básico
static char* generate_manifest(const char* name, const char* version) {
    char* json = (char*)malloc(4096);
    if (!json) return NULL;

    snprintf(json, 4096,
        "{\n"
        "  \"schema\": 1,\n"
        "  \"package\": {\n"
        "    \"name\": \"%s\",\n"
        "    \"version\": \"%s\",\n"
        "    \"creator\": { \"name\": \"arx_pack\", \"pubkey\": \"\" }\n"
        "  },\n"
        "  \"engine\": { \"min_version\": \"0.0.1\" },\n"
        "  \"permissions\": {\n"
        "    \"network\":     { \"allowed\": false },\n"
        "    \"filesystem\":  { \"allowed\": true, \"scope\": \"sandbox:rw\" },\n"
        "    \"audio\":       { \"allowed\": true },\n"
        "    \"camera\":      { \"allowed\": false },\n"
        "    \"microphone\":  { \"allowed\": false },\n"
        "    \"location\":    { \"allowed\": false },\n"
        "    \"clipboard\":   { \"allowed\": false },\n"
        "    \"notifications\": { \"allowed\": false },\n"
        "    \"fullscreen\":  { \"allowed\": true },\n"
        "    \"sleep_block\": { \"allowed\": false }\n"
        "  },\n"
        "  \"assets\": { \"entry_scene\": \"\", \"asset_count\": 0, \"total_size_uncompressed\": 0 },\n"
        "  \"scripts\": { \"entry\": \"main.zen\", \"type\": 0, \"aot_compiled\": false },\n"
        "  \"metadata\": { \"title\": \"%s\", \"description\": \"\", \"language\": \"es\", \"icon\": \"\", \"tags\": [] }\n"
        "}\n",
        name ? name : "unnamed",
        version ? version : "0.0.1",
        name ? name : "unnamed"
    );
    return json;
}

static void usage(const char* prog) {
    fprintf(stderr,
        "ARX Pack — Empaqueta un directorio en .aex firmado\n\n"
        "Uso:\n"
        "  %s --input <dir> --output <file.aex> --priv <key.priv> [options]\n\n"
        "Opciones:\n"
        "  --input, -i <dir>       Directorio de assets a empaquetar\n"
        "  --output, -o <file>     Archivo .aex de salida\n"
        "  --priv <key.priv>       Clave privada Ed25519 (64 bytes)\n"
        "  --manifest <file.json>  Manifest JSON personalizado (opcional)\n"
        "  --scripts <dir>         Directorio de scripts (opcional)\n"
        "  --compressed            Comprimir assets con zstd\n"
        "  --name <name>           Nombre del paquete (default: unnamed)\n"
        "  --version <ver>         Versión (default: 0.0.1)\n",
        prog);
}

int main(int argc, char** argv) {
    const char* input_dir = NULL;
    const char* output_path = NULL;
    const char* priv_path = NULL;
    const char* manifest_path = NULL;
    const char* scripts_dir = NULL;
    const char* pkg_name = NULL;
    const char* pkg_version = NULL;
    bool compressed = false;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--input") == 0 || strcmp(argv[i], "-i") == 0) {
            if (i + 1 < argc) input_dir = argv[++i];
        } else if (strcmp(argv[i], "--output") == 0 || strcmp(argv[i], "-o") == 0) {
            if (i + 1 < argc) output_path = argv[++i];
        } else if (strcmp(argv[i], "--priv") == 0) {
            if (i + 1 < argc) priv_path = argv[++i];
        } else if (strcmp(argv[i], "--manifest") == 0) {
            if (i + 1 < argc) manifest_path = argv[++i];
        } else if (strcmp(argv[i], "--scripts") == 0) {
            if (i + 1 < argc) scripts_dir = argv[++i];
        } else if (strcmp(argv[i], "--compressed") == 0) {
            compressed = true;
        } else if (strcmp(argv[i], "--name") == 0) {
            if (i + 1 < argc) pkg_name = argv[++i];
        } else if (strcmp(argv[i], "--version") == 0) {
            if (i + 1 < argc) pkg_version = argv[++i];
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            usage(argv[0]);
            return 0;
        }
    }

    if (!input_dir || !output_path || !priv_path) {
        fprintf(stderr, "Error: faltan argumentos obligatorios\n");
        usage(argv[0]);
        return 2;
    }

    // Leer clave privada
    uint8_t* priv_data = NULL;
    size_t priv_size = 0;
    if (!read_file(priv_path, &priv_data, &priv_size)) {
        fprintf(stderr, "Error: no se pudo leer '%s'\n", priv_path);
        return 2;
    }
    if (priv_size != ARX_ED25519_PRIVKEY_SIZE) {
        fprintf(stderr, "Error: clave privada debe ser %d bytes, no %zu\n",
                ARX_ED25519_PRIVKEY_SIZE, priv_size);
        free(priv_data);
        return 2;
    }

    // Crear builder
    AexBuilder* b = aex_builder_new();
    if (!b) {
        fprintf(stderr, "Error: no se pudo crear builder\n");
        free(priv_data);
        return 2;
    }

    // Setear clave privada (aex_builder_set_creator_key espera el seed de 32 bytes,
    // que es la primera mitad de la clave privada de 64 bytes)
    AexResult r = aex_builder_set_creator_key(b, priv_data);
    if (r != AEX_OK) {
        fprintf(stderr, "Error seteando clave: %s\n", aex_strerror(r));
        aex_builder_free(b); free(priv_data);
        return 2;
    }

    // Setear manifest
    char* manifest_json = NULL;
    if (manifest_path) {
        uint8_t* mdata = NULL;
        size_t msize = 0;
        if (!read_file(manifest_path, &mdata, &msize)) {
            fprintf(stderr, "Error: no se pudo leer manifest '%s'\n", manifest_path);
            aex_builder_free(b); free(priv_data);
            return 2;
        }
        manifest_json = (char*)malloc(msize + 1);
        memcpy(manifest_json, mdata, msize);
        manifest_json[msize] = 0;
        free(mdata);
    } else {
        manifest_json = generate_manifest(pkg_name, pkg_version);
    }

    r = aex_builder_set_manifest(b, manifest_json);
    free(manifest_json);
    if (r != AEX_OK) {
        fprintf(stderr, "Error seteando manifest: %s\n", aex_strerror(r));
        aex_builder_free(b); free(priv_data);
        return 2;
    }

    // Compresión
    if (compressed) {
        aex_builder_set_compressed(b, true);
    }

    // Añadir assets
    printf("Empaquetando '%s' → '%s'...\n", input_dir, output_path);
    r = add_directory_recursive(b, input_dir, "");
    if (r != AEX_OK) {
        aex_builder_free(b); free(priv_data);
        return 1;
    }

    // Añadir scripts si se proporcionaron
    if (scripts_dir) {
        printf("\nAñadiendo scripts desde '%s'...\n", scripts_dir);
        // TODO: aex_builder_set_scripts() cuando la API esté lista
        printf("  (scripts: API no implementada todavía)\n");
    }

    // Escribir .aex
    printf("\nEscribiendo '%s'...\n", output_path);
    r = aex_builder_write(b, output_path);
    if (r != AEX_OK) {
        fprintf(stderr, "Error escribiendo: %s\n", aex_strerror(r));
        aex_builder_free(b); free(priv_data);
        return 1;
    }

    printf("✓ Paquete .aex creado: %s\n", output_path);

    aex_builder_free(b);
    free(priv_data);
    return 0;
}
