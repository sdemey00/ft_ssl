#include "ft_ssl.h"
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

uint8_t *read_stdin(size_t *len)
{
    uint8_t buffer[4096];
    uint8_t *data = NULL;
    size_t total = 0;
    ssize_t r;

    *len = 0;

    while ((r = read(0, buffer, sizeof(buffer))) > 0)
    {
        uint8_t *new_data = malloc(total + r);
        if (!new_data)
            return NULL;

        for (size_t i = 0; i < total; i++)
            new_data[i] = data[i];

        for (ssize_t i = 0; i < r; i++)
            new_data[total + i] = buffer[i];

        free(data);
        data = new_data;
        total += r;
    }

    *len = total;
    return data;
}