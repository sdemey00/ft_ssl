#include "ft_ssl.h"
#include "sha256.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// SHA - 256

static const uint32_t g_k[64] =
{
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
    0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
    0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

static uint32_t rotr(uint32_t x, uint32_t n)
{
    return (x >> n) | (x << (32 - n));
}

static uint32_t ch(uint32_t x, uint32_t y, uint32_t z)
{
    return (x & y) ^ (~x & z);
}

static uint32_t maj(uint32_t x, uint32_t y, uint32_t z)
{
    return (x & y) ^ (x & z) ^ (y & z);
}

static uint32_t big_sigma0(uint32_t x)
{
    return rotr(x, 2) ^ rotr(x, 13) ^ rotr(x, 22);
}

static uint32_t big_sigma1(uint32_t x)
{
    return rotr(x, 6) ^ rotr(x, 11) ^ rotr(x, 25);
}

static uint32_t small_sigma0(uint32_t x)
{
    return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3);
}

static uint32_t small_sigma1(uint32_t x)
{
    return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10);
}


static void sha256_compress(t_sha256_ctx *ctx, const uint8_t *block)
{
    uint32_t w[64];
    uint32_t a, b, c, d, e, f, g, h;
    uint32_t t1, t2;
    int      i;

    /*
     * SHA-256 is BIG-ENDIAN.
     *
     * block[0] is the HIGH byte of w[0].
     */
    i = 0;
    while (i < 16)
    {
        w[i] = ((uint32_t)block[i * 4] << 24)
             | ((uint32_t)block[i * 4 + 1] << 16)
             | ((uint32_t)block[i * 4 + 2] << 8)
             | ((uint32_t)block[i * 4 + 3]);
        i++;
    }

    /*
     * Expand 16 words -> 64 words.
     */
    while (i < 64)
    {
        w[i] = small_sigma1(w[i - 2])
             + w[i - 7]
             + small_sigma0(w[i - 15])
             + w[i - 16];
        i++;
    }

    a = ctx->state[0];
    b = ctx->state[1];
    c = ctx->state[2];
    d = ctx->state[3];
    e = ctx->state[4];
    f = ctx->state[5];
    g = ctx->state[6];
    h = ctx->state[7];

    i = 0;
    while (i < 64)
    {
        t1 = h + big_sigma1(e) + ch(e, f, g)
           + g_k[i] + w[i];

        t2 = big_sigma0(a) + maj(a, b, c);

        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;

        i++;
    }

    ctx->state[0] += a;
    ctx->state[1] += b;
    ctx->state[2] += c;
    ctx->state[3] += d;
    ctx->state[4] += e;
    ctx->state[5] += f;
    ctx->state[6] += g;
    ctx->state[7] += h;
}

/*
MD5                         SHA-256
------------------------------------------------
w[16]                       w[64]
little endian               big endian
a b c d                      a b c d e f g h
4 state words                8 state words
64 rounds                    64 rounds
leftrotate                   rightrotate
F/G/H/I                      Ch/Maj
*/

void sha256_init(void *state)
{
    t_sha256_ctx *ctx = (t_sha256_ctx *)state;

    ctx->state[0] = 0x6a09e667;
    ctx->state[1] = 0xbb67ae85;
    ctx->state[2] = 0x3c6ef372;
    ctx->state[3] = 0xa54ff53a;
    ctx->state[4] = 0x510e527f;
    ctx->state[5] = 0x9b05688c;
    ctx->state[6] = 0x1f83d9ab;
    ctx->state[7] = 0x5be0cd19;

    ctx->bitlen = 0;
    ctx->buffer_len = 0;
}

static void sha256_update_raw(t_sha256_ctx *ctx, const uint8_t *data, size_t len) {
    size_t i = 0;

    while (i < len)
    {
        ctx->buffer[ctx->buffer_len++] = data[i++];
        if (ctx->buffer_len == 64)
        {
            DEBUG_PRINT("sha256: compressing full block\n");
            sha256_compress(ctx, ctx->buffer);
            ctx->buffer_len = 0;
        }
    }
}

void sha256_update(void *state, const uint8_t *data, size_t len)
{
    t_sha256_ctx *ctx = (t_sha256_ctx *)state;

    sha256_update_raw(ctx, data, len);
    ctx->bitlen += (uint64_t)len * 8;
    DEBUG_PRINT("sha256_update: +%zu bytes, bitlen now %lu, buffer_len=%zu\n", len, ctx->bitlen, ctx->buffer_len);
}

void sha256_final(void *state, uint8_t *out)
{
    t_sha256_ctx *ctx = (t_sha256_ctx *)state;
    uint64_t bitlen;
    uint8_t  pad;
    uint8_t  zero;
    uint8_t  b;
    int      i;

    bitlen = ctx->bitlen;
    pad = 0x80;
    zero = 0x00;

    DEBUG_PRINT("sha256_final: original bitlen=%lu, leftover buffer_len=%zu\n", bitlen, ctx->buffer_len);
    sha256_update_raw(ctx, &pad, 1);
    DEBUG_PRINT("sha256_final: appended 0x80, buffer_len now %zu\n", ctx->buffer_len);

    while (ctx->buffer_len != 56)
        sha256_update_raw(ctx, &zero, 1);
    DEBUG_PRINT("sha256_final: zero-padded to buffer_len=56\n");

    i = 7;
    while (i >= 0)
    {
        b = (uint8_t)(bitlen >> (i * 8));
        sha256_update_raw(ctx, &b, 1);
        i--;
    }

    i = 0;
    while (i < 8)
    {
        out[i * 4]     = (uint8_t)(ctx->state[i] >> 24);
        out[i * 4 + 1] = (uint8_t)(ctx->state[i] >> 16);
        out[i * 4 + 2] = (uint8_t)(ctx->state[i] >> 8);
        out[i * 4 + 3] = (uint8_t)(ctx->state[i]);
        i++;
    }
    DEBUG_PRINT("sha256_final: digest = %02x%02x%02x%02x...\n", out[0], out[1], out[2], out[3]);
}
