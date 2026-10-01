// test_integration.c — Test integrado: .aex con permisos + sandbox.
//
// Crea un .aex con manifest complejo (permisos de red + FS + clipboard),
// lo abre, parsea el manifest, configura un sandbox, y verifica que los
// checks funcionen correctamente.

#include "arx/format/aex.h"
#include "arx/format/sandbox.h"
#include "arx/crypto/ed25519.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static const char* MANIFEST =
    "{\"schema\":1,"
    "\"package\":{\"name\":\"mmo_demo\",\"version\":\"1.0.0\"},"
    "\"engine\":{\"min_version\":\"0.0.1\"},"
    "\"permissions\":{"
    "\"network\":{\"allowed\":true,\"endpoints\":[\"*.mmo.com:7777\"]},"
    "\"filesystem\":{\"allowed\":true,\"scope\":\"sandbox:/data/mmo/\"},"
    "\"audio\":{\"allowed\":true},"
    "\"camera\":{\"allowed\":false},"
    "\"microphone\":{\"allowed\":false},"
    "\"clipboard\":{\"allowed\":true,\"read\":true,\"write\":true}"
    "},"
    "\"scripts\":{\"entry\":\"main.arx\",\"type\":\"arxscript_source\"}}";

int main(void) {
    printf("=== Test integrado: .aex + sandbox ===\n");

    uint8_t priv[64], pub[32];
    if (!arx_ed25519_keygen(priv, pub)) {
        printf("SKIP: ed25519 no disponible\n");
        return 77;
    }

    // 1. Crear .aex con manifest + assets + scripts
    AexBuilder* b = aex_builder_new();
    assert(aex_builder_set_manifest(b, MANIFEST) == AEX_OK);
    assert(aex_builder_add_asset(b, "assets/textures/hero.png",
                                   (const uint8_t*)"PNG_DATA", 8) == AEX_OK);
    assert(aex_builder_add_asset(b, "assets/audio/jump.wav",
                                   (const uint8_t*)"WAV_DATA", 8) == AEX_OK);
    assert(aex_builder_set_scripts(b, AEX_SCRIPTS_ARXSCRIPT_CPP,
                                      (const uint8_t*)"print('hi')", 11) == AEX_OK);
    assert(aex_builder_set_creator_key(b, priv) == AEX_OK);

    const char* out = "/tmp/test_integration.aex";
    assert(aex_builder_write(b, out) == AEX_OK);
    printf("OK: %s creado\n", out);
    aex_builder_free(b);

    // 2. Abrir y verificar
    AexPackage* pkg = NULL;
    assert(aex_open(out, &pkg) == AEX_OK);
    assert(aex_verify_signature(pkg) == AEX_OK);
    printf("OK: abierto y firma verificada\n");

    // 3. Parsear manifest
    assert(aex_parse_manifest(pkg) == AEX_OK);
    const ArxManifest* m = aex_get_manifest(pkg);
    assert(m && m->valid);
    printf("OK: manifest parseado\n");
    printf("  package: %s v%s\n", m->package_name, m->package_version);
    printf("  engine.min_version: %s\n", m->engine_min_version);
    printf("  scripts.entry: %s\n", m->scripts_entry);
    printf("  scripts.type: %d\n", m->scripts_type);

    // 4. Verificar permisos del manifest
    assert(arx_manifest_has_permission(m, ARX_PERM_NETWORK));
    assert(arx_manifest_has_permission(m, ARX_PERM_FILESYSTEM));
    assert(arx_manifest_has_permission(m, ARX_PERM_AUDIO));
    assert(!arx_manifest_has_permission(m, ARX_PERM_CAMERA));
    assert(!arx_manifest_has_permission(m, ARX_PERM_MICROPHONE));
    assert(arx_manifest_has_permission(m, ARX_PERM_CLIPBOARD));
    printf("OK: permisos del manifest correctos\n");

    // 5. Verificar endpoints
    assert(arx_manifest_check_endpoint(m, "game.mmo.com:7777"));
    assert(arx_manifest_check_endpoint(m, "login.mmo.com:7777"));
    assert(!arx_manifest_check_endpoint(m, "evil.com:6666"));
    printf("OK: endpoints verificados\n");

    // 6. Crear sandbox
    ArxSandbox* sb = arx_sandbox_new(m);
    assert(sb);

    // 7. Checks del sandbox
    assert(arx_sandbox_can_network(sb, "game.mmo.com:7777"));
    assert(!arx_sandbox_can_network(sb, "evil.com:6666"));
    assert(arx_sandbox_can_play_audio(sb));
    assert(!arx_sandbox_can_use_camera(sb));
    assert(arx_sandbox_can_read_clipboard(sb));
    assert(arx_sandbox_can_write_clipboard(sb));

    // FS
    assert(arx_sandbox_can_read_file(sb, "res://textures/hero.png"));
    assert(!arx_sandbox_can_write_file(sb, "res://textures/hero.png"));
    assert(arx_sandbox_can_read_file(sb, "sandbox:/data/mmo/save.dat"));
    assert(arx_sandbox_can_write_file(sb, "sandbox:/data/mmo/save.dat"));
    assert(!arx_sandbox_can_write_file(sb, "/etc/passwd"));
    printf("OK: sandbox checks todos correctos\n");

    // 8. Leer un asset via aex_read_asset (con TAR parser)
    uint8_t* asset_data = NULL;
    size_t asset_size = 0;
    assert(aex_read_asset(pkg, "assets/textures/hero.png", &asset_data, &asset_size) == AEX_OK);
    assert(asset_size == 8);
    assert(memcmp(asset_data, "PNG_DATA", 8) == 0);
    printf("OK: asset leido via TAR parser\n");
    aex_free(asset_data);

    // 9. Revocar un permiso y verificar
    arx_sandbox_revoke(sb, ARX_PERM_NETWORK);
    assert(!arx_sandbox_can_network(sb, "game.mmo.com:7777"));
    printf("OK: revocacion de network funciona\n");

    arx_sandbox_free(sb);
    aex_close(pkg);

    printf("\nALL TESTS PASSED\n");
    return 0;
}
