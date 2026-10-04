# Degree-Preserving Structural Perturbations and CDCL Behavior in Tseitin Formulas

**Amandine Morin**

This repository accompanies a **preprint that has not been peer reviewed**. The
paper studies how controlled, degree-preserving perturbations of a 4-regular
ring are associated with fixed-budget behavior of a specific CDCL solver on
the corresponding Tseitin formulas.

The current public package is the `v1.0.1-paper` corrective release. It removes personal filesystem paths from the reproducibility archive and clarifies file-category licenses; scientific results are unchanged. The historical `v1.0-paper` release remains available but should not be used for redistribution.

## Paper and reproducibility materials

- [Final preprint (PDF)](deliveries/2026-10-04_13-26_Amandine-Morin_Tseitin-Structural-Perturbations_v1.0.1-paper/paper/2026-10-04_13-26_Amandine-Morin_Tseitin-Structural-Perturbations.pdf)
- [Final LaTeX source](deliveries/2026-10-04_13-26_Amandine-Morin_Tseitin-Structural-Perturbations_v1.0.1-paper/paper/2026-10-04_13-26_Amandine-Morin_Tseitin-Structural-Perturbations.tex)
- [Reproducibility instructions](deliveries/2026-10-04_13-26_Amandine-Morin_Tseitin-Structural-Perturbations_v1.0.1-paper/reproducibility/README.md)
- [Verification report](deliveries/2026-10-04_13-26_Amandine-Morin_Tseitin-Structural-Perturbations_v1.0.1-paper/reproducibility/verification/VERIFICATION_REPORT.md)
- [Release and frozen reproducibility archive](https://github.com/amandine-morin/Tseitin-Spectral-Complexity/releases/tag/v1.0.1-paper)

## Experimental design and findings

The study applies randomized double-edge swaps while preserving the number of
vertices, number of edges, and every vertex degree. Its stages have distinct
roles:

1. a 300-instance discovery campaign identified structural associations;
2. an independently seeded 200-instance confirmation tested frozen within-swap
   comparisons;
3. an eight-graph pilot examined sensitivity to formula presentation;
4. a prespecified robustness campaign used 60 outcome-blind selected graphs
   and five randomized presentations per graph; and
5. censored AFT, quadratic-treewidth, score-uncertainty, and effective-dose
   analyses were retained as exploratory or sensitivity analyses as labelled
   in the paper.

The independent confirmation replicated the prespecified association of
higher global efficiency and lower modularity with censoring at both studied
swap levels. In the repeated-presentation sample, 24 of 60 graphs had both
resolved and censored outcomes among their five presentations. This is an
observation for this sample, solver, presentation distribution, and 60-second
budget; it is not a universal instability rate for SAT benchmarks.

In the prespecified grouped analysis, the adjusted global-efficiency
coefficient had the expected negative direction and its graph-bootstrap
interval excluded zero. The additional held-out binary log-score gain was
positive but inconclusive because its conditional interval included zero.
Under the frozen decision rule, **robustness was not established**. This does
not constitute evidence that the association is absent.

The experiments measure observed search cost for one encoding and one solver.
They do not establish causality, exact-treewidth independence, minimum proof
size, or generalization to other graph sizes or solvers.

## Reproducing the paper analyses without Kissat

Download the reproducibility archive from the `v1.0.1-paper` release, verify its
SHA-256 sidecar, extract it, and follow its English `README.md`. The listed
commands operate only on preserved inputs and outcomes; they do not invoke a
solver. The package records the exact analysis commands, dependencies, measured
analysis runtimes, known historical limitations, and a SHA-256 manifest.

The frozen archive SHA-256 is:

```text
262d55a54b5571539022283495aa2af1808e215e9722bf96ba69ac2403ffacd8
```

The implementation material below documents the broader experimental pipeline.
It is not required for analysis-only reproduction of the published results.

Status: Archived v1 (Python-based exploratory pipeline).
See branch cpp-experiment for the current experimental pipeline using Kissat.

---

## 🧾 Experimental Disclaimer

The experiments in this repository are exploratory and **solver-dependent**.
Measured runtimes are **not** proofs of Resolution hardness. Results can change
with solver version or options, CPU/OS configuration, and other system factors.
Reproducibility depends on recording **seeds** and the **exact build environment**
(compiler, flags, and solver binary).

---

## ⏱️ Interpreting Timeouts as Censored Data

When runs hit a timeout, treat those measurements as **right-censored** rather
than as exact runtimes. For reporting, include (i) the **timeout rate** (fraction
of censored runs) and (ii) the **median of successful runs** to summarize the
resolved instances without biasing results. For visualization, use **separate
plots**: one for successful-run runtimes (e.g., median/IQR or log-scale scatter)
and another for timeout rates versus size/degree. This avoids conflating solver
speed with censoring effects and keeps comparisons scientifically interpretable.

---

## 📚 Licensing

### Original project code and analysis scripts

MIT License.

### Paper, author-created figures, and original experimental data and metadata

Creative Commons Attribution 4.0 International (CC BY 4.0).

### Third-party components

Third-party software is not relicensed. The reproducibility package does not redistribute a Kissat executable, a vendored Python environment, or source code from named Python dependencies. See the corrective delivery's `LICENSES.md` for the precise category statement.

---

## 💬 Feedback & Collaboration

This project is released in the spirit of **open scientific exploration**.  
If you test, extend, or refute any aspect of this work, please share your findings.

Comments and discussions are welcome.

---

## 🛠️ C++ Tseitin CNF Generator & Kissat Runner

This repository now includes a minimal, modular C++17 project that builds d-regular graphs, generates Tseitin contradictions, writes DIMACS CNF files, and runs the Kissat SAT solver through WSL. The C++ code lives in `code/cpp` to keep it separated from the Python experiments in `code/`. It is organized for Windows + WSL workflows and compiles with MSVC (Visual Studio 2022).

### Project Layout

```
code/
  main_experiment.py
  cpp/
    CMakeLists.txt
    include/
      graph.hpp
      kissat_runner.hpp
      tseitin_cnf.hpp
    src/
      graph.cpp
      kissat_runner.cpp
      tseitin_cnf.cpp
      main.cpp
```

### Build (VS Code on Windows)
1. Install **Visual Studio 2022 Build Tools** with the C++ workload and **CMake**.
2. Install **WSL** with an Ubuntu distribution and build Kissat at `<path-to-kissat>/build/kissat` inside WSL.
3. Open the repository folder (or `code/cpp` directly) in VS Code (Windows side) and install the **CMake Tools** extension.
4. Configure the project with `code/cpp/CMakeLists.txt` as the source:
   - Command Palette → `CMake: Select a Kit` → choose **Visual Studio 17 2022** (x64).
   - Command Palette → `CMake: Configure` (ensure the source directory is `code/cpp`).
5. Build:
   - Command Palette → `CMake: Build` → target `tseitin_app`.
6. The executable will be placed under `build/Debug/` or `build/Release/` (relative to the repository root if you configure with `-B build`).

Command-line equivalent:
```powershell
cmake -S code/cpp -B build
cmake --build build --config Release
```

### Running Experiments
1. Configure SAT solver settings in `code/cpp/solver_config.ini`:
   - set `kissat_path` and `minisat_path` once
   - set a uniform `timeout_seconds` for both solvers
   - switch solver by editing one line only: `solver=kissat` or `solver=minisat`
2. From a Developer Command Prompt (or VS Code terminal) at the repository root, run the built executable:
   ```
   build/Debug/tseitin_app.exe
   ```
   or
   ```
   build/Release/tseitin_app.exe
   ```
3. The program will:
   - Build several d-regular graphs.
   - Generate Tseitin CNFs in `out/*.cnf`.
   - Invoke the configured SAT solver (Kissat or Minisat).
   - Log results to `out/results.csv`.

For the standalone generator/runner (`run_kissat`), you can use config or override at runtime:
```bash
# Config only
code/cpp/build/run_kissat --n 80 --d 4 --config code/cpp/solver_config.ini

# Override solver
code/cpp/build/run_kissat --n 80 --d 4 --solver minisat --solver-path /usr/bin/minisat

# Override timeout
code/cpp/build/run_kissat --n 80 --d 4 --timeout 120
```

### Notes
- Windows paths are converted to WSL paths using a manual `C:\\...` → `/mnt/c/...` conversion to keep redirection working inside WSL.
- XOR gates are encoded with the standard 4-clause Tseitin expansion for `z = x XOR y`.
- Each vertex gets a parity constraint; a single charged vertex ensures the overall instance is unsatisfiable.
