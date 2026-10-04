# Reproducibility package

This package accompanies *Degree-Preserving Structural Perturbations and CDCL Behavior in Tseitin Formulas*. It contains archived inputs and analysis code for the empirical results reported in the paper. It is deliberately analysis-only: no Kissat executable, solver launcher, or command that generates new runs is included.

This is the `v1.0.1-paper` corrective package. Personal Ubuntu filesystem paths present in the historical package were replaced with the portable markers documented in `PATH_SANITIZATION.md`. Scientific results and methods are unchanged.

## Scope and organization

- `experiments/degree_preserving_ring_transition_dense_n80_d4/`: discovery campaign (300 graphs), including archived graph/CNF inputs, measurements, frozen protocol, and summary.
- `experiments/degree_preserving_ring_transition_dense_n80_d4_replication_s20_s24_n100/`: independently seeded confirmation at 20 and 24 swaps (200 graphs), including archived inputs, results, metadata, and confirmatory outputs.
- `experiments/kissat_charge_presentation_pilot_s20_s24/`: eight-graph presentation pilot, with its frozen protocol, manifests, prepared CNFs, metadata, and outcomes.
- `experiments/kissat_combined_presentation_robustness_s20_s24_n60/`: prespecified presentation-robustness campaign (60 graphs, five presentations each), including selection, frozen folds, manifests, source/prepared CNFs, outcomes, and grouped analysis.
- `experiments/exploratory_aft_efficiency_modularity_20261003/`: exploratory censored AFT analyses of the discovery and confirmation campaigns.
- `experiments/exploratory_aft_score_uncertainty_20261003/`: conditional bootstrap uncertainty for archived AFT replication scores.
- `experiments/exploratory_effective_dose_robustness_20261004/`: exploratory effective-dose sensitivity for the robustness sample.
- `experiments/tseitin_encoding_proof_complexity_audit/`: separate theoretical applicability audit used to check the paper's proof-complexity framing; it is not an empirical result.
- `scripts/`: analysis-only scripts used for campaign summaries and paper figures.
- `report/figures_20261003/`: regenerated paper figures.
- `expected/`: snapshots of archived numerical outputs retained before the reproduction run.
- `verification/`: machine-readable and human-readable verification results and measured runtimes.

The paper distinguishes five roles. The discovery campaign identified structural associations; the independent confirmation campaign tested the frozen primary comparison; the eight-graph pilot exposed sensitivity to a controlled presentation change; the 60-graph campaign tested the prespecified repeated-presentation robustness criterion; and the AFT, score-uncertainty, quadratic-treewidth, and effective-dose analyses are explicitly exploratory or sensitivity analyses as labelled in the paper.

## Dependencies

The archived analyses recorded Python 3.12.3, NumPy 1.26.4, NetworkX 2.8.8, and SciPy 1.11.4. The delivery environment used Matplotlib 3.6.3. The effective-dose reconstruction additionally used a C++17 compiler, recorded as GCC 13.3.0. See `requirements-analysis.txt` and the per-analysis metadata files.

Create an isolated environment if desired:

```sh
python3 -m venv .analysis-venv
.analysis-venv/bin/python -m pip install -r requirements-analysis.txt
```

## Safe reproduction commands

Run these commands from the package root. They read archived solver outcomes and do not invoke Kissat:

```sh
python3 scripts/analyze_degree_preserving_ring_campaign.py --campaign-dir experiments/degree_preserving_ring_transition_dense_n80_d4
python3 scripts/analyze_degree_preserving_ring_replication.py --campaign-dir experiments/degree_preserving_ring_transition_dense_n80_d4_replication_s20_s24_n100
python3 experiments/exploratory_aft_efficiency_modularity_20261003/analyze.py
python3 experiments/exploratory_aft_score_uncertainty_20261003/bootstrap_scores.py
python3 experiments/kissat_combined_presentation_robustness_s20_s24_n60/analyze.py
python3 experiments/kissat_combined_presentation_robustness_s20_s24_n60/audit_campaign.py
python3 experiments/exploratory_effective_dose_robustness_20261004/analyze.py
python3 scripts/make_paper_figures_20261003.py
python3 scripts/verify_archived_results.py
```

The measured wall times from the delivery verification are in `verification/runtimes.tsv`. They are approximate and depend on hardware and software versions. The grouped robustness bootstrap and effective-dose sensitivity are the slowest steps.

## Reproducibility limits

The package reproduces analyses from recorded outcomes; it does not rerun the solver. Solver options, available binary hashes, seeds, selected instances, and input hashes are retained in campaign protocols and metadata. Raw solver-console directories were omitted because they are not needed by the analyses; the structured outcome tables are included. No proof certificates were recorded, so minimum proof size is not measurable from these artifacts. Exact treewidth was not computed: the paper uses the recorded min-fill upper bound. Wall-clock measurements remain specific to the recorded machine and execution conditions, and presentation experiments do not isolate every source of runtime variability.

Analysis scripts resolve `<LOCAL_PROJECT_ROOT>/...` against the extracted package root when an included input is required. `<LOCAL_KISSAT_PATH>` appears only in historical provenance fields; analysis-only reproduction does not require or invoke that executable.

For the older AFT evaluation, per-fold fitted coefficients and per-observation predictions were not historically saved; its later score interval is therefore conditional on saved fitted quantities and does not include retraining variability. The robustness CV interval is likewise conditional on the frozen out-of-fold predictions, as stated in the paper. These historical limitations are documented rather than reconstructed.

## Licensing

This package is multi-licensed by file category. Original project code and analysis scripts are MIT-licensed. The paper, author-created figures, and original experimental data and metadata are CC BY 4.0. Third-party software is not relicensed or redistributed. See `LICENSES.md` for the precise category statement.

## Integrity

`MANIFEST.sha256` contains SHA-256 digests for every delivered package file other than the manifest itself. Verify it from the package root with:

```sh
sha256sum -c MANIFEST.sha256
```

