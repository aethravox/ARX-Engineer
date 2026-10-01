// arx/format/manifest.h — Parser del manifest JSON del .aex + modelo de permisos.
//
// Estructura del manifest (resumen, ver format/spec/SPEC.md para detalle):
// {
//   "schema": 1,
//   "package": { "name", "version", "creator": {...} },
//   "engine": { "min_version" },
//   "permissions": {
//     "network":     { "allowed": bool, "endpoints": [...] },
//     "filesystem":  { "allowed": bool, "scope": "sandbox:..." },
//     "audio":       { "allowed": bool },
//     "camera":      { "allowed": bool },
//     "microphone":  { "allowed": bool },
//     "location":    { "allowed": bool },
//     "clipboard":   { "allowed": bool, "read": bool, "write": bool },
//     "notifications": { "allowed": bool },
//     "fullscreen":  { "allowed": bool },
//     "sleep_block": { "allowed": bool }
//   },
//   "assets":  { "entry_scene", "asset_count", "total_size_uncompressed" },
//   "scripts": { "entry", "type", "aot_compiled" },
//   "metadata": { "title", "description", "language", "icon", "tags" }
// }

#ifndef ARX_FORMAT_MANIFEST_H
#define ARX_FORMAT_MANIFEST_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================
// Permisos soportados
// ============================================================

typedef enum {
    ARX_PERM_NETWORK       = 0,
    ARX_PERM_FILESYSTEM    = 1,
    ARX_PERM_AUDIO         = 2,
    ARX_PERM_CAMERA        = 3,
    ARX_PERM_MICROPHONE    = 4,
    ARX_PERM_LOCATION      = 5,
    ARX_PERM_CLIPBOARD     = 6,
    ARX_PERM_NOTIFICATIONS = 7,
    ARX_PERM_FULLSCREEN    = 8,
    ARX_PERM_SLEEP_BLOCK   = 9,
    ARX_PERM_COUNT
} ArxPermission;

// Detalle de un permiso
typedef struct {
    bool allowed;
    // Solo para NETWORK: lista de endpoints permitidos (CIDR o host:port con wildcards)
    // Implementacion simplificada: hasta 16 endpoints de hasta 128 chars cada uno.
    int  endpoint_count;
    char endpoints[16][128];
    // Solo para FILESYSTEM: scope del sandbox
    char fs_scope[256];
    // Solo para CLIPBOARD: read/write flags
    bool clipboard_read;
    bool clipboard_write;
} ArxPermissionDetail;

// Estructura completa del manifest parseado
typedef struct {
    int  schema;
    char package_name[128];
    char package_version[64];
    char creator_name[128];
    char creator_id[64];
    char creator_pubkey_fingerprint[80];
    char engine_min_version[32];

    ArxPermissionDetail permissions[ARX_PERM_COUNT];

    char entry_scene[256];
    int  asset_count;
    int64_t total_size_uncompressed;

    char scripts_entry[256];
    int  scripts_type;          // AEX_SCRIPTS_LUAU_BYTECODE_V3 / AOT_NATIVE / ARXSCRIPT_CPP
    bool aot_compiled;

    char title[256];
    char description[1024];
    char language[16];
    char icon_path[256];

    bool valid;                 // true si el manifest parseo OK
} ArxManifest;

// ============================================================
// API
// ============================================================

// Parsea un JSON de manifest. Retorna true si OK.
// Los campos faltantes se dejan con valores por defecto (strings vacios, bool false).
bool arx_manifest_parse(ArxManifest* out, const char* json);

// Verifica si un permiso esta concedido.
bool arx_manifest_has_permission(const ArxManifest* m, ArxPermission perm);

// Obtiene el detalle de un permiso (NULL si perm invalido).
const ArxPermissionDetail* arx_manifest_get_permission(const ArxManifest* m,
                                                          ArxPermission perm);

// Verifica si un endpoint de red esta dentro de los permitidos.
// Si el manifest no lista endpoints (endpoints_count==0) y network.allowed==true,
// se permite cualquier endpoint.
// Si lista endpoints, se hace match por sufijo o wildcard ('*' al inicio).
bool arx_manifest_check_endpoint(const ArxManifest* m, const char* host_port);

// Convierte un ArxPermission a string ("network", "filesystem", etc.).
const char* arx_permission_name(ArxPermission perm);

// Convierte un string a ArxPermission (-1 si no se reconoce).
int arx_permission_from_name(const char* name);

#ifdef __cplusplus
}
#endif

#endif // ARX_FORMAT_MANIFEST_H
