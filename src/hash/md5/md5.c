#include "ft_ssl.h"
#include <stdio.h>
#include <stdlib.h>

void    md5_init(void *state){
    (void)state;
}

void    md5_update(void *state, const uint8_t *data, size_t len){
    (void)state; (void)data; (void)len;
}

void    md5_final(void *state, uint8_t *out){
    (void)state; (void)out;
}