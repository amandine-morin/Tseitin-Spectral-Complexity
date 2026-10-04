import os
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# --------------------------------------------------------------------
# Locate CSV robustly relative to this script
# --------------------------------------------------------------------
BASE_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
csv_path = os.path.join(BASE_DIR, "cpp", "work_heavy", "results_kissat_heavy.csv")

print(f"Loading CSV from: {csv_path}")
df = pd.read_csv(csv_path)

# Ensure a global figures directory exists
FIG_DIR = os.path.join(BASE_DIR, "figures")
os.makedirs(FIG_DIR, exist_ok=True)

# --------------------------------------------------------------------
# Timeout reconstruction if missing
# --------------------------------------------------------------------
if "timed_out" not in df.columns:
    df["timed_out"] = (
        (df["exit_code"] == 0) |
        (df["runtime_ms"] >= 59000)
    ).astype(int)

# --------------------------------------------------------------------
# Group by n and d
# --------------------------------------------------------------------
grouped = df.groupby(["n", "d"]).agg(
    median_runtime=("runtime_ms", "median"),
    timeout_rate=("timed_out", "mean"),
).reset_index()

degrees = sorted(grouped["d"].unique())

# --------------------------------------------------------------------
# Plot 1: runtime vs n
# --------------------------------------------------------------------
plt.figure(figsize=(10, 6))
for d in degrees:
    sub = grouped[grouped["d"] == d]
    ok = sub[sub["timeout_rate"] == 0]
    hard = sub[sub["timeout_rate"] > 0]

    if not ok.empty:
        plt.plot(ok["n"], ok["median_runtime"], marker="o", label=f"d={d} no-timeout")
    if not hard.empty:
        plt.scatter(hard["n"], hard["median_runtime"], marker="x", s=80, label=f"d={d} timeout")

plt.xlabel("n")
plt.ylabel("runtime (ms)")
plt.title("Kissat runtime vs n (timeouts marked)")
plt.grid(True, alpha=0.3)
plt.legend()
plt.tight_layout()
plt.savefig(os.path.join(FIG_DIR, "runtime_vs_n_with_timeouts.png"), dpi=200)
plt.close()

# --------------------------------------------------------------------
# Plot 2: runtime vs n (log)
# --------------------------------------------------------------------
plt.figure(figsize=(10, 6))
for d in degrees:
    sub = grouped[grouped["d"] == d]
    ok = sub[sub["timeout_rate"] == 0]
    hard = sub[sub["timeout_rate"] > 0]

    if not ok.empty:
        plt.plot(ok["n"], ok["median_runtime"], marker="o", label=f"d={d} no-timeout")
    if not hard.empty:
        plt.scatter(hard["n"], hard["median_runtime"], marker="x", s=80, label=f"d={d} timeout")

plt.yscale("log")
plt.xlabel("n")
plt.ylabel("runtime (ms, log)")
plt.title("Kissat runtime vs n (log scale)")
plt.grid(True, which="both", alpha=0.3)
plt.legend()
plt.tight_layout()
plt.savefig(os.path.join(FIG_DIR, "runtime_vs_n_log_with_timeouts.png"), dpi=200)
plt.close()

# --------------------------------------------------------------------
# Plot 3: timeout heatmap
# --------------------------------------------------------------------
timeout_matrix = grouped.pivot(index="d", columns="n", values="timeout_rate")

plt.figure(figsize=(10, 6))
im = plt.imshow(timeout_matrix, cmap="Reds", aspect="auto", vmin=0, vmax=1)
plt.colorbar(im, label="timeout rate")

plt.xticks(range(len(timeout_matrix.columns)), timeout_matrix.columns)
plt.yticks(range(len(timeout_matrix.index)), timeout_matrix.index)

plt.xlabel("n")
plt.ylabel("d")
plt.title("Timeout phase diagram")

plt.tight_layout()
plt.savefig(os.path.join(FIG_DIR, "timeout_phase_diagram.png"), dpi=200)
plt.close()

print("Plots generated successfully!")
