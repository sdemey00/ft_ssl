for f in tests/files/*; do
    echo "=== $f ==="
    echo "md5:"
    ./ft_ssl md5 -q "$f"
    md5sum "$f" | cut -d' ' -f1
    echo "sha256:"
    ./ft_ssl sha256 -q "$f"
    sha256sum "$f" | cut -d' ' -f1
done