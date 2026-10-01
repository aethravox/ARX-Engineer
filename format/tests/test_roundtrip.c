// test_roundtrip.c — Test end-to-end del .aex
//
// 1. Crea un builder con manifest + 2 assets + scripts dummy
// 2. Genera par de claves Ed25519
// 3. Firma y escribe test.aex
// 4. Abre el .aex con aex_open
// 5. Verifica la firma con aex_verify_signature
// 6. Lee manifest + scripts y compara con lo original

#include "arx/format/aex.h"
#include "arx/crypto/ed25519.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static const char* TEST_MANIFEST =
    "{\"schema\":1,\"package\":{\"name\":\"test\",\"version\":\"0.1.0\"},"
    "\"permissions\":{\"network\":{\"allowed\":false},\"filesystem\":{\"allowed\":false}}}";

static const char* SCRIPTS_DUMMY = "-- luau bytecode placeholder\n";

int main(void) {
    printf("=== Test round-trip .aex ===\n");

    // 1. Generar par de claves
    uint8_t priv[64], pub[32];
    if (!arx_ed25519_keygen(priv, pub)) {
        printf("SKIP: arx_ed25519_keygen fallo (probablemente falta libsodium)\n");
        printf("Para ejecutar este test con firma real, instalar libsodium:\n");
        printf("  sudo apt install libsodium-dev\n");
        return 77;  // skip
    }
    printf("OK: par de claves generado\n");
    printf("  pub: ");
    for (int i = 0; i < 32; i++) printf("%02x", pub[i]);
    printf("\n");

    // 2. Construir .aex
    AexBuilder* b = aex_builder_new();
    assert(b);

    AexResult r = aex_builder_set_manifest(b, TEST_MANIFEST);
    assert(r == AEX_OK);

    r = aex_builder_add_asset(b, "assets/textures/hero.png",
                                 (const uint8_t*)"PNG_DUMMY_DATA", 14);
    assert(r == AEX_OK);

    r = aex_builder_add_asset(b, "assets/audio/jump.wav",
                                 (const uint8_t*)"WAV_DUMMY", 9);
    assert(r == AEX_OK);

    r = aex_builder_set_scripts(b, AEX_SCRIPTS_LUAU_BYTECODE_V3,
                                   (const uint8_t*)SCRIPTS_DUMMY,
                                   strlen(SCRIPTS_DUMMY));
    assert(r == AEX_OK);

    r = aex_builder_set_creator_key(b, priv);
    assert(r == AEX_OK);

    r = aex_builder_set_compressed(b, false);
    assert(r == AEX_OK);

    const char* out_path = "/tmp/test_roundtrip.aex";
    r = aex_builder_write(b, out_path);
    if (r != AEX_OK) {
        printf("FAIL: aex_builder_write: %s\n", aex_strerror(r));
        aex_builder_free(b);
        return 1;
    }
    printf("OK: %s escrito\n", out_path);
    aex_builder_free(b);

    // 3. Abrir el .aex
    AexPackage* pkg = NULL;
    r = aex_open(out_path, &pkg);
    if (r != AEX_OK) {
        printf("FAIL: aex_open: %s\n", aex_strerror(r));
        return 1;
    }
    printf("OK: aex_open\n");

    // 4. Verificar firma
    r = aex_verify_signature(pkg);
    if (r != AEX_OK) {
        printf("FAIL: aex_verify_signature: %s\n", aex_strerror(r));
        aex_close(pkg);
        return 1;
    }
    printf("OK: firma criptograficamente valida\n");

    // 5. Manifest
    const char* manifest = aex_get_manifest_json(pkg);
    if (!manifest || strcmp(manifest, TEST_MANIFEST) != 0) {
        printf("FAIL: manifest no coincide\n  got: %s\n  exp: %s\n",
               manifest ? manifest : "(null)", TEST_MANIFEST);
        aex_close(pkg);
        return 1;
    }
    printf("OK: manifest coincide\n");

    // 6. Scripts
    uint8_t* scripts_data = NULL;
    size_t scripts_size = 0;
    r = aex_read_scripts(pkg, &scripts_data, &scripts_size);
    if (r != AEX_OK) {
        printf("FAIL: aex_read_scripts: %s\n", aex_strerror(r));
        aex_close(pkg);
        return 1;
    }
    if (scripts_size != strlen(SCRIPTS_DUMMY) ||
        memcmp(scripts_data, SCRIPTS_DUMMY, scripts_size) != 0) {
        printf("FAIL: scripts no coinciden\n");
        aex_free(scripts_data);
        aex_close(pkg);
        return 1;
    }
    printf("OK: scripts coinciden (%zu bytes)\n", scripts_size);
    aex_free(scripts_data);

    // 7. Pubkey
    const uint8_t* read_pub = aex_get_creator_pubkey(pkg);
    if (!read_pub || memcmp(read_pub, pub, 32) != 0) {
        printf("FAIL: pubkey no coincide\n");
        aex_close(pkg);
        return 1;
    }
    printf("OK: pubkey coincide\n");

    aex_close(pkg);
    printf("\nALL TESTS PASSED\n");
    return 0;
}
