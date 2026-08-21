#ifndef FT_SSL_H
# define FT_SSL_H

# include <stdbool.h>
# include <stddef.h>
# include <stdint.h>
# include <unistd.h>
# include <stdio.h>
# include <signal.h>

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


/*  
Merkel-Damgard construction (shared paradigm)
    1. Fixed initial state ("magic" constant (Initialization Vector)), specific to the algorithm.
    2. Pad the message to a multiple of a fixed block size, in a way that encodes the original message length (Merkle-Damgard strengthening).
    3. Split the padded message into fixed-size blocks.
    4. Compression function: for each block, mix it into the current state to produce a new state. This is the actual "hashing", everything else is blockkeeping.
    5. Final state, serialized = the digest.
Every algorithm in this family is: what are the constants, what's the block size, and what does the compression function do internally.
This maps directly onto t_hash_module interface (init/update/final)
*/
typedef struct s_hash_module
{
    char    *name;      // command name, e.g. "md5"
    char    *label;     // display label, e.g. "MD5"

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
int     run_interactive(t_context *ctx);

// SIGNALS
extern volatile sig_atomic_t   g_interrupted;
void    install_signal_handlers(void);

// REGISTRY
t_hash_module   *get_hash_module(char *name);

// HASH
int             hash_handler(t_context *ctx, t_hash_module *mod, int argc, char **argv);

// PARSER
int             parse_args(t_context *ctx, int argc, char **argv, t_exec *exec);

// FORMATTER
void print_digest(t_context *ctx, t_hash_module *mod, t_input *in,
                   const uint8_t *data, size_t len, const uint8_t *digest);
                   
// INPUT
typedef enum e_read_status
{
    READ_OK,
    READ_ERR_OPEN,
    READ_ERR_ALLOC,
    READ_INTERRUPTED
}   t_read_status;

/* every reader returns a status and writes the buffer via an out-param. On error, *out is guaranteed NULL adn *len is 0. No uninitialized reads possible.*/
t_read_status   read_input(t_input *in, uint8_t **out, size_t *len);
t_read_status   read_file(char *path, uint8_t **out, size_t *len);
t_read_status   read_stdin(uint8_t **out, size_t *len);
t_read_status   read_string(char *s, uint8_t **out, size_t *len);
t_read_status   read_fd(int fd, uint8_t **out, size_t *len);


#endif