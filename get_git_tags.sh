cd ~/Development/Wizardry/Sorcery

for dir in ext/*; do
    if [ -d "$dir/.git" ]; then
        echo
        echo "=== $dir ==="
        git -C "$dir" remote get-url origin
        git -C "$dir" rev-parse HEAD
    fi
done
