// arx/crypto/ed25519.c — Wrapper Ed25519
//
// Estrategia:
//   1. Si ARX_CRYPTO_USE_SODIUM=1 y libsodium esta linkeado, usar
//      crypto_sign_ed25519_* (production-ready, optimizado).
//   2. Si no, usar la implementacion embebida ed25519_donna de orlp
//      (dominio publico), ubicada en common/src/crypto/ed25519_donna/.
//
// IMPORTANTE — formato de la clave privada:
//   - libsodium:    sk = seed(32) || pub(32)   -> 64 bytes
//   - orlp/ed25519: sk = scalar(32) || prefix(32)  -> 64 bytes
//   Para mantener compatibilidad entre ambos, ARX usa el formato de
//   libsodium (seed||pub). Cuando usamos orlp internamente, derivamos
//   el scalar/prefix on-the-fly.

#include "arx/crypto/ed25519.h"
#include "arx/crypto/sha256.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#if defined(ARX_CRYPTO_USE_SODIUM) && ARX_CRYPTO_USE_SODIUM
    #include <sodium.h>
#else
    #include "ed25519_donna.h"
#endif

// ============================================================
// Helper: derivar el scalar+prefix (formato orlp) desde un seed de 32 bytes
// ============================================================
#if !defined(ARX_CRYPTO_USE_SODIUM) || !ARX_CRYPTO_USE_SODIUM
static void derive_orlp_sk(const uint8_t seed[32], uint8_t out_sk64[64]) {
    // sk_orlp = SHA-512(seed), con clamping
    extern void sha512(const unsigned char* m, size_t n, unsigned char* out);  // de sha512_donna.c
    sha512(seed, 32, out_sk64);
    out_sk64[0]  &= 248;
    out_sk64[31] &= 63;
    out_sk64[31] |= 64;
}
#endif

// ============================================================
// API publica
// ============================================================

bool arx_ed25519_keygen(uint8_t out_priv[ARX_ED25519_PRIVKEY_SIZE],
                          uint8_t out_pub[ARX_ED25519_PUBKEY_SIZE]) {
#if defined(ARX_CRYPTO_USE_SODIUM) && ARX_CRYPTO_USE_SODIUM
    if (get_random_bytes(out_priv, 32) != 0) return false;
    crypto_sign_ed25519_seed_keypair(out_pub, out_priv, out_priv);
    memcpy(out_priv + 32, out_pub, 32);
    return true;
#else
    // orlp: generar seed aleatorio
    unsigned char seed[32];
    if (ed25519_create_seed(seed) != 0) return false;
    // Derivar pubkey (y el scalar+prefix interno)
    unsigned char orlp_sk[64];
    ed25519_create_keypair(out_pub, orlp_sk, seed);
    // En ARX guardamos priv = seed(32) || pub(32)
    memcpy(out_priv, seed, 32);
    memcpy(out_priv + 32, out_pub, 32);
    return true;
#endif
}

bool arx_ed25519_derive_pub(const uint8_t priv[ARX_ED25519_PRIVKEY_SIZE],
                              uint8_t out_pub[ARX_ED25519_PUBKEY_SIZE]) {
#if defined(ARX_CRYPTO_USE_SODIUM) && ARX_CRYPTO_USE_SODIUM
    // priv es 64 bytes (seed||pub); devolver pub
    memcpy(out_pub, priv + 32, 32);
    return true;
#else
    // priv puede ser:
    //   - 32 bytes (solo seed): derivar pub
    //   - 64 bytes (seed||pub): devolver priv[32..64]
    // Heuristicamente: si priv[32..64] es distinto de cero, asumir formato 64.
    // Para evitar ambiguedad, el caller debe asegurar que priv sea 64 bytes.
    // Si los ultimos 32 bytes son todos cero, asumir que es solo seed.
    int looks_like_seed_only = 1;
    for (int i = 32; i < 64; i++) {
        if (priv[i] != 0) { looks_like_seed_only = 0; break; }
    }
    if (looks_like_seed_only) {
        // Derivar pub desde seed (priv[0..32])
        unsigned char orlp_sk[64];
        unsigned char tmp_pub[32];
        ed25519_create_keypair(tmp_pub, orlp_sk, priv);
        memcpy(out_pub, tmp_pub, 32);
        return true;
    }
    // Ya tiene pub en priv[32..64]
    memcpy(out_pub, priv + 32, 32);
    return true;
#endif
}

bool arx_ed25519_sign(const uint8_t* msg, size_t msg_len,
                        const uint8_t priv[ARX_ED25519_PRIVKEY_SIZE],
                        uint8_t out_sig[ARX_ED25519_SIG_SIZE]) {
#if defined(ARX_CRYPTO_USE_SODIUM) && ARX_CRYPTO_USE_SODIUM
    unsigned long long sig_len = 0;
    if (crypto_sign_ed25519_detached(out_sig, &sig_len, msg, msg_len, priv) != 0)
        return false;
    return sig_len == ARX_ED25519_SIG_SIZE;
#else
    // priv = seed(32) || pub(32). Necesitamos:
    //   - pub = priv[32..64]
    //   - orlp_sk = SHA-512(priv[0..32]) clamped
    unsigned char orlp_sk[64];
    derive_orlp_sk(priv, orlp_sk);
    ed25519_sign(out_sig, msg, msg_len, priv + 32, orlp_sk);
    return true;
#endif
}

bool arx_ed25519_verify(const uint8_t* msg, size_t msg_len,
                          const uint8_t pub[ARX_ED25519_PUBKEY_SIZE],
                          const uint8_t sig[ARX_ED25519_SIG_SIZE]) {
#if defined(ARX_CRYPTO_USE_SODIUM) && ARX_CRYPTO_USE_SODIUM
    return crypto_sign_ed25519_verify_detached(sig, msg, msg_len, pub) == 0;
#else
    return ed25519_verify(sig, msg, msg_len, pub) != 0;
#endif
}
