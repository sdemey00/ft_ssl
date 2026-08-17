#include "ft_ssl.h"
#include <stdlib.h>

void hash_handler(t_context *ctx, t_hash_module *mod, int argc, char **argv)
{
    t_exec  exec;
    void    *state;
    uint8_t *out;
    int     i;

    if (parse_args(ctx, argc, argv, &exec))
        return ;
    state = malloc(mod->state_size);
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
        size_t        len;
        uint8_t       *data;
        t_read_status status = read_input(&exec.inputs[i], &data, &len);
        DEBUG_PRINT("hash: input[%d] type=%d status=%d len=%zu\n", i, exec.inputs[i].e_type, status, len);
        if (status != READ_OK)
        {
            dprintf(2, "ft_ssl: %s: %s\n", exec.inputs[i].value ? exec.inputs[i].value : "stdin",
                    status == READ_ERR_OPEN ? "No such file or directory" : "malloc error");
            i++;
            continue;
        }
        mod->init(state);
        mod->update(state, data, len);
        mod->final(state, out);
        print_digest(ctx, mod, &exec.inputs[i], data, len, out);
        DEBUG_PRINT("hash: input[%d] digest computed (%zu bytes)\n", i, mod->digest_size);
        free(data);
        i++;
    }
    free(state);
    free(out);
    free(exec.inputs);
}