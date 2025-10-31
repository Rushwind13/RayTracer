Rendering Artifact Schema BDD (PixelFactory, Feeder, Logger, Writer)
Several categories follow. Final category is named Acceptance Criteria. In your output, List the names (only) of all categories.

- Message vs Disk contract

  - Bus: One logical message consists of a Pixel header plus an Intersection payload. PixelFactory is the sole exception and may emit Pixel-only messages; all other actors MUST emit both a Pixel and an Intersection payload. EOF is a single control Pixel (type=iInvalid, x=y=-1) on the bus only.
  - Disk: Canonical artifacts MUST contain exactly two lines per record (2N total): Pixel CSV line followed by the Intersection CSV line. When payload is absent, the second line MUST be present but blank. EOF MUST NOT be written to artifacts.
  - Do not consider work items complete until full pipeline integration behavior is demonstrated for all use cases.

- Format and invariants
  - [ ] Define and document the canonical "Rendering Artifact CSV" format: exactly two lines per pixel, first line a Pixel CSV, second line an Intersection CSV.
  - [ ] Allow a blank Intersection line to mean "empty payload" (default Intersection: gothit=false, oid=-1, distances large, vectors zero).
  - [ ] EOF convention: a single EOF Pixel (type=iInvalid, x=y=-1) terminates streams on the bus; EOF MUST NOT be written to disk artifacts; files must contain exactly 2N lines (no extra or trailing blank lines).
  - [ ] Counting invariants: N pixels -> 2N lines; tools and tests compute counts by pairs, not lines.
  - [ ] Writers MUST include a blank Intersection line when payload is absent (do not omit the line). Readers MAY treat a missing second line as blank for robustness, but omission is non-compliant for writers.

- Testing and scaffolding
  - [ ] Avoid scripting in bash. Bash is to be used only for process orchestration.
  - [ ] No Python. No awk. No sed. This is a C++ application. Any data string parsing must happen with the ReadPixel / ReadIntersection functions.
  - [ ] Don't try to "get around" the language rules.
  - [ ] Reuse existing start.sh wrappers for all actors
  - [ ] Use ControlChannel proxy (XSUB/XPUB) where needed to rendezvous Feeder and Logger/Writer; fixed ports from pipeline.cfg.sh.
  - [ ] Optional helper: enhance PixelFactory to emit a single Pixel+Intersection pair (arbitrary <x,y> with correct Ray angles for complete image size and FOV) for micro-tests.
  - [ ] Optional helper: new pipeline actor, Echo, which copies "Rendering Artifact CSV" on the input, emitted unchanged on the output. Perform e.g. Feeder-to-Logger behavior, with fewer socket/binding modifications.
  - [ ] Provide a tiny canonical 3-record sample artifact file (no EOF in file). For bus-oriented tests, append a single EOF Pixel to the message stream (not to the file).

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
  - [ ] Bus: emits Pixel-only messages and MUST emit a single EOF Pixel after the last pixel.
  - [ ] Deterministic traversal; x fast / y slow; width/height from Camera.
  - [ ] BDD ensures data in "Rendering Artifact CSV" format for truth generation (e.g., data/pixels.txt is 2N lines).
  - [ ] Use PrintPixel for Pixel lines. For records without an Intersection payload, write an empty (blank) Intersection line to preserve the two-line record, not a synthesized numeric Intersection line.
  - [ ] Disk: when writing artifacts directly, MUST write a Pixel CSV line followed by a blank Intersection line per record (to preserve the 2N-line invariant). Do NOT write EOF to disk artifacts.

- Feeder
  - [ ] Reads records in "Rendering Artifact CSV" format; when the second line is blank or missing, synthesize a default Intersection for that record.
  - [ ] Emits bus messages that ALWAYS contain both a Pixel and an Intersection payload (synthesized default if input payload is blank or missing).
  - [ ] Do not carry Intersection payload across records (bug fix): reset Intersection each record to avoid reusing previous payload on blank lines.
  - [ ] FEEDER_LIMIT limits by records (pixels), not lines; emits one EOF after the final record across all input files.
  - [ ] Supports multiple input files; treats them as a single concatenated stream.
  - [ ] Use ReadPixel/ReadIntersection exclusively for input; no bespoke parsing in bash/sed/awk/python.
  - [ ] Recognize EOF via the single EOF Pixel sentinel and avoid emitting or consuming any additional lines after EOF.

- Logger
 - [ ] Writes canonical interleaved (2N-line) files; if an input message lacks an Intersection payload, write a blank Intersection line for that record. Logger does NOT write EOF to disk. When acting as a pass-through on the bus, forward exactly one EOF after the final record.
  - [ ] Supports LOG_BASEDIR and LOG_APPEND; prints counts for pixels and intersections; never transforms business fields.
  - [ ] Use PrintPixel/PrintIntersection exclusively; append mode concatenates complete Pixel+Intersection pairs; emit exactly one EOF at the end of a capture.
  

- Writer
  - [ ] Consumes bus messages (Pixel + Intersection); prefers pixel Object ID; only uses Intersection Object ID if pixel is malformed; ignores Intersection payload otherwise; Writer does not parse text artifacts directly.
  - [ ] For file-to-PNG flows, use Feeder→Writer: Feeder reads pairs from disk and emits bus messages to Writer. Writer stops on the EOF Pixel received on the bus.
  - [ ] flushes buffers appropriately; every PNG is complete, no leftover pixels; if final batch size is <1000 pixels, all pixels are flushed to output at EOF.
  - [ ] Writing multiple files in a row (using same ControlChannel and same topic names) does not spill data between files.

- Migration/consistency
  - [ ] Ensure data/pixels.txt is data in "Rendering Artifact CSV" format.
  - [ ] Keep `runs/complete` artifacts as the canonical data in "Rendering Artifact CSV" format; 
  - [ ] Tools like to_png and stage_to_png operate via Feeder→Writer: Feeder parses the artifact file using ReadPixel/ReadIntersection and emits messages to Writer; Writer does not parse files directly.
  - [ ] use `all_down.sh` to close running pipeline before all tests.

- Acceptance Criteria
  - [ ] All four BDD features pass locally and in CI.
  - [ ] 150x2 cross-section (pixel rows 49–50) passes end-to-end from interleaved slice to PNG without editing artifacts.