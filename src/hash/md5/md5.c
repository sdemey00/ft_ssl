#include "ft_ssl.h"
#include <stdio.h>
#include <stdlib.h>

void    execute_md5(t_context *ctx, t_exec *exec) {
    (void)ctx;
    (void)exec;
    printf("md5 command reached\n");
}

void    md5_handler(t_context *ctx, int argc, char **argv) {
    t_exec  exec;

    if (parse_args(ctx, argc, argv, &exec))
        return;
    execute_md5(ctx, &exec);
    free(exec.inputs);
}
