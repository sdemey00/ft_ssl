#include "ft_ssl.h"
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

# define READ_CHUNK 4096

/* Chunk logic */
t_read_status read_fd(int fd, uint8_t **out, size_t *len)
{
    uint8_t buffer[READ_CHUNK];
    uint8_t *data;
    size_t  total;
    ssize_t r;

    data = NULL;
    total = 0;
    while ((r = read(fd, buffer, READ_CHUNK)) > 0)
    {
        uint8_t *tmp = realloc(data, total + r);
        if (!tmp)
        {
            free(data);
            *out = NULL;
            *len = 0;
            return (READ_ERR_ALLOC);
        }
        data = tmp;
        memcpy(data + total, buffer, r);
        total += r;
    }
    if (r < 0)
    {
        free(data);
        *out = NULL;
        *len = 0;
        return (READ_ERR_OPEN);  // read() itself failed mid-stream
    }
    *out = data;   // NULL if total == 0, which is fine, see read_input contract
    *len = total;
    return (READ_OK);
}