# Zenodo metadata

## Record type

- Upload type: Publication
- Publication type: Preprint

## Title

Degree-Preserving Structural Perturbations and CDCL Behavior in Tseitin Formulas

## Creator

- Name: Amandine Morin
- Affiliation: Not supplied
- ORCID: Not supplied

## Publication date

2026-10-04

## Version

v1.0.1-paper

## Description / abstract

Tseitin formulas provide a controlled setting in which graph structure can be related to propositional proof search. We study randomized degree-preserving double-edge swaps of a 4-regular ring on 80 vertices, holding the 160 edges and complete degree sequence fixed. In a 300-instance discovery campaign with instrumented Kissat 4.0.4, the fraction exceeding a 60-second budget rises from 3.3% at 16 requested swaps to 36.7% at 20, 76.7% at 24, 96.7% at 32, and 100% at 48 and 80. A separate 200-instance confirmation replicated the prespecified within-swap associations: runs censored under the campaign presentation had higher global efficiency and lower modularity at both 20 and 24 swaps.

Exploratory censored accelerated-failure-time (AFT) analyses yielded positive predictive gains for global efficiency in several evaluations beyond the stated approximate baselines. After a fixed-graph pilot, we prespecified a robustness campaign with 60 outcome-blind selected graphs and five joint variable-renumbering and clause-order presentations each. In this sample, 24 of 60 graphs exhibited both resolved and censored outcomes across the five presentations. Higher efficiency retained a negative adjusted association with resolution probability (coefficient −1.525, graph-bootstrap 95% interval [−2.446, −0.688]), including after a treewidth-squared sensitivity, but its held-out log-score gain was +0.02265 with interval [−0.01130, +0.05959]. Under the frozen three-part rule, robustness was not established: the adjusted association was recovered, whereas additional predictive value was inconclusive. This does not imply absence of an effect.

## Keywords

- Tseitin formulas
- SAT solving
- CDCL
- degree-preserving graph perturbations
- double-edge swaps
- graph structure
- global efficiency
- treewidth
- censored runtime
- reproducibility

## Related identifiers

- Repository: https://github.com/amandine-morin/Tseitin-Spectral-Complexity
- GitHub release: https://github.com/amandine-morin/Tseitin-Spectral-Complexity/releases/tag/v1.0.1-paper
- Relation: Is supplement to / Is documented by the GitHub release

No DOI is supplied here. Do not replace or connect an earlier Zenodo record automatically.

## Files to upload

- `paper/2026-10-04_13-26_Amandine-Morin_Tseitin-Structural-Perturbations.pdf`
- `2026-10-04_13-26_Amandine-Morin_Tseitin-Structural-Perturbations_Reproducibility_v1.0.1.tar.gz`
- the corresponding `.sha256` sidecar files

## Rights and licenses

- Original project code and analysis scripts: MIT License, under the repository root `LICENSE`.
- Paper, author-created figures, original experimental data, and original experimental metadata: Creative Commons Attribution 4.0 International (CC BY 4.0).
- Third-party software: not relicensed. The package does not include a Kissat binary, a vendored Python environment, or source code from named Python dependencies; those components retain their own licenses.

This is a multi-license deposit. Do not describe the MIT license as covering the paper or data, and do not describe CC BY 4.0 as relicensing third-party software. Include `LICENSES.md` with the deposit and repeat these file-category terms in Zenodo's rights or description field. If the Zenodo interface requires one record-level license, select CC BY 4.0 for the research deposit while retaining the embedded MIT notice for code and the explicit third-party exclusion.

## Funding and communities

- Funding: Not supplied
- Communities: None specified

