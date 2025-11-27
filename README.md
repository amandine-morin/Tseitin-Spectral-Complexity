# Spectral Predictors for Tseitin Hardness: An Exploratory Study  
**Independent Research by Amandine Morin**

---

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

| Library    | Role                                     |
|------------|-------------------------------------------|
| `networkx` | Graph generation & spectral computations  |
| `pysat`    | SAT solving via MiniSat backend           |
| `numpy`    | Linear algebra & numerics                 |
| `matplotlib` | Visualizations                         |

### Install dependencies
```bash
pip install -r requirements.txt

### Run experiments
```bash
python code/main_experiment.py

### ⚖️ Licensing

The source code for the experiments (Python scripts in the `code/` folder) is released under the **MIT License**.

The text and figures of the research paper (in the `report/` folder) are licensed under the **Creative Commons Attribution 4.0 International License (CC BY 4.0)**, ensuring maximal reuse for academic purposes, provided proper citation is given.