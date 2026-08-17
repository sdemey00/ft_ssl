#include "ft_ssl.h"

t_read_status read_stdin(uint8_t **out, size_t *len)
{
    return (read_fd(0, out, len));
}