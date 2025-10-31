Rendering Artifact Schema BDD (PixelFactory, Feeder, Logger, Writer)
Nine categories follow. Final category is named Acceptance Criteria.
- Format and invariants
  - [ ] Define and document the canonical "Rendering Artifact CSV" format: exactly two lines per pixel, first line a Pixel CSV, second line an Intersection CSV.
  - [ ] Allow a blank Intersection line to mean "empty payload" (default Intersection: gothit=false, oid=-1, distances large, vectors zero).
  - [ ] EOF convention: a single EOF Pixel (type=iInvalid, x=y=-1) terminates streams; files should not contain trailing blank lines beyond interleaving.
  - [ ] Counting invariants: N pixels -> 2N lines; tools and tests compute counts by pairs, not lines.

- Testing and scaffolding
  - [ ] Avoid scripting in bash. Bash is to be used only for process orchestration.
  - [ ] No Python. No awk. No sed. This is a C++ application. Any data string parsing must happen with the ReadPixel / ReadIntersection functions.
  - [ ] Don't try to "get around" the language rules.
  - [ ] Reuse existing start.sh wrappers for all actors
  - [ ] Use ControlChannel proxy (XSUB/XPUB) where needed to rendezvous Feeder and Logger/Writer; fixed ports from pipeline.cfg.sh.
  - [ ] Optional helper: enhance PixelFactory to emit a single Pixel+Intersection pair (arbitrary <x,y> with correct Ray angles for complete image size and FOV) for micro-tests.
  - [ ] Optional helper: new pipeline actor, Echo, which copies "Rendering Artifact CSV" on the input, emitted unchanged on the output. Perform e.g. Feeder-to-Logger behavior, with fewer socket/binding modifications.
  - [ ] Provide a tiny canonical 3-record sample artifact (plus EOF) for fast BDD tests.


- BDD tests (style aligned with existing Matrix/Tuple/Ray/Sphere/Box features)
  - [ ] Create skeleton feature files and step definitions for all four actors: `pixel_factory.feature`, `feeder.feature`, `logger.feature`, `writer.feature` under `test/features/` with steps in `test/features/step_definitions/`.
  - pixel_factory.feature
    - [ ] Emits 2N-line interleaved file for N pixels (small camera or slice). Verify first Pixel line fields and that every Pixel line is followed by a blank Intersection line.
  - feeder.feature
    - [ ] Blank Intersection line produces default Intersection (gothit=0, oid=-1). Use a tiny 3-record sample file.
    - [ ] Respects FEEDER_LIMIT by records; EOF is published once after the last record.
  - logger.feature
    - [ ] Writes data in "Rendering Artifact CSV" format when capturing a stream with occasional missing payload; counts match records; append mode concatenates pairs.
  - writer.feature
    - [ ] Renders PNG from data in "Rendering Artifact CSV" format containing blank Intersection lines via Feeder→Writer; no file edits required.

- PixelFactory
  - [ ] Emits data in "Rendering Artifact CSV" format
  - [ ] Deterministic traversal (row-major: y fast/slow as defined); width/height from Camera.
  - [ ] BDD ensures data in "Rendering Artifact CSV" format for truth generation (e.g., data/pixels.txt is 2N lines).
  - [ ] Use PrintPixel/PrintIntersection exclusively for output; no ad-hoc formatting or extra whitespace.
  - [ ] When no Intersection payload is provided, emit a blank second line; terminate with a single EOF Pixel (type=iInvalid, x=y=-1) and no trailing lines.

- Feeder
  - [ ] Reads records in "Rendering Artifact CSV" format; when the second line is blank or missing, synthesize a default Intersection for that record.
  - [ ] Do not carry Intersection payload across records (bug fix): reset Intersection each record to avoid reusing previous payload on blank lines.
  - [ ] FEEDER_LIMIT limits by records (pixels), not lines; emits one EOF after the final record across all input files.
  - [ ] Supports multiple input files; treats them as a single concatenated stream.
  - [ ] Use ReadPixel/ReadIntersection exclusively for input; no bespoke parsing in bash/sed/awk/python.
  - [ ] Recognize EOF via the single EOF Pixel sentinel and avoid emitting or consuming any additional lines after EOF.

- Logger
  - [ ] Emits data in "Rendering Artifact CSV" format for stepwise outputs: write a Pixel line, when the second line is blank or missing, synthesize a default Intersection for that record.
  - [ ] Supports LOG_BASEDIR and LOG_APPEND; prints counts for pixels and intersections; never transforms business fields.
  - [ ] Use PrintPixel/PrintIntersection exclusively; append mode concatenates complete Pixel+Intersection pairs; emit exactly one EOF at the end of a capture.

- Writer
  - [ ] Consumes data in "Rendering Artifact CSV" format; Ignores Intersection payload;strides two lines per pixel.
  - [ ] PNG generation can render from any Rendering Artifact CSV via Feeder→PNG bus without editing.
  - [ ] flushes buffers appropriately; every PNG is complete, no leftover pixels; if final batch size is <1000 pixels, all pixels are flushed to output at EOF.
  - [ ] Writing multiple files in a row (using same ControlChannel and same topic names) does not spill data between files.
  - [ ] Use ReadPixel/ReadIntersection exclusively; stop processing on encountering the EOF Pixel sentinel (type=iInvalid, x=y=-1).

- Migration/consistency
  - [ ] Ensure data/pixels.txt is data in "Rendering Artifact CSV" format.
  - [ ] Keep `runs/complete` artifacts as the canonical data in "Rendering Artifact CSV" format; 
  - [ ] all tools (to_opng, stage_to_png) understand data in "Rendering Artifact CSV" format and parse it using ReadPixel/ReadIntersection.
  - [ ] use `all_down.sh` to close running pipeline before all tests.

- Acceptance Criteria
  - [ ] All four BDD features pass locally and in CI.
  - [ ] 150x2 cross-section (pixel rows 49–50) passes end-to-end from interleaved slice to PNG without editing artifacts.