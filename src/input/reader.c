#include "ft_ssl.h"
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

uint8_t *read_stdin(size_t *len);
uint8_t *read_string(char *s, size_t *len);
uint8_t *read_file(char *path, size_t *len);

uint8_t *read_input(t_input *in, size_t *len)
{
    if (in->e_type == INPUT_STDIN)
        return (read_stdin(len));
    if (in->e_type == INPUT_FILE)
        return (read_file(in->value, len));
    if (in->e_type == INPUT_STRING)
        return (read_string(in->value, len));
    return NULL;
}