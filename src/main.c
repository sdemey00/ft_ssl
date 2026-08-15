#include "ft_ssl.h"

int main(int ac, char **av) {
  t_context ctx;

  init_context(&ctx);
  dispatch_command(&ctx, ac - 1, av + 1);
  return (0);
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