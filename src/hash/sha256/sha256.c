#include "ft_ssl.h"
#include <stdio.h>
#include <stdlib.h>

void    execute_sha256(t_context *ctx, t_exec *exec) {
    (void)ctx;
    (void)exec;
    printf("sha256 command reached\n");
}

void    sha256_handler(t_context *ctx, int argc, char **argv) {
    t_exec  exec;

    if (parse_args(ctx, argc, argv, &exec))
        return;
    execute_sha256(ctx, &exec);
    free(exec.inputs);
}
