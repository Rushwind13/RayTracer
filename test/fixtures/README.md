sample_3_records.txt

- Provenance: copied from the first 3 records of data/pixels.txt at the time of creation.
- Format: canonical Rendering Artifact CSV — 2 lines per record (Pixel CSV, then blank Intersection line).
- Purpose: deterministic, tiny fixture for BDD tests (no external processes required).
- Refresh: if camera math changes, regenerate by copying the first 3 Pixel CSV lines from data/pixels.txt and ensure each is followed by a blank line.
