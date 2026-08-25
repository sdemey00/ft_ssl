# ft_ssl
A from-scratch reimplementation of core `OpenSSL` hashing functionality in C: `md5`, `sha256` and `whirlpool`. Built with an architecture design to make adding new algorithms trivial rather than a rewrite.
```
$ echo "42 is nice" | ./ft_ssl md5
(stdin)= 35f1d6de0302e2086a4e472266efb3a9

$ ./ft_ssl sha256 -s "42 is nice"
SHA256 ("42 is nice") = b7e44c7a40c5f80139f0a50f3650fb2bd8d00b0d24667c4c2ca32c88e13b758f
```
---
## Table of Contents

- [What this project is](#what-this-project-is)
- [What is a cryptographic hash function?](#what-is-a-cryptographic-hash-function)
- [Quick start](#quick-start)
- [Usage](#usage)
- [Architecture](#architecture)
- [Design decisions](#design-decisions)
- [How the hashing actually works](#how-the-hashing-actually-works)
- [Testing](#testing)
- [Adding a new algorithm](#adding-a-new-algorithm)
- [Development process & AI usage](#development-process--ai-usage)
- [Building](#building)

## What this project is

`ft_ssl` recreates a slice of the real `OpenSSL` command-line tool: given some input (a file, a string, or piped `stdin`), it computes a cryptographic fingerprint of that data using either **MD5** or **SHA256** hashing algorithm, and prints it in a format compatible with looks like `md5sum` / `sha256sum`.

Every byte of the actual hashing math: padding, bitwise mixing, state compression, is implemented from first principles, using only `open`, `close`, `read`, `write`, `malloc`, and `free`. No external cryptography library is used anywhere in this project; that's the entire point of the exercise.

Beyond "does it produce the right hash," the second goal of this project was **structural**: the codebase is built so that a third, fourth, or fifth hash algorithm can be added by writing one new file and one line in a lookup table (e.g. whirlpool) never by copy-pasting or restructuring existing code. That constraint shaped nearly every design decision described below.

---


## What is a cryptographic hash function?

First, we need to understand how hashing algorithm works. If you've never worked on anything like this before, here's the mental model in three sentences:

A hash function takes an input of **any size**, a single character or a 10 GB video file, it doesn't matter, and produces an output of a **fixed size** (128 bits for MD%, 256 bits for SHA-256). This output is called a **digest**. A *cryptographic* hash function has a few extra guarantees on top of an ordinary hash function: the output looks statistically random, it's computationnaly infeasible to find two differnet inputs that produce the same output (a "collision"), and it's infeasable to reconstruct the input just from the output.

This is why file-download pages often show a hash next to the download link: after downloading, you compute the hash yourself and compare it to the published one. If even a single bit of the file changed: corruption, tampering, a bad mirror and the digest will look completely different, with no visible pattern connecting it to the original. That property is called the **avalanche effect**, and it's a deliberate design goal of the algorithms like MD5 and SHA-256, not a side effect.

> **Note on MD5's security status:** MD5 is implemented here for its educational and historical value, it's simple enough to fully understand in an afternoon, and it's still the algorithm most people picture first. In real-world security-sensitive contexts it is considered **broken**: practical collision attacks exist, and it should never be used for password storage, digital signatures, or certificate validation. SHA-256 is currently considered secure for those purposes. `ft_ssl` implements both, but this distinction is worth understanding, not just implementing.

---

## Quick start

```bash
make
./ft_ssl md5 -s "hello world"
```

Expected output:
```
MD5 ("hello world") = 5eb63bbbe01eeed093cb22bb8f5acdc3
```

---

## Usage

```
ft_ssl command [flags] [file/string]
```

**Commands:** `md5`, `sha256`, `whirlpool`, `help`

**Flags:**

| Flag | Meaning |
|------|---------|
| `-p` | Echo `stdin` to `stdout`, then append its checksum |
| `-q` | Quiet mode, print only the digest, no labels |
| `-r` | Reverse output format (`hex filename` instead of `LABEL (filename) = hex`) |
| `-s <string>` | Hash the given string directly, instead of a file |

Flags may be freely combined. Once the first non-flag argument (a filename) appears, **flag parsing stops**. Anything after that point, even something that looks like `-s`, is treated as a literal filename. This mirrors the behavior of standard Unix argument parsing (`getopt`-style) and is demonstrated below:

```bash
$ ./ft_ssl md5 -r -p -s "foo" file -s "bar"
("one more thing")= a0bd1876c6f011dd50fae52827f445f5
acbd18db4cc2f85cedef654fccc4a4d8 "foo"
53d53ea94217b259c11a5a2d104ec58a file
ft_ssl: md5: -s: No such file or directory
ft_ssl: md5: bar: No such file or directory
```

Here, `-s "foo"` before `file` is parsed as a flag, but the second `-s` (appearing *after* `file`) is treated as a literal (and nonexistent) filename, exactly like `bar`.

### Examples

```bash
echo "42 is nice" | ./ft_ssl md5              # (stdin)= 35f1d6de...
echo "42 is nice" | ./ft_ssl md5 -p           # ("42 is nice")= 35f1d6de...
./ft_ssl md5 -s "42 is nice"                  # MD5 ("42 is nice") = 35f1d6de...
./ft_ssl md5 myfile.txt                       # MD5 (myfile.txt) = ...
./ft_ssl md5 -r myfile.txt                    # <hex> myfile.txt
./ft_ssl md5 -q myfile.txt                    # <hex> only
./ft_ssl sha256 myfile.txt                    # SHA256 (myfile.txt) = ...
```

---

## Architecture

Each directory has exactly one responsibility, and, critically, **no directory needs to know the internals of another**. `hash_handler.c` doesn't know how MD5's compression function works; it only knows that a `t_hash_module` has an `init`/`update`/`final`. `formatter.c` doesn't know or care which algorithm produced the digest it's printing; it only reads `mod->label` and `mod->digest_size`. This separation is what makes each piece independently testable and independently replaceable.

---

## Design 

### 1. No `if`/`else` forests (table-driven dispatch everywhere)

I wanted to avoid "a forest of if/else" for command dispatch, and use a function pointer array instead. This project extends that principle to **every** name-to-behavior lookup in the codebase, not just the one place it was required:

- **Commands -> hash algorithm:** `hash/registry.c` holds a static array of `t_hash_module { name, label, init, update, final, digest_size, state_size }`. Adding an algorithm is one array entry, `dispatch.c` never changes.
- **Flags -> behavior:** `cli/parser.c` holds a static array of `t_flag { name, has_arg, handler }`. Adding a flag is one array entry, the parsing loop never changes.

A table means the *lookup logic* and the *list of things being looked up* are separate concerns, which is really the entire idea behind "modularity" in a practical, not just theoretical, sense.

### 2. A generic hashing pipeline, not per-algorithm handlers

Early in development, `md5` and `sha256` each had their own handler function that duplicated the same four steps: parse arguments, read the input, run the hash, print the result. The only genuine difference between them was *which three function pointers to call*. That duplication was collapsed into a single `hash_handler()` that takes a `t_hash_module *` and runs the pipeline once. The per-algorithm code is now **only** the algorithm's math (`md5.c`/`sha256.c`), nothing else.

This mirrors how OpenSSL itself is structured internally (its `EVP_MD` interface is, at its core, the same idea: a name-indexed table of `init`/`update`/`final` function pointers plus digest-size metadata), arriving independently at the same shape as a widely-used production cryptography library is a reasonably strong signal the abstraction is the *right* one, not just *a* one.

### 3. Every function signature matches its `void *` contract exactly

`t_hash_module`'s function pointers are typed as `void (*)(void *state, ...)`. Rather than writing `md5_init(t_md5_ctx *state)` and casting the function pointer to fit the table (which is undefined behavior in C), every hash module's `init`/`update`/`final` is written to take `void *state` directly, and casts the pointer to its real type as the *first line inside the function body*. This keeps the registry itself free of any casting: it just stores and calls pointers, with zero type gymnastics.

### 5. Streaming-shaped interface, even though input isn't actually streamed

`update()` can be called multiple times with arbitrary chunk sizes, and correctly buffers partial blocks between calls. In this project's current form, `read_input()` reads an entire file into memory before hashing it, so `update()` is only ever called once per input in practice, but the interface doesn't assume that. This matters for two reasons: it matches how MD5/SHA-256 are *specified* to work (as streaming algorithms with internal state), and it means the exact same `md5.c`/`sha256.c` code would work correctly if `hash_handler.c` were later changed to read and hash a large file in fixed-size chunks instead of loading it all into memory at once.

---

## How the hashing actually works

Both MD5 and SHA-256 belong to a family of algorithms built on the same underlying construction, called **Merkle–Damgård**. Understanding this shared shape is what makes the two implementations look so similar despite computing very different output. It's what makes adding a third algorithm (see [Adding a new algorithm](#adding-a-new-algorithm)) mostly a matter of filling in a template rather than inventing new plumbing.

### The shared skeleton

1. **Fixed initial state.** Each algorithm starts from a small set of constant "magic numbers" (its Initialization Vector). These aren't arbitrary, MD5's are hardcoded hex constants, SHA-256's are derived from the fractional parts of the square roots of the first 8 prime numbers. Different constants, same purpose: a fixed, known starting point.
2. **Padding.** The message is extended so its length becomes a multiple of the algorithm's block size (64 bytes for both MD5 and SHA-256). Padding always appends a single `1` bit, then enough `0` bits to reach the right length, then the **original message length** (before padding) as a fixed-size integer. Encoding the length this way (called *Merkle–Damgård strengthening*) is a deliberate defense against certain classes of forgery.
3. **Block splitting.** The padded message is cut into fixed-size blocks (64 bytes each, here).
4. **Compression.** For each block, in order, a **compression function** mixes the block's bytes into the current running state, producing a new state. This is where all the real "hashing" happens. Everything else is bookkeeping around getting well-formed blocks to this function.
5. **Serialization.** After the last block, the final state *is* the digest. It just needs to be written out as bytes in the algorithm's correct byte order.

### Where `init` / `update` / `final` map onto this

| Step | Function | What happens |
|---|---|---|
| 1 | `init` | Set state to the fixed IV constants; zero the internal buffer |
| 2 – 4 (partial) | `update` | Buffer incoming bytes; whenever a full 64-byte block accumulates, run the compression function on it and clear the buffer |
| 2 (finish) – 5 | `final` | Apply padding to whatever's left in the buffer (which may trigger one or two more compression calls), then serialize the state into the output digest |

### What's actually different between MD5 and SHA-256

| | MD5 | SHA-256 |
|---|---|---|
| Digest size | 128 bits (16 bytes) | 256 bits (32 bytes) |
| Internal state | 4 × 32-bit words | 8 × 32-bit words |
| Block size | 64 bytes | 64 bytes |
| Rounds per block | 64 | 64 |
| Byte order (endianness) | Little-endian | Big-endian |
| Round function | Four different bitwise functions (`F`/`G`/`H`/`I`), cycled through in groups of 16 rounds | One consistent round structure (`Ch`, `Maj`, and two "sigma" mixing functions) used identically every round |
| Message schedule | Uses the block's 16 words directly, in a permuted order per round | Expands 16 words into 64 via a dedicated mixing step (`σ0`/`σ1`) before the round loop even starts |

---

## Bonus: interactive command mode

Real (old) `openssl`, when launched with no arguments from an actual terminal, drops into an interactive prompt (`OpenSSL>`) and reads commands one line at a time until you type `quit`/`exit` or hit `Ctrl+D`. `ft_ssl` reproduces this behavior:

```
$ ./ft_ssl
ft_ssl interactive mode. Type a command (e.g. "md5"), "help", or "quit".
ft_ssl> md5 -s hello
MD5 ("hello") = 5d41402abc4b2a76b9719d911017c592
ft_ssl> sha256 -s hello
SHA256 ("hello") = 2cf24dba5fb0a30e26e83b2ac5b9e29e1b161e5c1fa7425e73043362938b9824
ft_ssl> quit
```

**The interactive prompt only launches when stdin is an actual terminal** (`isatty(STDIN_FILENO)`), checked alongside "no arguments were given." Automated grading always runs a program non-interactively: piped input, redirected input, or a closed stdin. So `isatty()` will be false in that context, and the usage-message displays. A human running `./ft_ssl` directly from their own shell gets the bonus prompt instead, which mirrors exactly when real `openssl` does the same thing.


### Implementation notes

- **Line reading is manual (`read()`, one byte at a time), not `readline`.** The subject permits `readline` specifically for this bonus, and it would provide arrow-key history/editing for free. But it also adds an external library dependency that anyone building this project would need installed. A hand-rolled line reader keeps the project dependency-free and consistent with the rest of the codebase's I/O style; swapping in `readline` later would only require changing the line-reading function itself, not the REPL loop or command dispatch around it.
- **Each command line resets `t_context` before dispatch.** Without this, a flag like `-q` on one command would silently persist into the next command typed at the prompt, since `init_context()` was previously only ever called once at program start. Every interactive command now behaves as an independent, fresh invocation: exactly like running `./ft_ssl <command>` fresh from the shell each time.
- **Zero changes to `dispatch_command`, `parser.c`, `hash_handler.c`, or `formatter.c`.** The REPL loop's only job is to turn one typed line into a `char **argv`/`int argc` pair and hand it to the exact same `dispatch_command()` that `main()` calls for real process arguments. This is the modularity payoff described earlier in this document made concrete: because the hashing pipeline never assumed where its `argv` came from, adding an entirely new *input method* for commands required writing exactly one new file.
- **Overflow-safe line reading.** Lines longer than the internal buffer are safely truncated and the remainder is drained (not written out of bounds) until the next newline, so long input can't desynchronize line boundaries or corrupt memory.

---

## Bonus: Whirlpool

`whirlpool` adds a fourth digest algorithm, 512-bit output, considerably stronger than MD5,  slotting into the exact same registry/module pattern as `md5` and `sha256`:

```bash
./ft_ssl whirlpool -s "42 is nice"
```

### Why Whirlpool is structurally different from MD5/SHA-256

MD5 and SHA-256 mix each block directly into the running state with a custom bitwise round function. Whirlpool takes a different approach: its compression step is built from a dedicated 512-bit **block cipher**, used in **Miyaguchi–Preneel** mode: the current hash state is used as the *key* to encrypt the incoming block, and the result is XORed back with both the block and the old state. That internal cipher is structurally a cousin of AES: an 8×8 byte state run through 10 rounds of SubBytes -> ShiftColumns -> MixRows -> AddRoundKey, with the key schedule reusing that exact same round function (just swapping AddRoundKey for a round-constant addition). This is why `whirlpool.c` doesn't look like `md5.c`/`sha256.c` internally, even though `init`/`update`/`final` and the buffering/padding scaffolding around them are the same shape as every other module in this project.

### On sourcing the algorithm

An early draft of this feature was going to adapt an existing GPL-licensed library's Whirlpool file. That was deliberately not done, mixing GPL-licensed code into this repository would create a real licensing conflict with how this project is published, and more importantly, copying an existing implementation would defeat the actual point of the bonus, which is understanding the algorithm well enough to explain and defend it. Instead:

- The overall structure (Miyaguchi–Preneel construction, 10-round key-schedule-reuses-round-function design, round constants) was cross-verified against multiple independent, permissively-licensed sources before writing any code.
- The S-box/diffusion lookup table and round constants are values mathematically *required* by the ISO/IEC 10118-3 specification, every correct implementation produces the exact same table, the same way MD5's sine-derived constants or SHA-256's cube-root constants aren't a design choice, just a fact of the algorithm. Using the required constant values isn't the same as copying an implementation.
- All of the surrounding code: buffering, padding, the `t_whirlpool_ctx` context struct, `init`/`update`/`final`, integration into `t_hash_module`, was written fresh, matching this project's existing code style (`while` loops, snake_case, the same buffer-then-compress pattern as `md5.c`/`sha256.c`).

### Verification

Rather than trust hand-derived constants on faith, the implementation was checked against **the algorithm designers' own public-domain reference implementation** (Barreto & Rijmen, v3.0), compiled directly and compared byte-for-byte:

```
             ours                                                              reference
""      →    19fa61d7...42a66eb3                                          =    19fa61d7...42a66eb3
"abc"   →    4e2448a4...076d4eef5                                         =    4e2448a4...076d4eef5
"hello" →    0a25f55d...1c81cfd73                                         =    0a25f55d...1c81cfd73
```

...plus every file in `test_file/` (the same 0–129 and 1000/100000-byte boundary set used for MD5/SHA-256), all matching the reference implementation exactly, including the sizes that straddle Whirlpool's own padding boundary. Zero mismatches.

`whirlpool` also inherits every architectural guarantee already built into this project without any extra code: the generic `hash_handler` pipeline, the SIGINT/leak-safety handling, and the `-p`/`-q`/`-r`/`-s` flag formatting all work identically for a 64-byte digest as they do for a 16- or 32-byte one, because none of that code was ever written to assume a specific digest size in the first place.

---

## Testing

`tests/run_tests.sh` is a self-contained regression suite that:

- Verifies known test vectors (`md5("") = d41d8cd9...`, `sha256("abc") = ba7816bf...`, etc.)
- Compares output against the system's real `md5sum` / `sha256sum` across a range of boundary-length files (0, 1, 55, 56, 57, 63, 64, 65, 127, 128, 129, 1000, and 100000 bytes). These lengths specifically exercise the padding boundary logic, since 55/56/57 bytes straddle the exact point where MD5/SHA-256 padding either fits in one block or overflows into a second
- Checks every flag-combination output format directly against the worked examples from the project specification
- Checks basic error-path behavior (missing `-s` argument, nonexistent files)

Run it with:

```bash
make
./tests/run_tests.sh ./ft_ssl
```

A passing run confirms both the hashing math and the output formatting are byte-for-byte correct against real-world reference tools, not just "looks plausible."

---

## Adding a new algorithm

This is the scenario the whole architecture was built around, so here's what it actually takes:

1. Create `src/hash/<name>/<name>.c` and `.h`, implementing `<name>_init(void *state)`, `<name>_update(void *state, const uint8_t *data, size_t len)`, and `<name>_final(void *state, uint8_t *out)`, following the same `init` → buffer-and-compress → `final`-pads-and-serializes shape described above.
2. Define a context struct (state words, bit-length counter, internal buffer) sized appropriately for the new algorithm's block/state size.
3. Add one line to the table in `hash/registry.c`:
   ```c
   {"whirlpool", "WHIRLPOOL", whirlpool_init, whirlpool_update, whirlpool_final,
    WHIRLPOOL_DIGEST_SIZE, sizeof(t_whirlpool_ctx)},
   ```

Nothing in `dispatch.c`, `hash_handler.c`, `parser.c`, or `formatter.c` needs to change. If any of those files *do* need to change to support a new algorithm, that's a sign the abstraction has a gap worth revisiting, not something to route around with a special case.

---

## Development process & AI usage

## Sources

---

## Building

```bash
make        # builds ./ft_ssl
make clean  # removes object files
make fclean # removes object files and the binary
make re     # fclean + build
```

Requires a C compiler (`gcc`/`clang`) and a POSIX-compliant environment (Linux, or macOS/WSL with standard build tools).