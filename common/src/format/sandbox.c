// arx/format/sandbox.c — Implementacion del sandbox.

#include "arx/format/sandbox.h"
#include "arx/format/manifest.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct ArxSandbox {
    ArxManifest manifest_copy;  // copia del manifest parseado
    bool revoked[ARX_PERM_COUNT];
    ArxSandboxLogFn log_fn;
    void* log_user_data;
};

ArxSandbox* arx_sandbox_new(const ArxManifest* manifest) {
    if (!manifest) return NULL;
    ArxSandbox* sb = calloc(1, sizeof(ArxSandbox));
    if (!sb) return NULL;
    sb->manifest_copy = *manifest;
    sb->log_fn = NULL;
    sb->log_user_data = NULL;
    return sb;
}

void arx_sandbox_free(ArxSandbox* sb) {
    free(sb);
}

void arx_sandbox_set_logger(ArxSandbox* sb, ArxSandboxLogFn fn, void* user_data) {
    if (!sb) return;
    sb->log_fn = fn;
    sb->log_user_data = user_data;
}

static bool check_perm(ArxSandbox* sb, ArxPermission perm, const char* detail) {
    if (!sb) return false;
    bool allowed = sb->manifest_copy.permissions[perm].allowed && !sb->revoked[perm];
    if (sb->log_fn) {
        sb->log_fn(allowed ? ARX_SANDBOX_LOG_GRANTED : ARX_SANDBOX_LOG_DENIED,
                    perm, detail ? detail : "", sb->log_user_data);
    }
    return allowed;
}

bool arx_sandbox_can_network(const ArxSandbox* sb, const char* host_port) {
    if (!sb) return false;
    // Doble check: permiso global + endpoint especifico
    bool base = sb->manifest_copy.permissions[ARX_PERM_NETWORK].allowed &&
                !sb->revoked[ARX_PERM_NETWORK];
    if (!base) {
        if (sb->log_fn) {
            sb->log_fn(ARX_SANDBOX_LOG_DENIED, ARX_PERM_NETWORK,
                        host_port ? host_port : "", sb->log_user_data);
        }
        return false;
    }
    bool ok = arx_manifest_check_endpoint(&sb->manifest_copy, host_port ? host_port : "");
    if (sb->log_fn) {
        sb->log_fn(ok ? ARX_SANDBOX_LOG_GRANTED : ARX_SANDBOX_LOG_DENIED,
                    ARX_PERM_NETWORK, host_port ? host_port : "", sb->log_user_data);
    }
    return ok;
}

bool arx_sandbox_can_read_file(const ArxSandbox* sb, const char* path) {
    if (!sb || !path) return false;
    // res:// (assets empaquetados) siempre se permite leer.
    if (strncmp(path, "res://", 6) == 0) return true;
    // sandbox:/ siempre se permite si filesystem.allowed
    if (strncmp(path, "sandbox:", 8) == 0) {
        return check_perm((ArxSandbox*)sb, ARX_PERM_FILESYSTEM, path);
    }
    // Cualquier otro path absoluto: bloquear a menos que FS esté permitido
    // (el runtime debe traducir paths absolutos a sandbox:/)
    return check_perm((ArxSandbox*)sb, ARX_PERM_FILESYSTEM, path);
}

bool arx_sandbox_can_write_file(const ArxSandbox* sb, const char* path) {
    if (!sb || !path) return false;
    // Escritura solo permitida dentro del sandbox del paquete
    if (strncmp(path, "sandbox:", 8) == 0) {
        return check_perm((ArxSandbox*)sb, ARX_PERM_FILESYSTEM, path);
    }
    // res:// es read-only
    if (strncmp(path, "res://", 6) == 0) return false;
    // Absolutos: bloquear
    if (sb->log_fn) {
        sb->log_fn(ARX_SANDBOX_LOG_DENIED, ARX_PERM_FILESYSTEM, path, sb->log_user_data);
    }
    return false;
}

bool arx_sandbox_can_play_audio(const ArxSandbox* sb) {
    return check_perm((ArxSandbox*)sb, ARX_PERM_AUDIO, "play");
}

bool arx_sandbox_can_use_camera(const ArxSandbox* sb) {
    return check_perm((ArxSandbox*)sb, ARX_PERM_CAMERA, "use");
}

bool arx_sandbox_can_use_microphone(const ArxSandbox* sb) {
    return check_perm((ArxSandbox*)sb, ARX_PERM_MICROPHONE, "use");
}

bool arx_sandbox_can_get_location(const ArxSandbox* sb) {
    return check_perm((ArxSandbox*)sb, ARX_PERM_LOCATION, "get");
}

bool arx_sandbox_can_read_clipboard(const ArxSandbox* sb) {
    if (!sb) return false;
    const ArxPermissionDetail* d = &sb->manifest_copy.permissions[ARX_PERM_CLIPBOARD];
    bool ok = d->allowed && d->clipboard_read && !sb->revoked[ARX_PERM_CLIPBOARD];
    if (sb->log_fn) {
        sb->log_fn(ok ? ARX_SANDBOX_LOG_GRANTED : ARX_SANDBOX_LOG_DENIED,
                    ARX_PERM_CLIPBOARD, "read", sb->log_user_data);
    }
    return ok;
}

bool arx_sandbox_can_write_clipboard(const ArxSandbox* sb) {
    if (!sb) return false;
    const ArxPermissionDetail* d = &sb->manifest_copy.permissions[ARX_PERM_CLIPBOARD];
    bool ok = d->allowed && d->clipboard_write && !sb->revoked[ARX_PERM_CLIPBOARD];
    if (sb->log_fn) {
        sb->log_fn(ok ? ARX_SANDBOX_LOG_GRANTED : ARX_SANDBOX_LOG_DENIED,
                    ARX_PERM_CLIPBOARD, "write", sb->log_user_data);
    }
    return ok;
}

bool arx_sandbox_can_notify(const ArxSandbox* sb) {
    return check_perm((ArxSandbox*)sb, ARX_PERM_NOTIFICATIONS, "notify");
}

bool arx_sandbox_can_fullscreen(const ArxSandbox* sb) {
    return check_perm((ArxSandbox*)sb, ARX_PERM_FULLSCREEN, "fullscreen");
}

bool arx_sandbox_can_block_sleep(const ArxSandbox* sb) {
    return check_perm((ArxSandbox*)sb, ARX_PERM_SLEEP_BLOCK, "block_sleep");
}

void arx_sandbox_revoke(ArxSandbox* sb, ArxPermission perm) {
    if (!sb || perm < 0 || perm >= ARX_PERM_COUNT) return;
    sb->revoked[perm] = true;
}

void arx_sandbox_grant(ArxSandbox* sb, ArxPermission perm) {
    if (!sb || perm < 0 || perm >= ARX_PERM_COUNT) return;
    sb->revoked[perm] = false;
    sb->manifest_copy.permissions[perm].allowed = true;
}
