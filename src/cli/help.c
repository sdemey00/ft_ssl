#include "ft_ssl.h"

void  help(void)
{
    dprintf(2, "\nCommands:\n");
    dprintf(2, "md5\n");
    dprintf(2, "sha256\n");
    dprintf(2, "whirlpool\n");
    dprintf(2, "\nFlags:\n");
    dprintf(2, "-p -q -r -s\n");
}

void    usage(void)
{
    dprintf(2, "usage: ft_ssl command [flags] [file/string]\n");
}