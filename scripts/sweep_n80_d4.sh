#!/usr/bin/env bash
set -euo pipefail

OUT=/tmp/sweep
BIN=./code/cpp/build/run_kissat
KISSAT_BIN=${KISSAT_BIN:-kissat}

rm -rf "$OUT"
mkdir -p "$OUT"

if ! KISSAT_BIN=$(command -v "$KISSAT_BIN"); then
  echo "Kissat binary not found: set KISSAT_BIN to an executable path or add kissat to PATH." >&2
  echo "Example: KISSAT_BIN=/path/to/kissat bash scripts/sweep_n80_d4.sh" >&2
  exit 1
fi

if [[ ! -x "$BIN" ]]; then
  echo "run_kissat binary not found at $BIN. Build with: cmake --build code/cpp/build -j" >&2
  exit 1
fi

echo "mode,n,d,p,seed,exit,cnf_hash,runtime_ms,status"

for mode in circulant config_model; do
  p_value=0
  for seed in $(seq 0 19); do
    out="$OUT/${mode}_s${seed}"
    mkdir -p "$out"

    log="$OUT/log_${mode}_s${seed}.txt"

    set +e
    "$BIN" --n 80 --d 4 --seed "$seed" --graph_mode "$mode" --outdir "$out" --kissat "$KISSAT_BIN" > "$log"
    exitcode=$?
    set -e

    cnf=$(awk '/^cnf_hash:/{print $2; exit}' "$log" || true)
    ms=$(awk '/^runtime_ms:/{print $2; exit}' "$log" || true)
    status=$(awk '/^solve_status:/{print $2; exit}' "$log" || true)

    echo "$mode,80,4,$p_value,$seed,$exitcode,$cnf,$ms,$status"
  done
done
