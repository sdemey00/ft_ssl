#ifndef WHIRLPOOL_H
# define WHIRLPOOL_H

# include <stdint.h>
# include <stddef.h>

# define WHIRLPOOL_DIGEST_SIZE 64
# define WHIRLPOOL_BLOCK_SIZE 64

typedef struct s_whirlpool_ctx
{
    uint64_t    state[8];           // Miyaguchi-Preneel accumulator
    uint64_t    bitlen;
    uint8_t     buffer[64];
    size_t      buffer_len;
#if VERBOSE
    size_t      debug_blocks;
#endif
} t_whirlpool_ctx;

void    whirlpool_init(void *state);
void    whirlpool_update(void *state, const uint8_t *data, size_t len);
void    whirlpool_final(void *state, uint8_t *out);

#endif
