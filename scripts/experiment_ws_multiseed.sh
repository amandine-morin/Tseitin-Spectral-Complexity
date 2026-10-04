#!/usr/bin/env bash
set -euo pipefail

# ----------------------------------------
# Configuration
# ----------------------------------------
RUNNER="./code/cpp/build/run_kissat"
OUTDIR="results_ws_multiseed"
CSV="$OUTDIR/results.csv"

N=80
D=4
TIMEOUT=60

SEEDS=(0 1 2 3 4)
PS=(0 0.01 0.02 0.05)
SOLVERS=(kissat minisat)

# ----------------------------------------
# Checks
# ----------------------------------------
if [[ ! -x "$RUNNER" ]]; then
  echo "Error: runner not found or not executable: $RUNNER" >&2
  exit 1
fi

mkdir -p "$OUTDIR"

# ----------------------------------------
# CSV header
# ----------------------------------------
echo "seed,p,solver,runtime_ms,solve_status,cnf_hash,exit_code" > "$CSV"

# ----------------------------------------
# Sweep
# ----------------------------------------
for SEED in "${SEEDS[@]}"; do
  for SOLVER in "${SOLVERS[@]}"; do
    for P in "${PS[@]}"; do
      echo "Running seed=$SEED solver=$SOLVER p=$P ..."

      set +e
      OUT=$("$RUNNER" \
        --n "$N" \
        --d "$D" \
        --seed "$SEED" \
        --graph_mode watts_strogatz \
        --p "$P" \
        --solver "$SOLVER" \
        --timeout "$TIMEOUT" 2>&1)
      EXIT_CODE=$?
      set -e

      RUNTIME=$(echo "$OUT" | awk '/runtime_ms:/ {print $2}')
      STATUS=$(echo "$OUT" | awk '/solve_status:/ {print $2}')
      HASH=$(echo "$OUT" | awk '/cnf_hash:/ {print $2}')

      # Fallback safety in case parsing fails
      RUNTIME=${RUNTIME:-NA}
      STATUS=${STATUS:-UNKNOWN}
      HASH=${HASH:-NA}

      echo "$SEED,$P,$SOLVER,$RUNTIME,$STATUS,$HASH,$EXIT_CODE" >> "$CSV"

      echo "Done seed=$SEED solver=$SOLVER p=$P status=$STATUS runtime=$RUNTIME exit=$EXIT_CODE"
      echo "----------------------------------------"
    done
  done
done

echo "Finished."
echo "Results saved to: $CSV"