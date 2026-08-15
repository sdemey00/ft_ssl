#include "ft_ssl.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// TODO
// avoid if else forest and use flags dictionnary

static void add_input(t_exec *exec, t_input input)
{
    exec->inputs = realloc(exec->inputs, sizeof(t_input) * (exec->count + 1));
    exec->inputs[exec->count] = input;  // NULL deref if realloc failed
    exec->count++;
}

int parse_args(t_context *ctx, int argc, char **argv, t_exec *exec)
{
    int i = 0;

    exec->inputs = NULL;
    exec->count = 0;

    while (i < argc)
    {
        if (strcmp(argv[i], "-p") == 0)
        {
            ctx->print_stdin = 1;

            t_input in = {INPUT_STDIN, NULL};
            add_input(exec, in);
            DEBUG_PRINT("parse: -p -> stdin\n");
        }
        else if (strcmp(argv[i], "-q") == 0)
        {
            ctx->quiet = 1;
            DEBUG_PRINT("parse: -q -> quiet\n");
        }
        else if (strcmp(argv[i], "-r") == 0)
        {
            ctx->reverse = 1;
            DEBUG_PRINT("parse: -r -> reverse\n");
        }
        else if (strcmp(argv[i], "-s") == 0)
        {
            if (i + 1 >= argc)
            {
                dprintf(2, "ft_ssl: missing string for -s\n");
                return 1;
            }

            t_input in = {INPUT_STRING, argv[i + 1]};
            add_input(exec, in);
            DEBUG_PRINT("parse: -s -> string \"%s\"\n", argv[i + 1]);
            i++;
        }
        else
        {
            t_input in = {INPUT_FILE, argv[i]};
            add_input(exec, in);
            DEBUG_PRINT("parse: file -> \"%s\"\n", argv[i]);
        }
        i++;
    }
    return 0;
}

// Invalid command '-p'; type "help" for a list.