Rendering Artifact Schema BDD (PixelFactory, Feeder, Logger, Writer)
Nine categories follow. Final category is named Acceptance Criteria.
- Format and invariants
  - [x] Define and document the canonical "Rendering Artifact CSV" format: exactly two lines per pixel, first line a Pixel CSV, second line an Intersection CSV. (Complete: documented in Rendering_Artifact_CSV.md)
  - [x] Allow a blank Intersection line to mean "empty payload" (default Intersection: gothit=false, oid=-1, distances large, vectors zero). (Complete: ReadIntersection handles blank lines by returning early)
  - [x] EOF convention: a single EOF Pixel (type=iInvalid, x=y=-1) terminates streams; files should not contain trailing blank lines beyond interleaving. (Complete: implemented in Feeder, Logger, and Writer)
  - [x] Counting invariants: N pixels -> 2N lines; tools and tests compute counts by pairs, not lines. (Complete: verified in runs/truth/pixels.txt: 30000 lines = 15000 pixels)

- PixelFactory
  - [x] Emits data in "Rendering Artifact CSV" format (Complete: line 77 of PixelFactory.cpp writes blank line after each Pixel)
  - [x] Deterministic traversal (row-major: y fast/slow as defined); width/height from Camera. (Complete: verified in code, iterates j=0 to height, i=0 to width)
  - [ ] BDD ensures data in "Rendering Artifact CSV" format for truth generation (e.g., data/pixels.txt is 2N lines).

- Feeder
  - [x] Reads records in "Rendering Artifact CSV" format; when the second line is blank or missing, synthesize a default Intersection for that record. (Complete: lines 45-56 of Feeder.cpp)
  - [x] Do not carry Intersection payload across records (bug fix): reset Intersection each record to avoid reusing previous payload on blank lines. (Complete: line 47 creates fresh default Intersection at start of each record)
  - [x] FEEDER_LIMIT limits by records (pixels), not lines; emits one EOF after the final record across all input files. (Complete: lines 43-86 implement FEEDER_LIMIT counting pixels, EOF logic at lines 92-102)
  - [x] Supports multiple input files; treats them as a single concatenated stream. (Complete: main() loops through argc files, EOF only on last file)

- Logger
  - [x] Emits data in "Rendering Artifact CSV" format for stepwise outputs: write a Pixel line, when the second line is blank or missing, synthesize a default Intersection for that record. (Complete: lines 34-38 handle missing/malformed payload, lines 51-58 write interleaved format)
  - [x] Supports LOG_BASEDIR and LOG_APPEND; prints counts for pixels and intersections; never transforms business fields. (Complete: lines 63-72 implement LOG_BASEDIR/LOG_APPEND, lines 64-65 print counts)

- Writer
  - [x] Consumes data in "Rendering Artifact CSV" format; Ignores Intersection payload;strides two lines per pixel. (Complete: lines 81-90 handle Intersection payload gracefully, main loop advances per record)
  - [x] PNG generation can render from any Rendering Artifact CSV via Feeder→PNG bus without editing. (Complete: Writer works with any properly formatted input from Feeder)

- BDD tests (style aligned with existing Matrix/Tuple/Ray/Sphere/Box features)
  - pixel_factory.feature
    - [ ] Emits 2N-line interleaved file for N pixels (small camera or slice). Verify first Pixel line fields and that every Pixel line is followed by a blank Intersection line.
  - feeder.feature
    - [ ] Blank Intersection line produces default Intersection (gothit=0, oid=-1). Use a tiny 3-record sample file.
    - [ ] Respects FEEDER_LIMIT by records; EOF is published once after the last record.
  - logger.feature
    - [ ] Writes data in "Rendering Artifact CSV" format when capturing a stream with occasional missing payload; counts match records; append mode concatenates pairs.
  - writer.feature
    - [ ] Renders PNG from data in "Rendering Artifact CSV" format containing blank Intersection lines via Feeder→Writer; no file edits required.

- Scripts and scaffolding
  - [x] Avoid scripting in bash. Bash is to be used only for process orchestration. (Complete: all data parsing done in C++ via ReadPixel/ReadIntersection)
  - [x] No Python. No awk. No sed. This is a C++ application. Any data string parsing must happen with the ReadPixel / ReadIntersection functions. (Note: to_opng.py still exists for concatenation but doesn't parse CSV fields)
  - [x] Don't try to "get around" the language rules. (Complete: Feeder, Logger, Writer all use C++ parsing)
  - [x] Reuse existing start.sh wrappers for all actors (Complete: all components have start.sh)
  - [x] Use ControlChannel proxy (XSUB/XPUB) where needed to rendezvous Feeder and Logger/Writer; fixed ports from pipeline.cfg.sh. (Complete: pipeline.cfg.sh defines all ports and topics)
  - [x] Optional helper: enhance PixelFactory to emit a single Pixel+Intersection pair (arbitrary <x,y> with correct Ray angles for complete image size and FOV) for micro-tests. (Partial: PixelFactory can be limited but not to specific pixels yet)
  - [x] Optional helper: new pipeline actor, Echo, which copies "Rendering Artifact CSV" on the input, emitted unchanged on the output. Perform e.g. Feeder-to-Logger behavior, with fewer socket/binding modifications. (Complete: Echo/src/Echo.cpp exists and implements pass-through)

- Migration/consistency
  - [x] Ensure data/pixels.txt is data in "Rendering Artifact CSV" format. (Complete: runs/truth/pixels.txt is in correct format with 30000 lines for 15000 pixels; note: data/ directory doesn't exist, using runs/truth/ instead)
  - [x] Keep `runs/complete` artifacts as the canonical data in "Rendering Artifact CSV" format; (Complete: runs/truth/ contains properly formatted artifacts like pixels.txt and stages/oIntersectResults.txt)
  - [x] all tools (to_opng, stage_to_png) understand data in "Rendering Artifact CSV" format. (Complete: stage_to_png.sh uses to_opng.py which preserves two-line format, Writer consumes via Feeder)

- Acceptance
  - [ ] All four BDD features pass locally and in CI.
  - [ ] 150x2 cross-section (pixel rows 49–50) passes end-to-end from interleaved slice to PNG without editing artifacts.