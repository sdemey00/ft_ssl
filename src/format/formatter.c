#include "ft_ssl.h"
#include <stdio.h>

static void hex_encode(const uint8_t *digest, size_t len, char *out)
{
    static const char hex[] = "0123456789abcdef";
    size_t i;

    i = 0;
    while (i < len)
    {
        out[i * 2] = hex[digest[i] >> 4];
        out[i * 2 + 1] = hex[digest[i] & 0xF];
        i++;
    }
    out[len * 2] = '\0';
}

static void print_stdin_default(t_context *ctx, const char *hex)
{
    if (ctx->quiet)
        printf("%s\n", hex);
    else if (ctx->reverse)
        printf("%s stdin\n", hex);
    else
        printf("(stdin)= %s\n", hex);
}

static void print_stdin_echo(t_context *ctx, const uint8_t *data,
                              size_t len, const char *hex)
{
    const char  *content;
    size_t      disp_len;

    content = data ? (const char *)data : "";
    if (ctx->quiet)
    {
        if (len > 0)
            write(1, data, len);
        printf("%s\n", hex);
        return;
    }
    disp_len = len;
    if (disp_len > 0 && content[disp_len - 1] == '\n')
        disp_len--;
    printf("(\"%.*s\")= %s\n", (int)disp_len, content, hex);
}

static void print_named(t_context *ctx, t_hash_module *mod,
                         t_input *in, const char *hex)
{
    bool is_string = (in->e_type == INPUT_STRING);

    if (ctx->quiet)
        printf("%s\n", hex);
    else if (ctx->reverse)
    {
        if (is_string)
            printf("%s \"%s\"\n", hex, in->value);
        else
            printf("%s %s\n", hex, in->value);
    }
    else
    {
        if (is_string)
            printf("%s (\"%s\") = %s\n", mod->label, in->value, hex);
        else
            printf("%s (%s) = %s\n", mod->label, in->value, hex);
    }
}

void print_digest(t_context *ctx, t_hash_module *mod, t_input *in,
                   const uint8_t *data, size_t len, const uint8_t *digest)
{
    char hex[129];  // room up to 64-byte digests (whirlpool, bonus)

    hex_encode(digest, mod->digest_size, hex);
    if (in->e_type == INPUT_STDIN)
    {
        if (ctx->print_stdin)
            print_stdin_echo(ctx, data, len, hex);
        else
            print_stdin_default(ctx, hex);
    }
    else
        print_named(ctx, mod, in, hex);
}