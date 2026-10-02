// arx/format/aex.h — Libreria de lectura/escritura del formato .aex
//
// Esta libreria es compartida entre:
//   - ARX Engine (para escribir .aex al exportar)
//   - ARX Client (para leer y verificar .aex antes de ejecutar)
//   - ARX Sign / Pack / Verify (CLI tools)
//
// Licencia: MIT
// Version API: 1.0

#ifndef ARX_FORMAT_AEX_H
#define ARX_FORMAT_AEX_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "arx/format/manifest.h"  // para ArxManifest en aex_get_manifest

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================
// Constantes
// ============================================================

#define AEX_MAGIC 0x31584541u        // "AEX1" little-endian
#define AEX_FORMAT_VERSION 1u
#define AEX_HEADER_SIZE 64
#define AEX_SIG_BLOCK_SIZE 292
#define AEX_PUBKEY_SIZE 32
#define AEX_SIGNATURE_SIZE 64
#define AEX_SHA256_SIZE 32

// Flags de cabecera (bitmask)
#define AEX_FLAG_COMPRESSED     0x01u   // assets comprimidos con zstd
#define AEX_FLAG_ENCRYPTED      0x02u   // assets encriptados (XOR-AES)
#define AEX_FLAG_AOT_COMPILED   0x04u   // scripts precompilados a nativo

// Tipos de scripts soportados
#define AEX_SCRIPTS_LUAU_BYTECODE_V3   1
#define AEX_SCRIPTS_AOT_NATIVE          2
#define AEX_SCRIPTS_ARXSCRIPT_CPP       3

// ============================================================
// Estructuras binarias (little-endian)
// ============================================================

#pragma pack(push, 1)

typedef struct {
    uint32_t magic;              // AEX_MAGIC
    uint16_t format_version;     // AEX_FORMAT_VERSION
    uint16_t flags;              // bitmask AEX_FLAG_*
    int64_t  created_unix;       // timestamp Unix
    int64_t  manifest_offset;
    int64_t  manifest_size;
    int64_t  assets_offset;
    int64_t  assets_size;
    int64_t  scripts_offset;
    int64_t  scripts_size;
} AexHeader;                     // 64 bytes

typedef struct {
    uint8_t  creator_pubkey[AEX_PUBKEY_SIZE];      // 32
    uint8_t  creator_signature[AEX_SIGNATURE_SIZE]; // 64
    uint8_t  engine_pubkey[AEX_PUBKEY_SIZE];        // 32 (opcional)
    uint8_t  engine_signature[AEX_SIGNATURE_SIZE];  // 64 (opcional)
    uint8_t  manifest_sha256[AEX_SHA256_SIZE];      // 32
    uint8_t  assets_sha256[AEX_SHA256_SIZE];        // 32
    uint8_t  scripts_sha256[AEX_SHA256_SIZE];       // 32
    uint32_t sig_block_version;                     // 4 (=1)
} AexSignatureBlock;             // 292 bytes

#pragma pack(pop)

// ============================================================
// Codigos de error
// ============================================================

typedef enum {
    AEX_OK = 0,
    AEX_ERR_IO = -1,
    AEX_ERR_MAGIC_MISMATCH = -2,
    AEX_ERR_VERSION_UNSUPPORTED = -3,
    AEX_ERR_CORRUPT_HEADER = -4,
    AEX_ERR_MANIFEST_INVALID_JSON = -5,
    AEX_ERR_MANIFEST_MISSING_FIELD = -6,
    AEX_ERR_HASH_MISMATCH = -7,
    AEX_ERR_SIGNATURE_INVALID = -8,
    AEX_ERR_PERMISSION_DENIED = -9,
    AEX_ERR_ASSET_BLOCK_CORRUPT = -10,
    AEX_ERR_SCRIPTS_BLOCK_CORRUPT = -11,
    AEX_ERR_SIGNATURE_BLOCK_INVALID = -12,
    AEX_ERR_NULL_ARG = -13,
} AexResult;

// ============================================================
// API de lectura (client-side)
// ============================================================

typedef struct AexPackage AexPackage;

AexResult aex_open(const char* path, AexPackage** out_pkg);
AexResult aex_verify_signature(const AexPackage* pkg);
const char* aex_get_manifest_json(const AexPackage* pkg);
const uint8_t* aex_get_creator_pubkey(const AexPackage* pkg);

// Devuelve el manifest parseado (NULL si no se ha llamado a aex_parse_manifest).
// El puntero pertenece al pkg, no liberar.
const ArxManifest* aex_get_manifest(const AexPackage* pkg);

// Parsea el manifest JSON (si no se ha hecho ya) y lo cachea en el pkg.
// Llama a esto despues de aex_open para acceder a los permisos.
AexResult aex_parse_manifest(AexPackage* pkg);

AexResult aex_read_asset(const AexPackage* pkg,
                          const char* asset_path,
                          uint8_t** out_data,
                          size_t* out_size);

// Devuelve un puntero a los bytes crudos del .aex en memoria (read-only).
// El puntero pertenece al pkg, no liberar.
// Usado por el runtime para __arx_syscall_get_aex_bytes (verificación mutua).
const uint8_t* aex_get_raw_bytes(const AexPackage* pkg, size_t* out_size);

AexResult aex_read_scripts(const AexPackage* pkg,
                            uint8_t** out_data,
                            size_t* out_size);

void aex_close(AexPackage* pkg);
void aex_free(void* ptr);
const char* aex_strerror(AexResult code);

// ============================================================
// API de escritura (engine-side)
// ============================================================

typedef struct AexBuilder AexBuilder;

AexBuilder* aex_builder_new(void);
void aex_builder_free(AexBuilder* b);

AexResult aex_builder_set_manifest(AexBuilder* b, const char* json);

AexResult aex_builder_add_asset(AexBuilder* b,
                                  const char* path,
                                  const uint8_t* data,
                                  size_t size);

AexResult aex_builder_set_scripts(AexBuilder* b,
                                    uint8_t script_type,
                                    const uint8_t* data,
                                    size_t size);

AexResult aex_builder_set_creator_key(AexBuilder* b,
                                        const uint8_t private_key[32]);

AexResult aex_builder_set_compressed(AexBuilder* b, bool compressed);

AexResult aex_builder_write(AexBuilder* b, const char* path);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ARX_FORMAT_AEX_H
