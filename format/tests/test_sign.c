// test_sign.c — Test directo de Ed25519
//
// Genera un par, firma un mensaje, verifica la firma.
// Reporta OK o FAIL.

#include "arx/crypto/ed25519.h"
#include <stdio.h>
#include <string.h>

int main(void) {
    printf("=== Test Ed25519 ===\n");

    uint8_t priv[64], pub[32];
    if (!arx_ed25519_keygen(priv, pub)) {
        printf("SKIP: libsodium no disponible o keygen fallo\n");
        return 77;
    }
    printf("OK: keypair generado\n");

    const char* msg = "Hola ARX, este es un mensaje de prueba";
    size_t msg_len = strlen(msg);

    uint8_t sig[64];
    if (!arx_ed25519_sign((const uint8_t*)msg, msg_len, priv, sig)) {
        printf("FAIL: sign fallo\n");
        return 1;
    }
    printf("OK: firma generada (");
    for (int i = 0; i < 8; i++) printf("%02x", sig[i]);
    printf("...)\n");

    if (!arx_ed25519_verify((const uint8_t*)msg, msg_len, pub, sig)) {
        printf("FAIL: verify fallo para firma valida\n");
        return 1;
    }
    printf("OK: verify OK para firma valida\n");

    // Tamper: cambiar el mensaje y verificar que falla
    const char* bad_msg = "Hola ARX, este es un mensaje MODIFICADO";
    if (arx_ed25519_verify((const uint8_t*)bad_msg, strlen(bad_msg), pub, sig)) {
        printf("FAIL: verify acepto mensaje modificado (deberia rechazar)\n");
        return 1;
    }
    printf("OK: verify rechazo mensaje modificado\n");

    // Tamper: cambiar la firma
    sig[0] ^= 0x01;
    if (arx_ed25519_verify((const uint8_t*)msg, msg_len, pub, sig)) {
        printf("FAIL: verify acepto firma modificada (deberia rechazar)\n");
        return 1;
    }
    printf("OK: verify rechazo firma modificada\n");

    printf("\nALL TESTS PASSED\n");
    return 0;
}
