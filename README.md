# Spectral Predictors for Tseitin Hardness: An Exploratory Study  
**Independent Research by Amandine Morin**

[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.17730888.svg)](https://doi.org/10.5281/zenodo.17730888)

---

## 📎 Citation

If you use this work, please cite:

**Amandine Morin**. (2025). *amandine-morin/Tseitin-Spectral-Complexity: Initial Public Version (v1.0.0).* Zenodo.  
https://doi.org/10.5281/zenodo.17730888

BibTeX:

```bibtex
@software{amandine_tseitin_spectral_2025,
  author       = {Amandine Morin},
  title        = {{amandine-morin/Tseitin-Spectral-Complexity: Initial Public Version}},
  month        = jan,
  year         = 2025,
  publisher    = {Zenodo},
  version      = {v1.0.0},
  doi          = {10.5281/zenodo.17730888},
  url          = {https://doi.org/10.5281/zenodo.17730888}
}
```

## 🔍 Overview

This repository accompanies an **exploratory preprint** investigating how  
**spectral graph structure** relates to the hardness of **Tseitin formulas**  
in the Resolution proof system.

The project is motivated by a **physics-first viewpoint**:  
if information has physical cost (Landauer), then distinguishing SAT from UNSAT  
should require a minimal **information budget** determined by the structure of the graph.

This work presents *conjectures and preliminary observations*, not formal proofs.

---

## 🔑 Core Hypothesis: The Spectral Predictor `\hat{deg}(G)`

We propose the following spectral quantity as a hardness predictor:

\[
\widehat{\deg}(G) =
\frac{n}{\sqrt{\frac{1}{d} + \frac{1}{\lambda_2}}}
\]

Where:

- `n` = number of vertices  
- `d` = average degree  
- `λ₂` = the spectral gap of the normalized Laplacian  

This blends **local density** and **global connectivity** into a single  
structural measure intended to approximate the *minimal incompressible information*  
any algorithm must process to solve a Tseitin instance.

This is **conjectural**, not a proven theorem.

---

## 📊 Empirical Observations (Preliminary)

Small-scale experiments (up to `n = 80`) show:

- A noticeable correlation between `\hat{deg}(G)` and SAT solver time  
- Canonical families behave as predicted (cycles, grids, expanders)  
- Expander-like graphs show an **exponential jump** in difficulty  

These results are **illustrative**, not statistically conclusive.

### Quick Access

- 📈 **Correlation Plot:**  
  `./figures/tseitin_complexity_analysis.png`
- 📄 **Full Preprint (PDF):**  
  `./report/minimal_proof_compression.pdf`

---

## 🧪 Reproducing the Experiments

### Requirements

| Library      | Role                                      |
|--------------|--------------------------------------------|
| `networkx`   | Graph generation & spectral computations   |
| `pysat`      | SAT solving via MiniSat backend            |
| `numpy`      | Linear algebra & numerics                  |
| `matplotlib` | Visualizations                             |

### Install dependencies

```bash
pip install -r requirements.txt
```

### Run experiments

```bash
python code/main_experiment.py
```

---

## ⚠️ Disclaimer

This is an **exploratory research project** containing:

- Conjectures (no theorems are proved)
- Preliminary empirical evidence
- Speculative connections between physics, spectral graph theory,  
  and proof complexity

Instance sizes are small, and the results should be viewed as  
**hypothesis-generating** rather than conclusive.

Feedback, testing, critique, and replication are warmly invited.

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

### Code  
MIT License.

### Research Text & Figures  
Creative Commons Attribution 4.0 International (CC BY 4.0).

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
2. Install **WSL** with an Ubuntu distribution and build Kissat at `/home/dinah/kissat/build/kissat` inside WSL.
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
1. Ensure the Kissat binary exists at `/home/dinah/kissat/build/kissat` inside WSL.
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
   - Invoke Kissat via `wsl /home/dinah/kissat/build/kissat <input> > <output>`.
   - Log results to `out/results.csv`.

### Notes
- Windows paths are converted to WSL paths using a manual `C:\\...` → `/mnt/c/...` conversion to keep redirection working inside WSL.
- XOR gates are encoded with the standard 4-clause Tseitin expansion for `z = x XOR y`.
- Each vertex gets a parity constraint; a single charged vertex ensures the overall instance is unsatisfiable.
