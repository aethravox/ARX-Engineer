// arx/format/sandbox.h — API de enforcement de permisos.
//
// El sandbox es la capa del ARX Client que bloquea accesos no autorizados
// antes de que el juego (.aex) pueda tocar FS, red, audio, etc.
//
// Flujo:
//   1. aex_open() carga el .aex
//   2. aex_verify_signature() valida la firma
//   3. aex_get_manifest() parsea el manifest
//   4. arx_sandbox_init() configura el sandbox con los permisos del manifest
//      + la politica del usuario (que puede ser mas restrictiva)
//   5. Antes de cada operacion del juego, el runtime llama a:
//      - arx_sandbox_can_network(sandbox, "host:port")
//      - arx_sandbox_can_read_file(sandbox, "/path")
//      - arx_sandbox_can_write_file(sandbox, "/path")
//      - arx_sandbox_can_play_audio(sandbox)
//      - etc.
//      Si devuelve false, la operacion se bloquea y se loguea.

#ifndef ARX_FORMAT_SANDBOX_H
#define ARX_FORMAT_SANDBOX_H

#include "arx/format/manifest.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ArxSandbox ArxSandbox;

// Crea un sandbox con los permisos del manifest.
// El manifest debe estar parseado (arx_manifest_parse).
// Retorna NULL si falla.
ArxSandbox* arx_sandbox_new(const ArxManifest* manifest);

// Libera el sandbox.
void arx_sandbox_free(ArxSandbox* sb);

// ============================================================
// Checks por categoria
// ============================================================

// ¿El juego puede abrir una conexion a host:port?
bool arx_sandbox_can_network(const ArxSandbox* sb, const char* host_port);

// ¿El juego puede leer un archivo del FS virtual?
// path puede ser "sandbox:/data/<pkg>/..." o "res://..." (assets empaquetados)
bool arx_sandbox_can_read_file(const ArxSandbox* sb, const char* path);

// ¿El juego puede escribir un archivo?
bool arx_sandbox_can_write_file(const ArxSandbox* sb, const char* path);

// ¿Puede reproducir audio?
bool arx_sandbox_can_play_audio(const ArxSandbox* sb);

// ¿Puede usar la camara?
bool arx_sandbox_can_use_camera(const ArxSandbox* sb);

// ¿Puede usar el microfono?
bool arx_sandbox_can_use_microphone(const ArxSandbox* sb);

// ¿Puede acceder a la ubicacion GPS?
bool arx_sandbox_can_get_location(const ArxSandbox* sb);

// ¿Puede leer el portapapeles?
bool arx_sandbox_can_read_clipboard(const ArxSandbox* sb);

// ¿Puede escribir el portapapeles?
bool arx_sandbox_can_write_clipboard(const ArxSandbox* sb);

// ¿Puede mostrar notificaciones del SO?
bool arx_sandbox_can_notify(const ArxSandbox* sb);

// ¿Puede entrar en modo pantalla completa?
bool arx_sandbox_can_fullscreen(const ArxSandbox* sb);

// ¿Puede bloquear el sleep del SO (para juegos)?
bool arx_sandbox_can_block_sleep(const ArxSandbox* sb);

// ============================================================
// Politica del usuario (override)
// ============================================================

// El usuario puede revocar permisos que el manifest pide.
// Por defecto, todos los permisos del manifest se conceden si el usuario
// aprobo el paquete. Pero el usuario puede revocar individualmente.
void arx_sandbox_revoke(ArxSandbox* sb, ArxPermission perm);

// O conceder permisos extra (para testing).
void arx_sandbox_grant(ArxSandbox* sb, ArxPermission perm);

// ============================================================
// Logging
// ============================================================

typedef enum {
    ARX_SANDBOX_LOG_DENIED = 0,
    ARX_SANDBOX_LOG_GRANTED = 1,
} ArxSandboxLogKind;

// Callback para loguear decisiones del sandbox.
typedef void (*ArxSandboxLogFn)(ArxSandboxLogKind kind,
                                  ArxPermission perm,
                                  const char* detail,
                                  void* user_data);

void arx_sandbox_set_logger(ArxSandbox* sb, ArxSandboxLogFn fn, void* user_data);

#ifdef __cplusplus
}
#endif

#endif // ARX_FORMAT_SANDBOX_H
