# Portable-path correction

This delivery is the `v1.0.1-paper` portability correction. The scientific paper, numerical results, statuses, runtimes, metrics, seeds, commands, and input-content hashes are unchanged.

The frozen `v1.0-paper` reproducibility package contained provenance fields with an Ubuntu home-directory project root and a local Kissat executable path. In this corrective copy:

- project-root references are represented as `<LOCAL_PROJECT_ROOT>/...`;
- the locally installed solver executable is represented as `<LOCAL_KISSAT_PATH>`;
- 5,241 project-root occurrences and 705 solver-path occurrences were replaced across 514 text files;
- analysis path resolvers map `<LOCAL_PROJECT_ROOT>/...` to the extracted package root where required.

The replacements affect only location strings. They do not assert that the sanitized tables are byte-identical to their historical counterparts. SHA-256 values for the sanitized files and archive were regenerated. Hashes describing unchanged source graphs, CNFs, the historical solver binary, or other unchanged experimental inputs retain their original meaning.

The final PDF was checked separately: its document metadata and embedded strings contain no personal filesystem path, so the PDF was copied without modification.

The earlier `v1.0-paper` release and its Git history remain historical public artifacts. This corrective release does not rewrite or delete them.
