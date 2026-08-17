#ifndef MD5_H
# define MD5_H

# include <stdint.h>
# include <stddef.h>

# define MD5_DIGEST_SIZE 16

typedef struct s_md5_ctx
{
    uint32_t    state[4];
    uint64_t    bitlen;
    uint8_t     buffer[64];
    size_t      buffer_len;
    size_t      debug_blocks;
}   t_md5_ctx;

void    md5_init(void *state);
void    md5_update(void *state, const uint8_t *data, size_t len);
void    md5_final(void *state, uint8_t *out);

#endif