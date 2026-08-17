#ifndef FT_SSL_H
# define FT_SSL_H

# include <stdbool.h>
# include <stddef.h>
# include <stdint.h>
# include <unistd.h>
# include <stdio.h>

#ifndef VERBOSE
# define VERBOSE 0
#endif
#if VERBOSE
# define DEBUG_PRINT(...) dprintf(2, __VA_ARGS__)
#else
# define DEBUG_PRINT(...) ((void)0)
#endif

typedef struct s_context {
  bool quiet;
  bool reverse;
  bool print_stdin;
  bool saw_input;
} t_context;

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
    size_t  state_size;
}   t_hash_module;

// CORE
int     dispatch_command(t_context *ctx, int argc, char **argv);
void    init_context(t_context *ctx);
void    help(void);
void    usage(void);

// REGISTRY
t_hash_module   *get_hash_module(char *name);

// HASH
void            hash_handler(t_context *ctx, t_hash_module *mod, int argc, char **argv);

// PARSER
int             parse_args(t_context *ctx, int argc, char **argv, t_exec *exec);

typedef enum e_read_status
{
    READ_OK,
    READ_ERR_OPEN,
    READ_ERR_ALLOC
}   t_read_status;

// INPUT
/* every reader returns a status and writes the buffer via an out-param. On error, *out is guaranteed NULL adn *len is 0. No uninitialized reads possible.*/
t_read_status   read_input(t_input *in, uint8_t **out, size_t *len);
t_read_status   read_file(char *path, uint8_t **out, size_t *len);
t_read_status   read_stdin(uint8_t **out, size_t *len);
t_read_status   read_string(char *s, uint8_t **out, size_t *len);
t_read_status   read_fd(int fd, uint8_t **out, size_t *len);

# endif