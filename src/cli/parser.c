#include "ft_ssl.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct s_flag
{
    char    *name;
    bool    has_arg;
    bool    (*handler)(t_context *ctx, t_exec *exec, char *arg);
}   t_flag;

static bool add_input(t_exec *exec, t_input input)
{
    t_input *tmp;

    tmp = realloc(exec->inputs, sizeof(t_input) * (exec->count + 1));
    if (!tmp)
        return false;
    exec->inputs = tmp;
    exec->inputs[exec->count] = input;
    exec->count++;
    return true;
}

static bool flag_p(t_context *ctx, t_exec *exec, char *arg)
{
    (void)arg;
    ctx->print_stdin = true;
    return add_input(exec, (t_input){INPUT_STDIN, NULL});
}

static bool flag_q(t_context *ctx, t_exec *exec, char *arg)
{
    (void)exec;
    (void)arg;
    ctx->quiet = true;
    return true;
}

static bool flag_r(t_context *ctx, t_exec *exec, char *arg)
{
    (void)exec;
    (void)arg;
    ctx->reverse = true;
    return true;
}

static bool flag_s(t_context *ctx, t_exec *exec, char *arg)
{
    (void)ctx;
    return add_input(exec, (t_input){INPUT_STRING, arg});
}

static t_flag g_flags[] =
{
    {"-p", false, flag_p},
    {"-q", false, flag_q},
    {"-r", false, flag_r},
    {"-s", true,  flag_s},
    {NULL, false, NULL}
};

static t_flag *find_flag(char *name)
{
    int i = 0;

    while (g_flags[i].name)
    {
        if (strcmp(g_flags[i].name, name) == 0)
            return (&g_flags[i]);
        i++;
    }
    return (NULL);
}

int parse_args(t_context *ctx, int argc, char **argv, t_exec *exec)
{
    int     i = 0;
    t_flag  *flag;
    bool    stop_flags = false;
    char    *arg;

    exec->inputs = NULL;
    exec->count = 0;
    while (i < argc)
    {
        flag = stop_flags ? NULL : find_flag(argv[i]);
        if (flag)
        {
            arg = NULL;
            if (flag->has_arg)
            {
                if (i + 1 >= argc)
                {
                    dprintf(2, "ft_ssl: missing string for %s\n", flag->name);
                    return (1);
                }
                arg = argv[++i];
            }
            if (!flag->handler(ctx, exec, arg))
                return (1);
        }
        else
        {
            stop_flags = true;
            if (!add_input(exec, (t_input){INPUT_FILE, argv[i]}))
                return (1);
        }
        i++;
    }
    if (exec->count == 0)
        add_input(exec, (t_input){INPUT_STDIN, NULL});
    return (0);
}