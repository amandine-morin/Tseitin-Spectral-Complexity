#!/usr/bin/env bash
set -euo pipefail

OUT=/tmp/sweep
BIN=./code/cpp/build/run_kissat

rm -rf "$OUT"
mkdir -p "$OUT"

echo "mode,seed,exit,cnf_hash,runtime_ms,status"

for mode in circulant config_model; do
  for seed in $(seq 0 49); do
    out="$OUT/${mode}_s${seed}"
    mkdir -p "$out"

    log="$OUT/log_${mode}_s${seed}.txt"

    set +e
    "$BIN" --n 80 --d 4 --seed "$seed" --graph_mode "$mode" --outdir "$out" > "$log"
    exitcode=$?
    set -e

    cnf=$(awk '/^cnf_hash:/{print $2; exit}' "$log" || true)
    ms=$(awk '/^runtime_ms:/{print $2; exit}' "$log" || true)
    status=$(awk '/^solve_status:/{print $2; exit}' "$log" || true)

    echo "$mode,$seed,$exitcode,$cnf,$ms,$status"
  done
done
