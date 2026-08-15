#include <stdio.h>

void  help(void) {
    dprintf(2, "usage: ft_ssl command [flags] [file/string]\n");
    dprintf(2, "\nCommands:\n");
    dprintf(2, "md5\n");
    dprintf(2, "sha256\n");
    dprintf(2, "\nFlags:\n");
    dprintf(2, "-p -q -r -s\n");
}