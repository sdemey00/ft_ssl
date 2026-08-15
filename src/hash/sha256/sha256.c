#include "ft_ssl.h"
#include <stdio.h>
#include <stdlib.h>

void    sha256_init(void *state){
    (void)state;
}

void    sha256_update(void *state, const uint8_t *data, size_t len){
    (void)state; (void)data; (void)len;
}

void    sha256_final(void *state, uint8_t *out){
    (void)state; (void)out;
}
