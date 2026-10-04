## Portable-path and licensing correction

This corrective release republishes the non-peer-reviewed preprint *Degree-Preserving Structural Perturbations and CDCL Behavior in Tseitin Formulas* with a sanitized reproducibility archive.

The correction is limited to portability, personal filesystem paths, and license documentation:

- Ubuntu home-directory project paths were replaced with `<LOCAL_PROJECT_ROOT>/...`;
- the historical local solver executable path was replaced with `<LOCAL_KISSAT_PATH>`;
- analysis path resolvers understand the portable project-root marker;
- original project code and analysis scripts are identified as MIT-licensed;
- the paper, author-created figures, and original experimental data and metadata are identified as CC BY 4.0;
- third-party software remains under its own licenses and is not redistributed by the package.

The scientific paper, numerical results, statuses, runtimes, metrics, seeds, commands, methods, conclusions, and hashes of unchanged experimental inputs are unchanged. The sanitized files and archive necessarily have new SHA-256 values, recorded in the attached sidecars and internal manifest.

The earlier `v1.0-paper` release remains available as a historical artifact and still contains the original local-path provenance. Use this `v1.0.1-paper` release for redistribution and Zenodo deposit.

### Release assets

- Unchanged final timestamped preprint PDF
- Sanitized reproducibility archive
- Multi-license statement and path-sanitization note
- SHA-256 sidecars for the PDF and sanitized archive
