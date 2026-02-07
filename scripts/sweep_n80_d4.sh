#!/usr/bin/env bash
set -e

OUT=/tmp/sweep
BIN=./build/run_kissat

rm -rf "$OUT"
mkdir -p "$OUT"

echo "mode,seed,exit,cnf_hash,runtime_ms,status"

for mode in circulant config_model; do
  for seed in 0 1 2 3 4; do
    out="$OUT/${mode}_s${seed}"
    mkdir -p "$out"

    $BIN --n 80 --d 4 --seed "$seed" --graph_mode "$mode" --outdir "$out" \
      > "$OUT/log_${mode}_s${seed}.txt"
    exitcode=$?

    cnf=$(grep -m1 '^cnf_hash:' "$OUT/log_${mode}_s${seed}.txt" | awk '{print $2}')
    ms=$(grep -m1 '^runtime_ms:' "$OUT/log_${mode}_s${seed}.txt" | awk '{print $2}')
    status=$(grep -m1 '^solve_status:' "$OUT/log_${mode}_s${seed}.txt" | awk '{print $2}')

    echo "$mode,$seed,$exitcode,$cnf,$ms,$status"
  done
done
