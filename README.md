# ft_ssl
A C implementation of cryptographic hashing algorithms inspired by OpenSSL's command-line interface.

# Overview
ft_ssl is an introduction to cryptographic hashing and low-level security programming. The project focuses on recreating part of the functionality provided by OpenSSL, implementing hashing algorithms directly in C rather than relying on existing cryptographic libraries.

The mandatory part of the project covers MD5 and SHA-256, along with command-line parsing, standard input/output handling, file input, string hashing, and several output-formatting flags.

The main goal is not simply to reproduce the output of existing tools, but to understand what happens underneath them. This includes working with bitwise operations, fixed-width integer arithmetic, message padding, block processing, and the internal compression functions used by cryptographic hash algorithms.
