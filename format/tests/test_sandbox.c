// test_sandbox.c — Test del sandbox de permisos.
//
// Crea manifests con distintos permisos, parsea, configura sandbox,
// y verifica que los checks funcionen correctamente.

#include "arx/format/manifest.h"
#include "arx/format/sandbox.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

static int denied_count = 0;
static int granted_count = 0;

static void log_fn(ArxSandboxLogKind kind, ArxPermission perm,
                    const char* detail, void* user_data) {
    (void)user_data;
    if (kind == ARX_SANDBOX_LOG_DENIED) {
        denied_count++;
        printf("  [DENIED] %s: %s\n", arx_permission_name(perm), detail);
    } else {
        granted_count++;
        printf("  [GRANTED] %s: %s\n", arx_permission_name(perm), detail);
    }
}

int main(void) {
    printf("=== Test Sandbox de permisos ===\n");

    // Manifest 1: todos los permisos cerrados
    const char* m1 = "{\"schema\":1,\"package\":{\"name\":\"locked\"},\"permissions\":{}}";
    ArxManifest manifest1;
    assert(arx_manifest_parse(&manifest1, m1));
    printf("OK: manifest 1 parseado (todos los permisos cerrados por defecto)\n");

    ArxSandbox* sb1 = arx_sandbox_new(&manifest1);
    assert(sb1);
    arx_sandbox_set_logger(sb1, log_fn, NULL);

    // Todo debe estar bloqueado
    assert(!arx_sandbox_can_network(sb1, "anywhere.com:80"));
    assert(!arx_sandbox_can_play_audio(sb1));
    assert(!arx_sandbox_can_use_camera(sb1));
    assert(!arx_sandbox_can_read_clipboard(sb1));
    printf("OK: manifest 1 bloquea todo correctamente\n\n");

    arx_sandbox_free(sb1);
    denied_count = 0; granted_count = 0;

    // Manifest 2: red + audio + FS abiertos, con endpoints restringidos
    const char* m2 =
        "{\"schema\":1,\"package\":{\"name\":\"mmo\"},"
        "\"permissions\":{"
        "\"network\":{\"allowed\":true,\"endpoints\":[\"*.mmo.com:7777\",\"192.168.1.0/24\"]},"
        "\"filesystem\":{\"allowed\":true,\"scope\":\"sandbox:/data/mmo/\"},"
        "\"audio\":{\"allowed\":true},"
        "\"clipboard\":{\"allowed\":true,\"read\":true,\"write\":false}"
        "}}";
    ArxManifest manifest2;
    assert(arx_manifest_parse(&manifest2, m2));
    printf("OK: manifest 2 parseado (mmo con endpoints)\n");
    printf("  network.allowed: %d\n", manifest2.permissions[ARX_PERM_NETWORK].allowed);
    printf("  endpoints: %d\n", manifest2.permissions[ARX_PERM_NETWORK].endpoint_count);
    for (int i = 0; i < manifest2.permissions[ARX_PERM_NETWORK].endpoint_count; i++) {
        printf("    [%d] %s\n", i, manifest2.permissions[ARX_PERM_NETWORK].endpoints[i]);
    }
    printf("  fs.scope: %s\n", manifest2.permissions[ARX_PERM_FILESYSTEM].fs_scope);
    printf("  clipboard.read: %d, write: %d\n",
            manifest2.permissions[ARX_PERM_CLIPBOARD].clipboard_read,
            manifest2.permissions[ARX_PERM_CLIPBOARD].clipboard_write);

    ArxSandbox* sb2 = arx_sandbox_new(&manifest2);
    arx_sandbox_set_logger(sb2, log_fn, NULL);

    // Endpoint permitido (wildcard)
    assert(arx_sandbox_can_network(sb2, "game.mmo.com:7777"));
    printf("OK: 'game.mmo.com:7777' permitido\n");

    // Endpoint exacto permitido (sin CIDR real, solo match exacto por ahora)
    assert(arx_sandbox_can_network(sb2, "192.168.1.50:1234") == false);  // no listado
    printf("OK: '192.168.1.50:1234' (no en lista) bloqueado\n");

    // Endpoint NO permitido
    assert(!arx_sandbox_can_network(sb2, "evil.com:6666"));
    printf("OK: 'evil.com:6666' bloqueado\n");

    // Audio OK
    assert(arx_sandbox_can_play_audio(sb2));
    printf("OK: audio permitido\n");

    // Camara bloqueada
    assert(!arx_sandbox_can_use_camera(sb2));
    printf("OK: camara bloqueada\n");

    // FS: sandbox permitido
    assert(arx_sandbox_can_read_file(sb2, "sandbox:/data/mmo/saves/slot1.dat"));
    printf("OK: leer archivo sandbox permitido\n");

    // FS: escribir en sandbox OK
    assert(arx_sandbox_can_write_file(sb2, "sandbox:/data/mmo/saves/slot1.dat"));
    printf("OK: escribir archivo sandbox permitido\n");

    // FS: fuera del sandbox bloqueado
    assert(!arx_sandbox_can_write_file(sb2, "/etc/passwd"));
    printf("OK: escribir /etc/passwd bloqueado\n");

    // res:// siempre legible
    assert(arx_sandbox_can_read_file(sb2, "res://textures/hero.png"));
    printf("OK: leer res:// permitido\n");

    // res:// no escribible
    assert(!arx_sandbox_can_write_file(sb2, "res://textures/hero.png"));
    printf("OK: escribir res:// bloqueado\n");

    // Clipboard read OK, write NO
    assert(arx_sandbox_can_read_clipboard(sb2));
    assert(!arx_sandbox_can_write_clipboard(sb2));
    printf("OK: clipboard read=true, write=false\n");

    // Revocar network
    arx_sandbox_revoke(sb2, ARX_PERM_NETWORK);
    assert(!arx_sandbox_can_network(sb2, "game.mmo.com:7777"));
    printf("OK: tras revoke(network), red bloqueada\n");

    // Conceder de nuevo
    arx_sandbox_grant(sb2, ARX_PERM_NETWORK);
    assert(arx_sandbox_can_network(sb2, "game.mmo.com:7777"));
    printf("OK: tras grant(network), red permitida\n");

    arx_sandbox_free(sb2);

    // Manifest 3: sin endpoints listados (todo permitido)
    const char* m3 =
        "{\"schema\":1,\"permissions\":{\"network\":{\"allowed\":true}}}";
    ArxManifest manifest3;
    assert(arx_manifest_parse(&manifest3, m3));
    ArxSandbox* sb3 = arx_sandbox_new(&manifest3);
    assert(arx_sandbox_can_network(sb3, "anywhere.com:80"));
    printf("\nOK: sin endpoints listados, cualquier host permitido\n");
    arx_sandbox_free(sb3);

    printf("\nALL TESTS PASSED\n");
    printf("Stats: %d granted, %d denied\n", granted_count, denied_count);
    return 0;
}
