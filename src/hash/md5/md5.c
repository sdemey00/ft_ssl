#include "ft_ssl.h"
#include "md5.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Message Digest 5

static const uint32_t g_r[64] =
{
    7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,
    5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,
    4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,
    6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21
};

// floor(2^32 × abs(sin(i)))
static const uint32_t g_k[64] =
{
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
    0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
    0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
    0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
    0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
    0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
    0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
    0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
    0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};

static uint32_t leftrotate(uint32_t x, uint32_t c)
{
    return ((x << c) | (x >> (32 - c)));
}

// 01 02 03 04
// little-endian:
// 0x04030201

// consume exactly one 64-byte block, mixing it into ctx->state
static void md5_compress(t_md5_ctx *ctx, const uint8_t *block)
{
    uint32_t w[16];
    uint32_t a, b, c, d, f, temp;
    uint32_t g;
    int      i;

    // little-endian: block[0] is the LOW byte of w[0]
    i = 0;
    while (i < 16)
    {
        w[i] = (uint32_t)block[i * 4]
             | ((uint32_t)block[i * 4 + 1] << 8)
             | ((uint32_t)block[i * 4 + 2] << 16)
             | ((uint32_t)block[i * 4 + 3] << 24);
        i++;
    }

    a = ctx->state[0];
    b = ctx->state[1];
    c = ctx->state[2];
    d = ctx->state[3];

    i = 0;
    while (i < 64)
    {
        if (i < 16)
        {
            f = (b & c) | (~b & d);         // F(X,Y,Z) = XY ∨ ¬(X)Z
            g = i;
        }
        else if (i < 32)
        {
            f = (d & b) | (~d & c);         // G(X,Y,Z) = XZ ∨ Y¬Z
            g = (5 * i + 1) % 16;           // 1 6 11 0 5 10 15 4 9 14 3 8 13 2 7 12
        }
        else if (i < 48)
        {
            f = b ^ c ^ d;                  // H(X,Y,Z) = X xor Y xor Z
            g = (3 * i + 5) % 16;           // 5 8 11 14 1 4 7 10 13 0 3 6 9 12 15 2
        }
        else
        {
            f = c ^ (b | ~d);               // I(X,Y,Z) = Y xor (X ∨ not(Z))
            g = (7 * i) % 16;               // 0 7 14 5 12 3 10 1 8 15 6 13 4 11 2 9
        }
        temp = d;
        d = c;
        c = b;
        b = b + leftrotate(a + f + g_k[i] + w[g], g_r[i]);      // B = B + ROTL(A + function(B,C,D) + T[i] + X[k], s)
        a = temp;
        i++;
    }

    ctx->state[0] += a;
    ctx->state[1] += b;
    ctx->state[2] += c;
    ctx->state[3] += d;
}

// buffers + compresses, without touching bitlen (used only during padding)
static void md5_update_raw(t_md5_ctx *ctx, const uint8_t *data, size_t len)
{
    size_t i = 0;

    while (i < len)
    {
        ctx->buffer[ctx->buffer_len++] = data[i++];
        if (ctx->buffer_len == 64)
        {
            DEBUG_PRINT("md5: compressing block #%zu (64-byte chunk)\n", ctx->debug_blocks);
            md5_compress(ctx, ctx->buffer);
            ctx->buffer_len = 0;
#if VERBOSE
            ctx->debug_blocks++;
#endif
        }
    }
}

void    md5_init(void *state){
    t_md5_ctx *ctx = (t_md5_ctx *)state;

    ctx->state[0] = 0x67452301;
    ctx->state[1] = 0xefcdab89;
    ctx->state[2] = 0x98badcfe;
    ctx->state[3] = 0x10325476;
    ctx->bitlen = 0;
    ctx->buffer_len = 0;
#if VERBOSE
    ctx->debug_blocks = 0;
#endif
}

void md5_update(void *state, const uint8_t *data, size_t len)
{
    t_md5_ctx *ctx = (t_md5_ctx *)state;

    md5_update_raw(ctx, data, len);
    ctx->bitlen += (uint64_t)len * 8;
    DEBUG_PRINT("md5: received %zu bytes; bitlen=%lu; buffer_len=%zu\n", len, ctx->bitlen, ctx->buffer_len);
}

void md5_final(void *state, uint8_t *out)
{
    t_md5_ctx *ctx = (t_md5_ctx *)state;
    uint64_t  bitlen = ctx->bitlen;
    uint8_t   pad = 0x80;           // 10000000
    uint8_t   zero = 0x00;
    int       i;

    DEBUG_PRINT("md5: finalizing message with original bitlen=%lu, buffer_len=%zu\n", bitlen, ctx->buffer_len);
    md5_update_raw(ctx, &pad, 1);
    DEBUG_PRINT("md5: finalizing message with padding, buffer_len=%zu\n", ctx->buffer_len);
    while (ctx->buffer_len != 56)
        md5_update_raw(ctx, &zero, 1);
    DEBUG_PRINT("md5: padding complete; buffer_len=%zu before length suffix\n", ctx->buffer_len);
    // litle-endian
    i = 0;
    while (i < 8)
    {
        uint8_t b = (uint8_t)(bitlen >> (8 * i));
        md5_update_raw(ctx, &b, 1);
        i++;
    }
    // digest begin with the low-order byte of A, and end with the high-order byte of D
    i = 0;
    while (i < 4)
    {
        out[i * 4]     = (uint8_t)(ctx->state[i]);
        out[i * 4 + 1] = (uint8_t)(ctx->state[i] >> 8);
        out[i * 4 + 2] = (uint8_t)(ctx->state[i] >> 16);
        out[i * 4 + 3] = (uint8_t)(ctx->state[i] >> 24);
        i++;
    }
    DEBUG_PRINT("md5: final digest prefix=%02x%02x%02x%02x\n", out[0], out[1], out[2], out[3]);
}