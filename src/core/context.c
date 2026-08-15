#include "ft_ssl.h"

void  init_context(t_context *ctx) {
  ctx->quiet = false;
  ctx->reverse = false;
  ctx->print_stdin = false;
  ctx->saw_input = false;
}