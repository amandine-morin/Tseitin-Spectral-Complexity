from pathlib import Path

import pandas as pd
import matplotlib.pyplot as plt

# -----------------------------
# Paths
# -----------------------------
CSV_PATH = Path("results_ws_multiseed/results.csv")
FIG_DIR = Path("figures")
FIG_DIR.mkdir(exist_ok=True)

KISSAT_RUNTIME_FIG = FIG_DIR / "ws_runtime_kissat_clean.png"
MINISAT_RUNTIME_FIG = FIG_DIR / "ws_runtime_minisat_clean.png"
TIMEOUT_FIG = FIG_DIR / "ws_timeout_rate_multiseed.png"

# -----------------------------
# Load data
# -----------------------------
df = pd.read_csv(CSV_PATH)

# Keep only UNSAT solved instances for runtime plots
success = df[df["solve_status"] == "UNSAT"].copy()

# -----------------------------
# Figure: Kissat runtime (final clean version)
# -----------------------------
kissat_df = success[success["solver"] == "kissat"].copy()

plt.figure(figsize=(8, 6))

if not kissat_df.empty:
    grouped = kissat_df.groupby("p")["runtime_ms"]

    median = grouped.median()
    p_values = median.index.to_numpy()

    # Median curve (main result)
    plt.plot(
        p_values,
        median.to_numpy(),
        marker="o",
        linewidth=3,
        markersize=7,
        label="Median runtime",
        zorder=3,
    )

    # Raw runs (important for credibility)
    plt.scatter(
        kissat_df["p"],
        kissat_df["runtime_ms"],
        s=35,
        alpha=0.35,
        color="black",
        zorder=2,
        label="Runs",
    )

plt.yscale("log")

plt.xlabel("Watts-Strogatz rewiring probability p")
plt.ylabel("Runtime (ms, log scale)")
plt.title("Tseitin runtime under Watts-Strogatz perturbations (kissat)")

# Timeout threshold (clean + subtle)
plt.axvline(x=0.02, linestyle="--", alpha=0.3)

plt.grid(alpha=0.25, which="both")

plt.legend(frameon=True)
plt.tight_layout()
plt.savefig(KISSAT_RUNTIME_FIG, dpi=300, bbox_inches="tight")
plt.close()

# -----------------------------
# Figure 2: Minisat runtime
# Raw solved points only
# -----------------------------
minisat_df = success[success["solver"] == "minisat"].copy()

plt.figure(figsize=(8, 6))

if not minisat_df.empty:
    for seed in sorted(minisat_df["seed"].unique()):
        sub = minisat_df[minisat_df["seed"] == seed].sort_values("p")
        plt.plot(
            sub["p"],
            sub["runtime_ms"],
            marker="o",
            linewidth=2,
            markersize=6,
            label=f"seed={seed}",
        )

plt.yscale("log")
plt.xlabel("Watts-Strogatz rewiring probability p")
plt.ylabel("Runtime (ms, log scale)")
plt.title("Tseitin runtime under Watts-Strogatz perturbations (minisat)")

plt.axvline(x=0.01, linestyle="--", alpha=0.4)

# Annotate missing higher-p points due to timeout
all_p = sorted(df[df["solver"] == "minisat"]["p"].unique())
plotted_p = sorted(minisat_df["p"].unique()) if not minisat_df.empty else []

if len(plotted_p) < len(all_p):
    plt.text(
        0.5,
        0.55,
        "Higher p values timeout",
        transform=plt.gca().transAxes,
        ha="center",
        fontsize=11,
        alpha=0.7,
    )

plt.grid(alpha=0.25, which="both")
if not minisat_df.empty:
    plt.legend(title="Seed", frameon=True)

plt.tight_layout()
plt.savefig(MINISAT_RUNTIME_FIG, dpi=300, bbox_inches="tight")
plt.close()

# -----------------------------
# Figure 3: Timeout rate
# -----------------------------
timeout_df = df.copy()
timeout_df["timed_out"] = timeout_df["solve_status"] == "UNKNOWN"

grouped_timeout = (
    timeout_df.groupby(["solver", "p"])["timed_out"]
    .mean()
    .reset_index()
)

plt.figure(figsize=(8, 6))

for solver in sorted(grouped_timeout["solver"].unique()):
    sub = grouped_timeout[grouped_timeout["solver"] == solver].sort_values("p")
    plt.plot(
        sub["p"],
        sub["timed_out"],
        marker="o",
        linewidth=2,
        markersize=6,
        label=solver,
    )

plt.xlabel("Watts-Strogatz rewiring probability p")
plt.ylabel("Timeout rate")
plt.title("Timeout rate under Watts-Strogatz perturbations")

plt.ylim(-0.02, 1.02)
plt.axvspan(0.01, 0.02, alpha=0.1)
plt.axvline(x=0.01, linestyle="--", alpha=0.4)

plt.grid(alpha=0.25)
plt.legend(title="Solver", frameon=True)

plt.tight_layout()
plt.savefig(TIMEOUT_FIG, dpi=300, bbox_inches="tight")
plt.close()

# -----------------------------
# Done
# -----------------------------
print("Saved:")
print(KISSAT_RUNTIME_FIG)
print(MINISAT_RUNTIME_FIG)
print(TIMEOUT_FIG)