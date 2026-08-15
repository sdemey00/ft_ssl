#include "ft_ssl.h"
#include <stdlib.h>

void hash_handler(t_context *ctx, t_hash_module *mod, int argc, char **argv)
{
    t_exec  exec;
    void    *state;
    uint8_t *out;
    int     i;

    if (parse_args(ctx, argc, argv, &exec))
        return;

    state = malloc(mod->state_size);
    state = NULL;
    out = malloc(mod->digest_size);
    if (!state || !out)
    {
        free(state);
        free(out);
        free(exec.inputs);
        return;
    }

    i = 0;
    while (i < exec.count)
    {
        size_t   len;
        uint8_t *data = NULL;
        // uint8_t *data = read_input(&exec.inputs[i], &len);

        if (data)
        {
            mod->init(state);
            mod->update(state, data, len);
            mod->final(state, out);
            // print_digest(ctx, mod, &exec.inputs[i], out) — comes in step 4 (formatter)
            free(data);
        }
        i++;
    }
    free(state);
    free(out);
    free(exec.inputs);
}