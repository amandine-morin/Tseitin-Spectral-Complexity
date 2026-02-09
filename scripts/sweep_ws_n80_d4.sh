#!/usr/bin/env bash
set -euo pipefail

OUT=/tmp/sweep_ws
BIN=./code/cpp/build/run_kissat
KISSAT_BIN=${KISSAT_BIN:-/home/dinah/kissat/build/kissat}

rm -rf "$OUT"
mkdir -p "$OUT"

if [[ ! -x "$KISSAT_BIN" ]]; then
  echo "Kissat binary not found at $KISSAT_BIN." >&2
  echo "Example: KISSAT_BIN=/path/to/kissat bash scripts/sweep_ws_n80_d4.sh" >&2
  exit 1
fi

if [[ ! -x "$BIN" ]]; then
  echo "run_kissat binary not found at $BIN. Build with: cmake --build code/cpp/build -j" >&2
  exit 1
fi

echo "mode,n,d,p,seed,exit,cnf_hash,runtime_ms,status"

mode=watts_strogatz
n=80
d=4

for p in 0 0.01 0.05 0.1 0.2 0.5 1.0; do
  for seed in $(seq 0 19); do
    out="$OUT/${mode}_p${p}_s${seed}"
    mkdir -p "$out"

    log="$OUT/log_${mode}_p${p}_s${seed}.txt"

    set +e
    "$BIN" --n "$n" --d "$d" --seed "$seed" --graph_mode "$mode" --p "$p" --outdir "$out" --kissat "$KISSAT_BIN" > "$log"
    exitcode=$?
    set -e

    cnf=$(awk '/^cnf_hash:/{print $2; exit}' "$log" || true)
    ms=$(awk '/^runtime_ms:/{print $2; exit}' "$log" || true)
    status=$(awk '/^solve_status:/{print $2; exit}' "$log" || true)

    echo "$mode,$n,$d,$p,$seed,$exitcode,$cnf,$ms,$status"
  done
done
