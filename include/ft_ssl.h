#ifndef FT_SSL_H
# define FT_SSL_H

# include <stdbool.h>
# include <stddef.h>
# include <stdint.h>

typedef struct s_context {
  bool quiet;
  bool reverse;
  bool print_stdin;
  bool saw_input;
} t_context;

// command handler signature
typedef void (*t_cmd_handler)(t_context *ctx, int ac, char **av);

typedef struct s_command {
  char          *name;
  t_cmd_handler handler;
} t_command;

typedef struct s_input
{
    enum
    {
        INPUT_STDIN,
        INPUT_FILE,
        INPUT_STRING
    }           e_type;
    char        *value;   // filename or string (NULL for stdin)
}   t_input;

typedef struct s_exec
{
    t_input *inputs;
    int     count;
}   t_exec;


typedef struct s_hash_module
{
    char    *name;

    void    (*init)(void *state);
    void    (*update)(void *state, const uint8_t *data, size_t len);
    void    (*final)(void *state, uint8_t *out);

    size_t  digest_size;
}   t_hash_module;

// CORE
int     dispatch_command(t_context *ctx, int argc, char **argv);
void    init_context(t_context *ctx);
void    print_usage(void);

// CMD
void    md5_handler(t_context *ctx, int argc, char **argv);
void    sha256_handler(t_context *ctx, int argc, char **argv);

// PARSER
int     parse_args(t_context *ctx, int argc, char **argv, t_exec *exec);

# endif