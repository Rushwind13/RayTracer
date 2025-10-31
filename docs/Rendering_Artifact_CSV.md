# Rendering Artifact CSV Specification

This document defines the canonical on-disk interchange format for rendering artifacts used in the stepwise pipeline.

## BDD rules

Goal: Lock down the canonical interleaved record format (Pixel + Intersection per record) and prove behavior with BDD tests for each actor. Keep data plumbing generic; minimize use of languages other than bash or C++; business logic stays in C++ actors; tests use bash for process handling only; rendering artifact data will be parsed via business logic always; perform validation in test harness (prefer a new test function written wholly in C++ to a new script); tests will minimize system() calls; minimize and reduce the usage of bash scripts; deprecate bash scripts as BDD tests replace them; deprecate use of Python scripts as BDD tests calling C++ functions replace them.

## Housekeeping rules

1) When data is captured, separate runtime logs from render artifacts; keep artifacts (*.txt, *.png) easy to compare/update; ensure logs are easy to clean.
2) Avoid re-running stages that already have a known-good truth txt.
3) Produce a PNG from each artifact txt (can run in parallel with other tasks; use stage_to_png.sh/Writer).
4) Avoid creating new scripts unless necessary; prefer reusing/consolidating existing ones and document reasons for any additions.
5) Maximize reuse of existing C++ code and scripts: cucumber tests for behavior, Writer for PNGs, PixelFactory for pixels, Feeder→Tool→Logger for stage/object runs.
6) Everything must be testable (unit or integration), with small deterministic scenes when feasible.
7) Goal: robust, repeatable, performant pipeline with minimal support overhead.

## Canonical format and invariants

- Record shape: exactly two lines per pixel record in this order:
  1) Pixel CSV line
  2) Intersection CSV line
- Blank payload rule: a blank second line is permitted and implies a default Intersection (gothit=false, oid=-1, distances large, vectors zero). Consumers must synthesize defaults, never carry a previous payload across records.
- EOF convention: a single EOF Pixel (type=iInvalid, x=y=-1) terminates streams/files; no extra blank lines after the final record.
- Counting invariants: for N pixels there are exactly 2N lines. Tools and tests count in records (pairs), not raw lines.

### Pixel CSV columns (in order)

These columns match `PrintPixel(std::ostream&, const Pixel&)` and must be parsed with `ReadPixel`.

1) x
2) y
3) oid
4-7) primaryRay.direction.x,y,z,w
8-11) primaryRay.origin.x,y,z,w
12) primaryRay.length
13) type (iPrimary=0, iShadow=1, iReflection=2, iRefraction=3; EOF=iInvalid=-1)
14-17) r.direction.x,y,z,w
18-21) r.origin.x,y,z,w
22) r.length
23-26) normal.x,y,z,w
27-30) position.x,y,z,w
31) distance
32) lid
33) NdotL
34) depth
35) weight
36-39) color.r,g,b,a
40) gothit

### Intersection CSV columns (in order)

These columns match `PrintIntersection(std::ostream&, const Intersection&)` and must be parsed with `ReadIntersection`.

1) gothit
2) anyhit
3) oid
4-7) normal.x,y,z,w
8-11) position.x,y,z,w
12) distance[0]
13) distance[1]

### Example (schematic)

Pixel: x,y,oid,Pr.dir(x,y,z,w),Pr.org(x,y,z,w),Pr.len,type,R.dir(x,y,z,w),R.org(x,y,z,w),R.len,Norm(x,y,z,w),Pos(x,y,z,w),Dist,lid,NdotL,depth,weight,Color(r,g,b,a),gothit

Intersection: gothit,anyhit,oid,Norm(x,y,z,w),Pos(x,y,z,w),Dist0,Dist1

## Producer/consumer contract

All actors below must treat the artifact format as the interface contract:

- PixelFactory
  - Emits Pixel lines paired with a blank Intersection line (default payload) unless explicitly provided.
  - Deterministic traversal (row-major as camera defines). Width/height come from Camera/World.

- Feeder
  - Reads records as pairs; blank or missing second line → synthesize default Intersection for that record.
  - FEEDER_LIMIT applies to records. Supports multiple input files as a single concatenated stream; emits exactly one EOF after the final record across all files.

- Logger
  - Captures a stream and writes records in canonical interleaved format. Supports `LOG_BASEDIR` and `LOG_APPEND`. Never mutates fields; logs counts for pixels/intersections.

- Writer
  - Consumes records; ignores the Intersection payload; advances one record (two lines) per pixel. Produces the final PNG at EOF.

## Compliance and validation

- Readers MUST use `ReadPixel`/`ReadIntersection` and default an `Intersection` when the payload line is blank/missing.
- Writers MUST use `PrintPixel`/`PrintIntersection` without reformatting or extra whitespace.
- EOF MUST be a single Pixel with `type=iInvalid` and `x=y=-1`, followed by no additional lines.
- Record counts MUST be computed in pairs; 2N-line invariant SHOULD be enforced in tests.

## BDD coverage

- pixel_factory.feature
  - Emits a 2N-line interleaved file for N pixels (tiny camera/slice). Verify first record fields; every Pixel is followed by a blank Intersection line.

- feeder.feature
  - Blank/missing Intersection lines produce default payload; payload state does not leak across records.
  - FEEDER_LIMIT obeys records; one EOF after the last record across multiple files.

- logger.feature
  - Writes canonical interleaved format; counts match; append concatenates whole records.

- writer.feature
  - Renders a PNG from a canonical artifact (with some blank payloads) via Feeder→Writer; no file edits required.

## Operational notes

- Prefer fixed ports from `scripts/tools/pipeline.cfg.sh` and ensure no bind conflicts; tear down old runs before ad-hoc tools.
- Keep `runs/complete` as canonical truth; regenerate only when inputs or code change.
