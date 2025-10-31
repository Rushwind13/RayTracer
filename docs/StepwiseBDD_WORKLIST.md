# Stepwise BDD Worklist

This document aggregates all "stepwise" work items (verbatim) and enumerates the integration tests we need to exercise each pipeline stage on a tiny deterministic dataset, producing correct artifacts per the Rendering Artifact CSV ruleset.


### Truth data and artifact comparisons (stepwise)

- [ ] Regenerate a full set of stepwise artifacts from data/pixels.txt (truth run)

## Integration tests to implement (following ArtifactCSVSteps.cpp patterns)

Design: Each test runs the minimal actor set for that stage, driven by Feeder (or PixelFactory for Stage 1), and asserts canonical 2N-line artifacts (or a PNG) using Logger/Writer. Use a tiny dataset (e.g., 2×1 or 3 records) from runs/complete or fixtures.

### Inputs and outputs per stage (at a glance)

This aligns with your artifact sequence: pixels → IntersectWith.per-obj → IntersectResults → Shader → Color → Depth → PNG → (png file).

- Stage 1 — PixelFactory
  - Consumes: World/Camera config (e.g., `bin/World.json`), no prior artifact
  - Produces: `pixels.txt` (2N lines: Pixel + blank Intersection per record)

- Stage 2 — IntersectWith (per-object)
  - Consumes: `pixels.txt`
  - Produces: `oIntersectWith.<oid>.txt` per world object (RESULT per object)

- Stage 3 — IntersectResults (aggregate + route)
  - Consumes: all `oIntersectWith.<oid>.txt` (concatenated stream, one EOF overall)
  - Produces: `oShader.txt`, `oBKG.txt`, `oBLACK.txt`, `oLIT.txt` (aggregated RESULT per pixel) and routes to SHADE/BKG/BLACK/LIT topics

- Stage 4 — Shader/Background/Black/Lit (producers of COLOR)
  - Consumes: `oShader.txt`, `oBKG.txt`, `oBLACK.txt`, `oLIT.txt`
  - Produces: `oCOLOR.txt` (COLOR contributions from all four paths)

- Stage 5 — ColorResults
  - Consumes: `oCOLOR.txt`
  - Produces: `oDEPTH.txt` (per-pixel layer weights/depth accumulation)

- Stage 6 — DepthChart
  - Consumes: `oDEPTH.txt`
  - Produces: `oPNG.txt` (PNG stream records)

- Stage 7 — Writer
  - Consumes: `oPNG.txt`
  - Produces: final PNG file (path provided to Writer, e.g., `runs/tmp/test.png`)

### Optional design: split IntersectResults into Fan-in + Mailroom (divide-out)

Rationale: today, `IntersectResults` both aggregates per-pixel results (fan-in) and routes to four topics (divide-out). Splitting these concerns can improve testability, observability, and reproducibility:

- Fan-in (IntersectResults-only)
  - Behavior: wait for `world.object_count` per pixel, choose nearest (primary/reflection) or first blocker (shadow), then publish a single aggregated message per pixel on a new topic, e.g., `IntersectResults`.
  - Artifact: `oIntersectResults.txt` via a Logger subscribing `IntersectResults`.

- Mailroom (new actor)
  - Behavior: subscribe to `IntersectResults`, apply the routing table, and publish to `Shader`/`Background`/`Black`/`Lit` topics. Optionally run separate mailrooms (e.g., shadow vs regular) for isolation.
  - Artifacts: `oShader.txt`, `oBKG.txt`, `oBLACK.txt`, `oLIT.txt` via Loggers.

Contract sketch (C++)
- IntersectResults (fan-in): after `testComplete` in `local_work`, pack `(Pixel, Intersection)` and `sendMessage(header, payload, "IntersectResults")`; on EOF, also send EOF to `IntersectResults`.
- Mailroom: a simple `Widget` that `SUB`scribes `IntersectResults`, then `sendMessage(...)` to one of `"Shader"|"Background"|"Black"|"Lit"` per rules used in `local_send` today.

Alt stepwise (fan-in → mailroom)
- Stage 3a — IntersectResults (fan-in only)
  - Consumes: per-object `oIntersectWith.<oid>.txt`
  - Produces: `oIntersectResults.txt` (single aggregated stream)
- Stage 3b — Mailroom (divide-out)
  - Consumes: `oIntersectResults.txt`
  - Produces: `oShader.txt`, `oBKG.txt`, `oBLACK.txt`, `oLIT.txt`

BDD additions
- mailroom.feature
  - Given `oIntersectResults.txt` with mixed hits/misses and shadow cases
  - When I run Mailroom
  - Then records route to the correct topics; each output obeys the 2N-line invariant; EOF fans out once.

