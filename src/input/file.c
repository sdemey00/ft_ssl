#include "ft_ssl.h"
#include <fcntl.h>
#include <unistd.h>

t_read_status read_file(char *path, uint8_t **out, size_t *len)
{
    int             fd;
    t_read_status   status;

    fd = open(path, O_RDONLY);
    if (fd < 0)
    {
        *out = NULL;
        *len = 0;
        return (READ_ERR_OPEN);
    }
    status = read_fd(fd, out, len);
    close(fd);
    return (status);
}