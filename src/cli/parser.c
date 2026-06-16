#include "ft_ssl.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static void add_input(t_exec *exec, t_input input)
{
    exec->inputs = realloc(exec->inputs, sizeof(t_input) * (exec->count + 1));
    exec->inputs[exec->count] = input;
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
        }
        else if (strcmp(argv[i], "-q") == 0)
        {
            ctx->quiet = 1;
        }
        else if (strcmp(argv[i], "-r") == 0)
        {
            ctx->reverse = 1;
        }
        else if (strcmp(argv[i], "-s") == 0)
        {
            if (i + 1 >= argc)
            {
                printf("ft_ssl: missing string for -s\n");
                return 1;
            }

            t_input in = {INPUT_STRING, argv[i + 1]};
            add_input(exec, in);
            i++;
        }
        else
        {
            t_input in = {INPUT_FILE, argv[i]};
            add_input(exec, in);
        }
        i++;
    }
    return 0;
}