- Stage 1 — PixelFactory → Logger
  - Given PixelFactory streaming N records on socket tcp://127.0.0.1:1313 channel INTERSECT
  - And Logger on socket tcp://127.0.0.1:1313 subscribing INTERSECT writing label Pixels
  - When I capture the stream
  - Then the logger artifact "Pixels.txt" should have N records
  - And the logger artifact "Pixels.txt" should have N blank payload lines

- Stage 2 — Feeder → IntersectWith (per-object) → Logger(objects)
  - Inputs: pixels.txt (or a small slice built by the BDD step "a 150x2 slice artifact")
  - Start one IntersectWith process per world object, each subscribing INTERSECT and publishing RESULT.
  - Start Loggers subscribing RESULT with distinct labels per object, e.g., IntersectWith.<oid>.
  - When I feed K records from the input artifact to channel INTERSECT on the chosen socket
  - Then each captured object artifact (e.g., oIntersectWith.<oid>.txt) has K records, is 2N lines, and contains consistent oid values.

- Stage 3 — Feeder → IntersectResults (aggregate + route) → Logger(SHADE/BKG/BLACK/LIT)
  - Inputs: runs/complete/objects/oIntersectWith.*.txt (concatenated per-object RESULT streams)
  - Start IntersectResults bound on RESULT in; it aggregates per-pixel across objects and routes by test result.
  - Start four Loggers on BUS_SHADER tcp://127.0.0.1:1312 subscribing SHADE, BKG, BLACK, LIT with labels Shader, BKG, BLACK, LIT
  - When I feed all per-object artifacts via Feeder to channel RESULT (one EOF after the final file)
  - Then oShader.txt, oBKG.txt, oBLACK.txt, and oLIT.txt exist; each has 2N lines; and K_bkg + K_shader equals the number of primary pixels (sanity check).

- Stage 4 — Feeder → Shader/Background/Black/Lit → Logger(Color)
  - Inputs: oShader.txt, oBKG.txt, oBLACK.txt, oLIT.txt
  - Start Shader bound on SHADE in; Background on BKG in; Black on BLACK in; Lit on LIT in; all publish COLOR to tcp://127.0.0.1:1303
  - Start Logger on BUS_COLOR tcp://127.0.0.1:1303 subscribing COLOR writing label COLOR
  - Feed each input file to its matching channel via Feeder; ensure one EOF total at end
  - Assert oCOLOR.txt has expected count and obeys the 2N-line invariant

- Stage 5 — Feeder → ColorResults → Logger(DEPTH)
  - Input: oCOLOR.txt
  - Start ColorResults bound on COLOR in; publishes DEPTH to tcp://127.0.0.1:1307
  - Start Logger subscribing DEPTH with label DEPTH
  - Feed N records; assert oDEPTH.txt has N records and is 2N lines

- Stage 6 — Feeder → DepthChart → Logger(PNG stream)
  - Input: oDEPTH.txt
  - Start DepthChart bound on DEPTH in; publishes PNG to tcp://127.0.0.1:1308
  - Start Logger subscribing PNG with label PNG
  - Feed N records; assert oPNG.txt has N records and is 2N lines

- Stage 7 — Feeder → Writer → PNG file
  - Input: oPNG.txt
  - Start Writer bound on BUS_PNG tcp://127.0.0.1:1308 output runs/tmp/test.png
  - Feed N records via Feeder to channel PNG
  - Assert PNG "runs/tmp/test.png" should exist

- Cross-cutting tests
  - EOF propagation per stage: exactly one EOF Pixel as control signal; never captured in artifacts; consumers terminate and flush.
  - Blank payload defaulting: when an input record has a blank payload line, the consumer synthesizes a default Intersection and still emits Pixel+Intersection.
  - Counting invariants: for N records, all artifacts have exactly 2N lines; tests count in records, not lines.

### New item

- [ ] Able to push an image of arbitrary HxW size through the pipeline using the integration test harness (parameterize Feeder limits and verify final PNG dimensions)

## Ruleset anchors (from docs/Rendering_Artifact_CSV.md)

- Interleaved 2N-line invariant: Pixel CSV line followed by Intersection CSV (blank allowed → default Intersection).
- EOF is a single control Pixel (type=iInvalid, x=y=-1) on the bus; never captured in artifacts.
- All actors except PixelFactory must emit both Pixel and Intersection in their bus messages; when missing on input, synthesize a default payload.
- Disk artifacts MUST be written with PrintPixel/PrintIntersection and read with ReadPixel/ReadIntersection; no extra whitespace.

