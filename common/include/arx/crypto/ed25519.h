// arx/crypto/ed25519.h — Wrapper de Ed25519 sobre libsodium o supraedori

#ifndef ARX_CRYPTO_ED25519_H
#define ARX_CRYPTO_ED25519_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ARX_ED25519_PUBKEY_SIZE  32
#define ARX_ED25519_PRIVKEY_SIZE 64  // seed(32) || pubkey(32)
#define ARX_ED25519_SEED_SIZE    32
#define ARX_ED25519_SIG_SIZE     64

bool arx_ed25519_keygen(uint8_t out_priv[ARX_ED25519_PRIVKEY_SIZE],
                          uint8_t out_pub[ARX_ED25519_PUBKEY_SIZE]);

bool arx_ed25519_derive_pub(const uint8_t priv[ARX_ED25519_PRIVKEY_SIZE],
                              uint8_t out_pub[ARX_ED25519_PUBKEY_SIZE]);

bool arx_ed25519_sign(const uint8_t* msg, size_t msg_len,
                        const uint8_t priv[ARX_ED25519_PRIVKEY_SIZE],
                        uint8_t out_sig[ARX_ED25519_SIG_SIZE]);

bool arx_ed25519_verify(const uint8_t* msg, size_t msg_len,
                          const uint8_t pub[ARX_ED25519_PUBKEY_SIZE],
                          const uint8_t sig[ARX_ED25519_SIG_SIZE]);

void arx_sha256(const uint8_t* data, size_t len, uint8_t out[32]);

void arx_hmac_sha256(const uint8_t* key, size_t key_len,
                       const uint8_t* msg, size_t msg_len,
                       uint8_t out[32]);

#ifdef __cplusplus
}
#endif

#endif // ARX_CRYPTO_ED25519_H
