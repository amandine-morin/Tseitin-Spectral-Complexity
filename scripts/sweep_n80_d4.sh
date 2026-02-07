#!/usr/bin/env bash
set -euo pipefail

OUT=/tmp/sweep
BIN=./code/cpp/build/run_kissat

rm -rf "$OUT"
mkdir -p "$OUT"

echo "mode,seed,exit,cnf_hash,runtime_ms,status"

for mode in circulant config_model; do
  for seed in 0 1 2 3 4; do
    out="$OUT/${mode}_s${seed}"
    mkdir -p "$out"

    log="$OUT/log_${mode}_s${seed}.txt"

    set +e
    "$BIN" --n 80 --d 4 --seed "$seed" --graph_mode "$mode" --outdir "$out" > "$log"
    exitcode=$?
    set -e

    cnf=$(grep -m1 '^cnf_hash:' "$log" | awk '{print $2}')
    ms=$(grep -m1 '^runtime_ms:' "$log" | awk '{print $2}')
    status=$(grep -m1 '^solve_status:' "$log" | awk '{print $2}')

    echo "$mode,$seed,$exitcode,$cnf,$ms,$status"
  done
done
