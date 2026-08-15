#include "ft_ssl.h"
#include "md5/md5.h"
#include "sha256/sha256.h"
#include <string.h>

static t_hash_module g_modules[] =
{
    {"md5",    md5_init,    md5_update,    md5_final,    MD5_DIGEST_SIZE,    sizeof(t_md5_ctx)},
    {"sha256", sha256_init, sha256_update, sha256_final, SHA256_DIGEST_SIZE, sizeof(t_sha256_ctx)},
    {NULL, NULL, NULL, NULL, 0, 0}
};

t_hash_module *get_hash_module(char *name)
{
    int i = 0;

    while (g_modules[i].name)
    {
        if (strcmp(g_modules[i].name, name) == 0)
            return (&g_modules[i]);
        i++;
    }
    return (NULL);
}