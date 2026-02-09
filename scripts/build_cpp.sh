cat > scripts/build_cpp.sh <<'EOF'
#!/usr/bin/env bash
set -euo pipefail

BUILD_DIR="code/cpp/build"
SRC_DIR="code/cpp"

# Configure if build dir is missing or not configured yet.
if [[ ! -d "$BUILD_DIR" ]] || [[ ! -f "$BUILD_DIR/CMakeCache.txt" ]]; then
  cmake -S "$SRC_DIR" -B "$BUILD_DIR"
fi

cmake --build "$BUILD_DIR" -j
EOF

chmod +x scripts/build_cpp.sh
