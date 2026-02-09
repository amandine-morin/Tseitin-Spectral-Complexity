cat > scripts/test_d0_equivalence.sh <<'EOF'
#!/usr/bin/env bash
set -euo pipefail

BIN="./code/cpp/build/run_kissat"
OUT="/tmp/tseitin_d0_equiv"
rm -rf "$OUT"
mkdir -p "$OUT"

# Ensure binary exists
if [[ ! -x "$BIN" ]]; then
  echo "run_kissat not found at $BIN. Run: scripts/build_cpp.sh" >&2
  exit 1
fi

circ_log="$OUT/circ.txt"
ws_log="$OUT/ws.txt"

"$BIN" --n 10 --d 0 --seed 0 --graph_mode circulant --outdir "$OUT/circ" --kissat /bin/true > "$circ_log"
"$BIN" --n 10 --d 0 --seed 0 --graph_mode watts_strogatz --p 0.0 --outdir "$OUT/ws" --kissat /bin/true > "$ws_log"

circ_hash=$(awk '/^cnf_hash:/{print $2; exit}' "$circ_log")
ws_hash=$(awk '/^cnf_hash:/{print $2; exit}' "$ws_log")

echo "circulant hash:      $circ_hash"
echo "watts_strogatz p=0:  $ws_hash"

if [[ "$circ_hash" != "$ws_hash" ]]; then
  echo "FAIL: hashes differ (d=0 should match)" >&2
  exit 1
fi

echo "OK: d=0 equivalence holds"
EOF

chmod +x scripts/test_d0_equivalence.sh
