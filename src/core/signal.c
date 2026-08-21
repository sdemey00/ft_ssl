#include "ft_ssl.h"
#include <signal.h>

volatile sig_atomic_t  g_interrupted = 0;

static void handle_sigint(int sig)
{
    (void)sig;
    g_interrupted = 1;
}

void install_signal_handlers(void)
{
    struct sigaction sa;

    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
}