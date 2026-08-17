#include "ft_ssl.h"

t_read_status read_input(t_input *in, uint8_t **out, size_t *len)
{
    if (in->e_type == INPUT_STDIN)
        return (read_stdin(out, len));
    if (in->e_type == INPUT_FILE)
        return (read_file(in->value, out, len));
    if (in->e_type == INPUT_STRING)
        return (read_string(in->value, out, len));
    *out = NULL;
    *len = 0;
    return (READ_ERR_OPEN);
}