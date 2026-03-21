#!/bin/bash
set -euo pipefail

OUTDIR="results_ws"
mkdir -p "$OUTDIR"

N=80
D=4
SEED=0

PS=(0 0.01 0.02 0.05 0.1 0.2 0.3 0.5)

echo "p,solver,runtime_ms,solve_status,cnf_hash" > "$OUTDIR/results.csv"

for SOLVER in kissat minisat; do
  for P in "${PS[@]}"; do
    echo "Running solver=$SOLVER p=$P ..."

    OUT=$(./code/cpp/build/run_kissat \
      --n "$N" \
      --d "$D" \
      --seed "$SEED" \
      --graph_mode watts_strogatz \
      --p "$P" \
      --solver "$SOLVER" 2>&1 || true)

    RUNTIME=$(echo "$OUT" | awk '/runtime_ms:/ {print $2}')
    STATUS=$(echo "$OUT" | awk '/solve_status:/ {print $2}')
    HASH=$(echo "$OUT" | awk '/cnf_hash:/ {print $2}')

    echo "$P,$SOLVER,$RUNTIME,$STATUS,$HASH" >> "$OUTDIR/results.csv"
    echo "Done p=$P solver=$SOLVER status=$STATUS runtime=$RUNTIME"
    echo "----------------------------------------"
  done
done

echo "Finished. Results in $OUTDIR/results.csv"