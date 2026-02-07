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

Status: Archived v1 (Python-based exploratory pipeline).
See branch cpp-experiment for the current experimental pipeline using Kissat.

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
