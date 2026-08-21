#!/bin/bash
# ft_ssl regression test suite
# Compares digest output against system md5sum/sha256sum, and checks
# flag-combination formatting against the subject's worked examples.

set -uo pipefail

BIN="${1:-./ft_ssl}"
TESTDIR="$(dirname "$0")/../tests/files"
PASS=0
FAIL=0

green() { printf "\033[32m%s\033[0m\n" "$1"; }
red()   { printf "\033[31m%s\033[0m\n" "$1"; }

check() {
    local desc="$1"
    local got="$2"
    local want="$3"

    if [ "$got" = "$want" ]; then
        PASS=$((PASS + 1))
        # uncomment for verbose passes:
        # green "PASS: $desc"
    else
        FAIL=$((FAIL + 1))
        red "FAIL: $desc"
        echo "  expected: $want"
        echo "  got:      $got"
    fi
}

echo "== Known test vectors =="
check "md5 empty string"   "$($BIN md5 -q -s '')"                    "d41d8cd98f00b204e9800998ecf8427e"
check "md5 abc"            "$($BIN md5 -q -s 'abc')"                 "900150983cd24fb0d6963f7d28e17f72"
check "sha256 empty"       "$(printf '' | $BIN sha256 -q)"           "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"
check "sha256 abc"         "$($BIN sha256 -q -s 'abc')"              "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad"

echo "== Boundary-length files vs system tools =="
if [ -d "$TESTDIR" ]; then
    for f in "$TESTDIR"/*; do
        name=$(basename "$f")
        ours_md5=$($BIN md5 -q "$f" 2>/dev/null)
        real_md5=$(md5sum "$f" | cut -d' ' -f1)
        check "md5 $name" "$ours_md5" "$real_md5"

        ours_sha=$($BIN sha256 -q "$f" 2>/dev/null)
        real_sha=$(sha256sum "$f" | cut -d' ' -f1)
        check "sha256 $name" "$ours_sha" "$real_sha"
    done
else
    red "test_file/ not found, skipping boundary file tests"
fi

echo "== Flag formatting (from subject examples) =="

check "default stdin format" \
    "$(echo '42 is nice' | $BIN md5)" \
    "(stdin)= 35f1d6de0302e2086a4e472266efb3a9"

check "-p echo format" \
    "$(echo '42 is nice' | $BIN md5 -p)" \
    "(\"42 is nice\")= 35f1d6de0302e2086a4e472266efb3a9"

check "-q -r stdin" \
    "$(echo 'Pity the living.' | $BIN md5 -q -r)" \
    "e20c3b973f63482a778f3fd1869b7f25"

echo "And above all," > /tmp/ft_ssl_test_file
check "file format" \
    "$($BIN md5 /tmp/ft_ssl_test_file)" \
    "MD5 (/tmp/ft_ssl_test_file) = 53d53ea94217b259c11a5a2d104ec58a"

check "-r file format" \
    "$($BIN md5 -r /tmp/ft_ssl_test_file)" \
    "53d53ea94217b259c11a5a2d104ec58a /tmp/ft_ssl_test_file"

check "-s string format" \
    "$($BIN md5 -s "pity those that aren't following baerista on spotify.")" \
    'MD5 ("pity those that aren'"'"'t following baerista on spotify.") = a3c990a1964705d9bf0e602f44572f5f'

echo "== SHA-256 example =="
echo "https://www.42.fr/" > /tmp/ft_ssl_website
check "sha256 -q website" \
    "$($BIN sha256 -q /tmp/ft_ssl_website)" \
    "1ceb55d2845d9dd98557b50488db12bbf51aaca5aa9c1199eb795607a2457daf"

check "sha256 -s format" \
    "$($BIN sha256 -s '42 is nice')" \
    'SHA256 ("42 is nice") = b7e44c7a40c5f80139f0a50f3650fb2bd8d00b0d24667c4c2ca32c88e13b758f'

echo "== Error paths =="
$BIN md5 -s >/dev/null 2>&1
check "missing -s argument exits nonzero" "$?" "1"

$BIN md5 /tmp/definitely_does_not_exist_xyz >/dev/null 2>&1
check "nonexistent file exits nonzero" "$?" "1"

echo "== Multi-input ordering (subject transcript case) =="
echo "and above all," > /tmp/ft_ssl_file2
out=$(echo "GL HF let's go" | $BIN md5 -p -s "foo" /tmp/ft_ssl_test_file)
lines=$(echo "$out" | wc -l)
check "multi-input produces 3 lines" "$lines" "3"

echo
echo "-----------------------------------"
echo "Passed: $PASS  Failed: $FAIL"
[ "$FAIL" -eq 0 ] && green "ALL TESTS PASSED" || red "SOME TESTS FAILED"
exit $([ "$FAIL" -eq 0 ] && echo 0 || echo 1)