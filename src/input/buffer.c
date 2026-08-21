#include "ft_ssl.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
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
    while (1)
    {
        r = read(fd, buffer, READ_CHUNK);
        if (r < 0)
        {
            free(data);
            *out = NULL;
            *len = 0;
            if (errno == EINTR)
                return (READ_INTERRUPTED);
            return (READ_ERR_OPEN);  // read() itself failed mid-stream
        }
        if (r == 0)
            break;
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
    *out = data;   // NULL if total == 0, which is fine, see read_input contract
    *len = total;
    return (READ_OK);
}