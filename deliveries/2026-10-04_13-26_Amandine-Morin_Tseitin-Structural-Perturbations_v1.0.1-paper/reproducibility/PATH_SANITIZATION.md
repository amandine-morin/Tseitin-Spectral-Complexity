# Portable paths

This is the `v1.0.1-paper` corrective package. Historical absolute paths rooted in the author's Ubuntu home directory were replaced only in this copy.

- `<LOCAL_PROJECT_ROOT>/...` denotes the original repository root. Analysis scripts resolve this marker against the extracted package root when they need an included input.
- `<LOCAL_KISSAT_PATH>` denotes the historical local Kissat executable. It is provenance text only; no solver is included or invoked by the analysis commands.

The path substitutions changed the bytes and SHA-256 values of affected provenance files, but did not change numerical results, statuses, runtimes, metrics, seeds, commands, or hashes of unchanged experimental inputs. `MANIFEST.sha256` records the sanitized package files rather than the historical package files.
