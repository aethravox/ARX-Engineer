// arx/crypto/sha256.h
#ifndef ARX_CRYPTO_SHA256_H
#define ARX_CRYPTO_SHA256_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t data[64];
    uint32_t datalen;
    uint64_t bitlen;
    uint32_t state[8];
} arx_sha256_ctx;

void arx_sha256_init(arx_sha256_ctx* ctx);
void arx_sha256_update(arx_sha256_ctx* ctx, const uint8_t* data, size_t len);
void arx_sha256_final(arx_sha256_ctx* ctx, uint8_t out[32]);

// Helper one-shot
void arx_sha256(const uint8_t* data, size_t len, uint8_t out[32]);

// HMAC-SHA256 (RFC 2104)
void arx_hmac_sha256(const uint8_t* key, size_t key_len,
                       const uint8_t* msg, size_t msg_len,
                       uint8_t out[32]);

#ifdef __cplusplus
}
#endif

#endif // ARX_CRYPTO_SHA256_H
