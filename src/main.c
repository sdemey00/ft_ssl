#include "ft_ssl.h"

int main(int ac, char **av) {
  t_context ctx;

  init_context(&ctx);
  return (dispatch_command(&ctx, ac - 1, av + 1));
}

// commands: md5 sha256
// flags: -p -q -r -s
// -q quiet mode
// -r reverse output
// -p stdin echo rule
// -s string labeling

// For each command:
// md5_handler(ctx, args)
//     ↓
// parse flags (-p -q -r -s)
//     ↓
// build input list
//     ↓
// for each input:
//     compute hash
//     format output