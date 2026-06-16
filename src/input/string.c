#include "ft_ssl.h"
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

uint8_t *read_string(char *s, size_t *len)
{
    size_t i = 0;
    uint8_t *data;

    while (s[i])
        i++;

    data = malloc(i);
    if (!data)
        return NULL;

    for (size_t j = 0; j < i; j++)
        data[j] = (uint8_t)s[j];

    *len = i;
    return data;
}