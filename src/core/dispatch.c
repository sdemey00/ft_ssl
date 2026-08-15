#include "ft_ssl.h"
#include <string.h>
#include <unistd.h>

int dispatch_command(t_context *ctx, int argc, char **argv)
{
    t_hash_module *mod;

    if (argc < 1)
    {
        help();
        return (1);
    }
    if (strcmp(argv[0], "help") == 0)
    {
        help();
        return (0);
    }
    mod = get_hash_module(argv[0]);
    if (!mod)
    {
        // dprintf(2, "Invalid command '%s'; type \"help\" for a list.\n", argv[0]);
        return (1);
    }
    hash_handler(ctx, mod, argc - 1, argv + 1);
    return (0);
}