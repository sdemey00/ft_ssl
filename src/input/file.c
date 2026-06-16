#include "ft_ssl.h"
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

uint8_t *read_file(char *path, size_t *len)
{
    int fd = open(path, O_RDONLY);
    uint8_t buffer[4096];
    uint8_t *data = NULL;
    size_t total = 0;
    ssize_t r;

    if (fd < 0)
        return NULL;

    while ((r = read(fd, buffer, sizeof(buffer))) > 0)
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

    close(fd);
    *len = total;
    return data;
}