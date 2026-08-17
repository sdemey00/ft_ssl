#include "ft_ssl.h"
#include <stdio.h>
#include <stdlib.h>

void    md5_init(void *state){
    (void)state;
    printf("md5: init\n");
}

void    md5_update(void *state, const uint8_t *data, size_t len){
    (void)state; (void)data; (void)len;
    printf("md5: update\n");
}

void    md5_final(void *state, uint8_t *out){
    (void)state; (void)out;
    printf("md5: final\n");
}