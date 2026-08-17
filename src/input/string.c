#include "ft_ssl.h"
#include <stdlib.h>
#include <string.h>

t_read_status read_string(char *s, uint8_t **out, size_t *len)
{
    size_t n;

    if (!s)
    {
        *out = NULL;
        *len = 0;
        return (READ_OK);  // shouldn't happen given parser, but not an error either
    }
    n = strlen(s);
    if (n == 0)
    {
        *out = NULL;
        *len = 0;
        return (READ_OK);
    }
    *out = malloc(n);
    if (!*out)
    {
        *len = 0;
        return (READ_ERR_ALLOC);
    }
    memcpy(*out, s, n);
    *len = n;
    return (READ_OK);
}