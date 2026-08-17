#include "ft_ssl.h"
#include <string.h>
#include <unistd.h>

int dispatch_command(t_context *ctx, int argc, char **argv)
{
    t_hash_module *mod;

    if (argc < 1)
    {
        usage();
        return (1);
    }
    if (strcmp(argv[0], "help") == 0)
    {
        usage();
        help();
        return (0);
    }
    mod = get_hash_module(argv[0]);
    if (!mod)
    {
        DEBUG_PRINT("dispatch: unknown command \"%s\"\n", argv[0]);
        dprintf(2, "ft_ssl: Error: '%s' is an invalid command.\n", argv[0]);
        help();
        return (1);
    }
    DEBUG_PRINT("dispatch: resolved \"%s\" -> module\n", argv[0]);
    hash_handler(ctx, mod, argc - 1, argv + 1);
    return (0);
}