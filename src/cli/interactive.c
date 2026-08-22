#include "ft_ssl.h"
#include <string.h>
#include <errno.h>

# define PROMPT     "ft_ssl> "
# define LINE_MAX   4096
# define ARGV_MAX   64

static int read_line(char *buf, size_t max)
{
    size_t  i;
    char    c;
    ssize_t r;

    i = 0;
    while (1)
    {
        r = read(STDIN_FILENO, &c, 1);
        if (r < 0 && errno == EINTR)
            return (-2);
        if (r <= 0)
        {
            if (i == 0)
                return (-1);
            break;
        }
        if (c == '\n')
            break;
        if (i < max - 1)
            buf[i++] = c;
    }
    buf[i] = '\0';
    return (0);
}

static int tokenize(char *line, char **argv, int max_tokens)
{
    int     argc;
    char    *src;
    char    *dst;
    char    *tok_end;
    char    quote;

    argc = 0;
    src = line;
    dst = line;
    while (*src && argc < max_tokens - 1)
    {
        while (*src == ' ' || *src == '\t')
            src++;
        if (*src == '\0')
            break;
        argv[argc++] = dst;
        quote = 0;
        while (*src)
        {
            if (quote)
            {
                if (*src == quote)
                {
                    quote = 0;
                    src++;
                    continue;
                }
                *dst++ = *src++;
                continue;
            }
            if (*src == '\'' || *src == '"')
            {
                quote = *src++;
                continue;
            }
            if (*src == ' ' || *src == '\t')
                break;
            *dst++ = *src++;
        }
        tok_end = dst;
        while (*src == ' ' || *src == '\t')
            src++;
        *tok_end = '\0';
        dst = tok_end + 1;
    }
    argv[argc] = NULL;
    return (argc);
}

int run_interactive(t_context *ctx)
{
    char        line[LINE_MAX];
    char        *argv[ARGV_MAX];
    int         argc;
    int         status;
    const char  *banner;

    banner = "ft_ssl interactive mode. Type a command (e.g. \"md5\"), \"help\", or \"quit\".\n";
    write(1, banner, strlen(banner));
    while (1)
    {
        write(1, PROMPT, sizeof(PROMPT) - 1);
        status = read_line(line, sizeof(line));
        if (status == -2)
        {
            write(1, "\n", 1);
            continue ;
            // write(1, "\nInterrupted.\n", 14);
            // break;
        }
        if (status < 0)
        {
            write(1, "\n", 1);
            break;
        }
        if (line[0] == '\0')
            continue;
        if (strcmp(line, "quit") == 0 || strcmp(line, "exit") == 0)
            break;
        argc = tokenize(line, argv, ARGV_MAX);
        if (argc == 0)
            continue;
        init_context(ctx);   // each command is independent, like a fresh invocation
        dispatch_command(ctx, argc, argv);
    }
    return (0);
}