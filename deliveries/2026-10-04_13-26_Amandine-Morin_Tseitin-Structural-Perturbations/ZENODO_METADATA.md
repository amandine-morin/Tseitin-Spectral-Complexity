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

v1.0-paper

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
- GitHub release: https://github.com/amandine-morin/Tseitin-Spectral-Complexity/releases/tag/v1.0-paper
- Relation: Is supplement to / Is documented by the GitHub release

No DOI is supplied here. Do not replace or connect an earlier Zenodo record automatically.

## Files to upload

- `paper/2026-10-04_13-26_Amandine-Morin_Tseitin-Structural-Perturbations.pdf`
- `2026-10-04_13-26_Amandine-Morin_Tseitin-Structural-Perturbations_Reproducibility.tar.gz`
- the corresponding `.sha256` sidecar files

## Rights and licenses

- Repository code covered by the root `LICENSE`: MIT License.
- Paper and author-created figures: CC BY 4.0, as declared in the repository README.
- Archived empirical data and metadata: no separate explicit data license was found. Do not assign a broader license to those files without an explicit rights decision by the author.
- The package does not include a Kissat binary or a vendored virtual environment. Named Python dependencies remain subject to their own licenses.

Because the upload contains materials under different terms and the archived data have no separate explicit license, do not select a single Zenodo license as applying indiscriminately to every file. Record the file-specific terms above in the description or rights notes; if Zenodo requires one record-level choice, resolve the data-license question before submission.

## Funding and communities

- Funding: Not supplied
- Communities: None specified

