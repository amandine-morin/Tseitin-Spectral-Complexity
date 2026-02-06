"""Automated sweep script for the Kissat Tseitin experiments.

This script executes the `./run_kissat` executable for multiple graph sizes and
regular degrees, records runtimes, and stores results in a CSV file. It is
intended to support reproducible experiments by providing consistent timeout
handling and logging.
"""
from __future__ import annotations

import csv
import random
import subprocess
import time
from pathlib import Path

# Experimental configuration.
D_VALUES = [3, 4, 5, 6]
N_VALUES = [40, 60, 80, 100, 120, 160, 200]
TRIALS_PER_SETTING = 5
TIMEOUT_SECONDS = 60
RESULTS_PATH = Path("results_kissat_sweep.csv")
MASTER_SEED = 123456789


def should_write_header(file_path: Path) -> bool:
    """Return True if the CSV header should be written for the given path."""
    return not file_path.exists()


def run_single_trial(n: int, d: int, timeout_seconds: int) -> tuple[float, int, int]:
    """Run a single trial and return runtime_ms, timeout_flag, and exit_code."""
    cmd = ["./run_kissat", "--n", str(n), "--d", str(d)]
    start = time.perf_counter()
    try:
        completed = subprocess.run(
            cmd,
            check=False,
            capture_output=True,
            text=True,
            timeout=timeout_seconds,
        )
        elapsed_ms = (time.perf_counter() - start) * 1000.0
        runtime_ms = elapsed_ms
        timeout_flag = 0
        exit_code = completed.returncode
    except subprocess.TimeoutExpired:
        elapsed_ms = (time.perf_counter() - start) * 1000.0
        runtime_ms = timeout_seconds * 1000.0
        timeout_flag = 1
        exit_code = -1
    return runtime_ms, timeout_flag, exit_code


def append_row(file_path: Path, row: list[object], write_header: bool) -> None:
    """Append a single row to the CSV file, writing the header if needed."""
    with file_path.open("a", newline="") as csvfile:
        writer = csv.writer(csvfile)
        if write_header:
            writer.writerow(
                ["n", "d", "trial_index", "seed", "runtime_ms", "timeout_flag", "exit_code"]
            )
        writer.writerow(row)


def main() -> None:
    """Run the experimental sweep and record results to CSV."""
    header_needed = should_write_header(RESULTS_PATH)

    rng = random.Random(MASTER_SEED)
    jobs: list[tuple[int, int, int, int]] = []
    for d in D_VALUES:
        for n in N_VALUES:
            for trial_index in range(1, TRIALS_PER_SETTING + 1):
                trial_seed = rng.randrange(0, 2**32)
                jobs.append((n, d, trial_index, trial_seed))

    rng.shuffle(jobs)

    for n, d, trial_index, trial_seed in jobs:
        print(
            f"Running d={d}, n={n}, trial {trial_index}/{TRIALS_PER_SETTING}, "
            f"seed={trial_seed}..."
        )
        runtime_ms, timeout_flag, exit_code = run_single_trial(n, d, TIMEOUT_SECONDS)

        if timeout_flag == 0 and exit_code != 0:
            print(
                f"Warning: run_kissat exited with code {exit_code} for n={n}, d={d}, "
                f"trial {trial_index}."
            )

        append_row(
            RESULTS_PATH,
            [n, d, trial_index, trial_seed, f"{runtime_ms:.3f}", timeout_flag, exit_code],
            header_needed,
        )
        header_needed = False


if __name__ == "__main__":
    main()
