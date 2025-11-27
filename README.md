# A Physics-First Formulation of the Minimal Proof Compression Conjecture

**Researcher:** Amandine MORIN (Independent Research)

## 🔬 Project Overview
This project investigates the link between the **spectral properties of graphs** and the computational complexity required to solve the corresponding **Tseitin formulas** (a core problem in complexity theory). The approach is motivated by a physics-first perspective on information bounds in computation.

***

## 🔑 Key Contribution: The Spectral Predictor $\widehat{\deg}(G)$

We introduce the spectral metric $\widehat{\deg}(G)$, which combines local density ($d$) and global expansion ($\lambda_2$), to predict the minimal required information budget—and thus, the hardness—of Resolution proofs:

$$\widehat{\deg}(G) = \frac{n}{\sqrt{\frac{1}{d}+\frac{1}{\lambda_2}}}$$

***

## 📈 Key Result (Empirical Validation)
The empirical results demonstrate a strong correlation and an **exponential jump** in solver time (MiniSat) as $\widehat{\deg}(G)$ increases, confirming the metric's validity for expander graphs (see the full report for details).

* [Correlation Plot (PNG)](./figures/tseitin_complexity_analysis.png)
* [Full Research Paper (PDF)](./report/minimal_proof_compression.pdf)

***

## 🛠️ Requirements & Execution
The experiment requires Python (3.x) and the following libraries:

| Library | Purpose |
| :--- | :--- |
| `pysat` | SAT solver interface (MiniSat backend) |
| `networkx` | Graph generation and spectral analysis |
| `numpy`, `matplotlib` | Mathematical computations and visualization |

To install dependencies, please create and activate a virtual environment, then run:
```bash
pip install -r requirements.txt

To run the experiment, use:
```bash
python code/main_experiment.py

## ⚖️ Licensing

The source code for the experiments (Python scripts in the `code/` folder) is released under the **MIT License**.

The text and figures of the research paper (in the `report/` folder) are licensed under the **Creative Commons Attribution 4.0 International License (CC BY 4.0)**, ensuring maximal reuse for academic purposes, provided proper citation is given.