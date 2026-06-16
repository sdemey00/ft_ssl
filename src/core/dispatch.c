#include "ft_ssl.h"
#include <string.h>
# include <stdio.h>

static t_command g_commands[] =
{
    {"md5", md5_handler},
    {"sha256", sha256_handler},
    {NULL, NULL}
};

static t_cmd_handler find_command(char *name)
{
    int i = 0;

    while (g_commands[i].name)
    {
        if (strcmp(g_commands[i].name, name) == 0)
            return (g_commands[i].handler);
        i++;
    }
    return NULL;
}

int dispatch_command(t_context *ctx, int argc, char **argv)
{
    t_cmd_handler handler;

    if (argc < 1)
    {
        print_usage();
        return 1;
    }

    handler = find_command(argv[0]);
    if (!handler)
    {
        dprintf(2, "ft_ssl: Error: '%s' is an invalid command.\n", argv[0]);
        dprintf(2, "\nCommands:\nmd5\nsha256\n");
        dprintf(2, "\nFlags:\n-p -q -r -s\n");
        return (1);
    }

    handler(ctx, argc - 1, argv + 1);
    return (0);
}