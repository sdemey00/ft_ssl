#include "ft_ssl.h"

int main(int ac, char **av)
{
    t_context   ctx;
    int         ret;
 
    install_signal_handlers();
    init_context(&ctx);
    if (ac == 1 && isatty(STDIN_FILENO))
        ret = run_interactive(&ctx);
    else
        ret = dispatch_command(&ctx, ac - 1, av + 1);
    if (g_interrupted)
        return (128 + SIGINT);   // standard Unix convention for signal-terminated processes
    return (ret);
}
// commands: md5 sha256
// flags: -p -q -r -s
// -q quiet mode
// -r reverse output
// -p stdin echo rule
// -s string labeling

// t_hash_module = table of each hash module -> init/update/final/digest_size
// one generic handler (e.g. in core/ or cli/) takes a t_hash_module * and does the parse -> read -> hash -> format -> print pipeline once
// md5_handler/sha256_handler collapse into thin wrappers (or disappear entirely, dispatch_command looks up the module by name in the registry and calls the generic handler directly)