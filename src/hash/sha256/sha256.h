#ifndef SHA256_H
# define SHA256_H

# include <stdint.h>
# include <stddef.h>

# define SHA256_DIGEST_SIZE 32

typedef struct s_sha256_ctx
{
    uint32_t    state[8];
    uint64_t    bitlen;
    uint8_t     buffer[64];
    size_t      buffer_len;
#if VERBOSE
    size_t      debug_blocks;
#endif
}   t_sha256_ctx;

void    sha256_init(void *state);
void    sha256_update(void *state, const uint8_t *data, size_t len);
void    sha256_final(void *state, uint8_t *out);

#endif