# Distributed Ray Tracer — Worklist and Component Map (2025-10-27)
## Scope
- Goal: Each rendering pipeline stage runs as its own process (ideally on its own CPU core) and is wired via ZeroMQ through the Widget interface.
- Repos in workspace: RayTracer (engine + pipeline stages) and zmq_widgets (ZeroMQ wrapper + broker).

## Architecture
### Pipeline components (files, purpose, I/O)
- PixelFactory (RayTracer/PixelFactory/src)
  - Purpose: Load camera, generate one primary ray per pixel. In stepwise mode writes output.txt; in streaming mode publishes Pixel headers to INTERSECT.
  - Input: none. Output: Pixel headers (and sometimes blank payload) on INTERSECT; in stepwise mode CSV lines to file.
  - Next steps: unify file emit format with Feeder; add CLI to choose streaming vs stepwise; emit EOF control pixel when done.

- Feeder (RayTracer/Feeder/src)
  - Purpose: Stepwise feeder; reads Pixel + Intersection pairs from files (CSV via Print/Read helpers) and publishes to downstream.
  - Input: file list; Output: INTERSECT. Handles EOF emission for last file.
  - Next steps: standardize file naming; integrate with run.csv; add validation and backpressure/sleep tuning elimination.

- IntersectWith (RayTracer/IntersectWith/src)
  - Purpose: Per-object intersection test. For shadow tests, uses "anyhit within distance" rule. Packs Intersection into payload.
  - Input: Pixel header (+ optional Intersection prep for shadow distance). Output: RESULT to IntersectResults.
  - Next steps: remove assert spam; make DEBUG controllable; ensure no self-shadow and proper epsilon offsets; add support for reflection/refraction rays.

- IntersectResults (RayTracer/IntersectResults/src)
  - Purpose: Aggregate intersections across all objects. Choose nearest for primary; for shadow, first blocker. Routes to next stage topic.
  - Input: RESULT; Output: SHADE/BKG or BLACK/LIT depending on Pixel.type and Intersection result.
  - Next steps: fix noted bug on shadow distance check; parameterize world.object_count; ensure consistent EOF propagation.

- Mailroom (RayTracer/Mailroom/src)
  - Purpose: Divide-out router. Subscribes to aggregated IntersectResults stream and republishes to Shader/Background/Black/Lit topics.
  - Input: IntersectResults (aggregate); Output: SHADE/BKG/BLACK/LIT.
  - Next steps: finalize topic and socket defaults in pipeline.cfg; add performance metrics and optional split mailrooms (shadow vs regular).

- Shader (RayTracer/Shader/src)
  - Purpose: For primary hits, compute ambient/emissive baseline and launch one shadow test per light. Stashes N·L, light id, distance.
  - Input: SHADE + payload=nearest Intersection; Output: INTERSECT for shadow rays; also prepares baseline COLOR for ambient/emissive.
  - Next steps: actually publish the baseline ambient/emissive to ColorResults; support refraction; move hardcoded constants to World.

- Black (RayTracer/Black/src)
  - Purpose: Shadowed point contribution (ambient + emissive only). Input: BLACK; Output: COLOR.
  - Next steps: world-driven ambient; add tests for ambient-only path.

- Lit (RayTracer/Lit/src)
  - Purpose: Unshadowed point contribution for a specific light. Computes diffuse and specular (Blinn-Phong). Input: LIT; Output: COLOR.
  - Next steps: energy-conserving mix with material.specular/diffuse; optional attenuation; support colored lights; tests for specular extremes.

- Background (RayTracer/Background/src)
  - Purpose: Sky/background color with horizon fade. Input: BKG; Output: COLOR.
  - Next steps: parameterize gradient in World.json; handle up-vector/horizon.

- ColorResults (RayTracer/ColorResults/src)
  - Purpose: Accumulate COLOR contributions (ambient/emissive + one per light). Completes when 1 + light_count received.
  - Input: COLOR; Output: DEPTH when complete.
  - Next steps: incorporate multiple recursion depths; avoid double-counting; unit tests for accumulation logic.

- Reflection (RayTracer/Reflection/src)
  - Purpose: When object is reflective, compute reflected ray, bump depth, weight *= reflectivity*0.5, and send INTERSECT.
  - Input: SHADE (+ payload=Intersection); Output: INTERSECT.
  - Next steps: integrate into graph (start script exists); coordinate with DepthChart; guard against runaway recursion.

- DepthChart (RayTracer/DepthChart/src)
  - Purpose: Merge primary and reflective layers by weight per pixel. Completes when all expected layers seen.
  - Input: DEPTH; Output: PNG.
  - Next steps: tie expectations to world.maxdepth and reflectivity; fix noted TODO for completion condition; support refraction layer.

- Writer (RayTracer/Writer/src)
  - Purpose: Write final image to PNG using pngwriter. Maintains in-memory image buffer.
  - Input: PNG; Output: world.filename PNG file.
  - Next steps: eliminate periodic autosave unless requested; configurable output path; checksum/log final stats.

- Configurator (RayTracer/Configurator/src)
  - Purpose: Sends EOF Pixel to kick downstream shutdown. Useful for stepwise runs.
  - Next steps: broaden to config broadcast (e.g., world, camera, ports) prior to runs.

- Widget library (zmq_widgets/Widget)
  - Purpose: Base class encapsulating ZMQ PUB/SUB and msgpack serialization; virtual local_setup/local_work/local_shutdown.
  - API: run() → setup() → loop() until running=false → shutdown(). Helpers for send/recv and unpacking.
  - Next steps: explicit backpressure or watermark settings; robust error handling; metrics hooks.

- ControlChannel broker (zmq_widgets/ControlChannel)
  - Purpose: ZMQ SUB→PUB proxy; example binds: in tcp://127.0.0.1:1313, out tcp://127.0.0.1:1300.
  - Caveat: README notes CPU-heavy and suggests moving to direct app-to-app channels; scripts currently also use 1301/1312/1303/1307/1308.
  - Next steps: define a minimal set of proxies or switch to ROUTER/DEALER or per-edge topics to avoid global bottleneck.

### High-level data flow (streaming mode, over TCP sockets)
- PixelFactory → INTERSECT → IntersectWith (many processes, one per world object)
- IntersectWith → RESULT → IntersectResults (aggregates per pixel across objects)
- IntersectResults routes by test result:
  - Primary miss → BKG → Background
  - Primary hit  → SHADE → Shader
  - Shadow hit   → BLACK → Black
  - Shadow miss  → LIT → Lit
- Background/Black/Lit → COLOR → ColorResults (accumulates per-pixel color)
- Shader (on hit) spawns shadow tests → INTERSECT → IntersectWith
- Reflection (on reflective hits) computes reflected ray → INTERSECT → IntersectWith
- ColorResults → DEPTH → DepthChart (merge primary + reflected layers)
- DepthChart → PNG → Writer (final PNG)
- Configurator can broadcast EOF control pixel to orchestrate shutdown.
- ControlChannel proxy can be used to bridge channels or fan out, but is a bottleneck if used globally.

### Network topics and sockets (from start.sh; see scripts/tools/pipeline.cfg.sh)
- PixelFactory: PUB to INTERSECT at tcp://127.0.0.1:1313
- IntersectWith: SUB INTERSECT tcp://127.0.0.1:1313, PUB INTERSECT tcp://127.0.0.1:1300 (per-object process)
- IntersectResults: SUB RESULT tcp://127.0.0.1:1301, PUB SHADE tcp://127.0.0.1:1312 (also sends to BLACK/LIT/BKG)
- Shader: SUB SHADE tcp://127.0.0.1:1312, PUB COLOR tcp://127.0.0.1:1303 (also emits INTERSECT for shadow tests)
- Black: SUB BLACK tcp://127.0.0.1:1312, PUB COLOR tcp://127.0.0.1:1303
- Lit: SUB LIT tcp://127.0.0.1:1312, PUB COLOR tcp://127.0.0.1:1303
- Background: SUB BKG tcp://127.0.0.1:1312, PUB COLOR tcp://127.0.0.1:1303
- ColorResults: SUB COLOR tcp://127.0.0.1:1303, PUB DEPTH tcp://127.0.0.1:1307
- Reflection: SUB SHADE tcp://127.0.0.1:1312, PUB INTERSECT tcp://127.0.0.1:1313
- DepthChart: SUB DEPTH tcp://127.0.0.1:1307, PUB PNG tcp://127.0.0.1:1308
- Writer: SUB PNG tcp://127.0.0.1:1308
- ControlChannel (zmq_widgets): SUB binds at 1313, PUB binds at 1300 (one example proxy). Note: current scripts mix ports 1300/1301; ensure required proxy(ies) are started or align sockets.

#### Ports and configuration policy (2025-10-29)
- All dynamic port scanning has been removed from active tools; fixed ports are defined in `scripts/tools/pipeline.cfg.sh`.
  - BUS_PNG: tcp://127.0.0.1:1308 — used by Writer and all PNG-related tools (stage_to_png, count_png_colors).
  - BUS_SHADER: tcp://127.0.0.1:1312 — used by IntersectResults aggregation and Shader subscribers.
- `stage_to_png.sh` now passes an absolute output path to Writer to ensure files land in the requested directory.
- Operational note: Always tear down old runs (`scripts/pipeline/all_down.sh`) before ad‑hoc tools to avoid bind conflicts on fixed ports.

### Math, scene, and rendering core (files, synopsis)
- Math.hpp: basic types (Position, Direction, Color, Range), constants (epsilon, deg2rad), linear algebra helpers, TransformRay, ReflectVector, RefractVector, matrix builders (Translate/Scale/Rotate/Shear), lerp.
- Object.hpp: materials, procedural patterns (Solid/Stripe/Gradient/Ring/Perlin-based), primitives (Sphere/Plane/Box) with intersection math; Light type.
- Camera.hpp: camera model; loads from bin/World.json; constructs rays via RayThroughPoint; builds camera frame.
- Lighting.hpp: helpers for light vectors and shading pseudocode references; Shadow(r) stub uses Intersection class.
- Intersection.hpp: Intersection data structure with msgpack, CSV Print/Read helpers.
- Raytracer2014.hpp: legacy prototypes and historical Python notes.

### Tests (RayTracer/test)
- Frameworks: GTest + cucumber-cpp; feature files for tuples, rays, shapes, materials, boxes, patterns.
- Step definitions exercise: vector/point ops, normalization, dot/cross, reflect vector, transforms, normals, box intersection, materials.
- Gaps: no integration tests for the ZMQ pipeline, routing, or end-to-end image; no tests for stepwise file I/O format; no tests for per-stage EOF handling.

### GitHub status (reachable via MCP)
- Owner: Rushwind13
- RayTracer: 0 open issues; 1 open PR (#18 “Feat/stepwise” — converts pipeline to stepwise modules reading/writing text files).
- zmq_widgets: 0 open issues; 0 open PRs.

## Scripts inventory
### Pipeline
  - scripts/pipeline/all_up.sh — Start full pipeline with proxy; quick smoke to Writer; tears down on success.
  - scripts/pipeline/all_down.sh — Robust teardown: kills known binaries, start.sh wrappers, and frees ports.
  - scripts/pipeline/all_up_stepwise.sh — Run pipeline in stepwise mode stage-by-stage, logging to runs/<timestamp>.
  - scripts/pipeline/only_one_up.sh — Run one stage in isolation (Stage 4/5/6 supported) from a given input o*.txt.
  - scripts/pipeline/run_objects_serial.sh — Per-object IntersectWith capture, then run downstream to frames; can render per-object PNGs.
  - scripts/pipeline/produce_intersectresults.sh — Aggregate truth per-object artifacts into oIntersectResults.txt and render intersectresults.png (fixed BUS_SHADER).
  - scripts/pipeline/regenerate_truth.sh — Deterministic truth run across stages (1→6), capture all o*.txt and final PNG.
### Artifacts
  - scripts/artifacts/capture_objects.sh — Re-capture specified objects’ oIntersectResult.<obj>.txt into runs/truth/objects.
  - scripts/artifacts/freeze_truth.sh — Freeze latest per-object run into runs/truth and compare to runs/complete.
  - scripts/artifacts/freeze_truth_pixels.sh — Freeze camera pixel reference to truth.
  - scripts/artifacts/regenerate_ref_pixels.sh — Rebuild PixelFactory pixels to data/pixels.txt with current camera.
  - scripts/tools/stage_to_png.sh — Convert any Logger-formatted artifact(s) to PNG via Feeder→Writer (dynamic ports; waits for flush).
  - scripts/tools/stage_to_png.sh — Convert any Logger-formatted artifact(s) to PNG via Feeder→Writer (fixed BUS_PNG; absolute output path; deterministic).
  - scripts/artifacts/rerender_from_artifacts.sh — Re-render PNGs from existing artifacts without recomputation.
  - scripts/artifacts/render_frames.sh — Render PNGs per stage artifact using Writer.
### Validation
  - scripts/validation/count_png_colors.sh — Approximate distinct colors by Writer [STATS] with safe fallback (fixed BUS_PNG).
  - scripts/validation/pretty_logs.sh — Summarize run directory: list artifacts and tail relevant logs.
  - scripts/validation/compare_artifacts.sh — Compare a run directory’s o*.txt vs runs/complete.
  - scripts/validation/trace_missing_pixels.py — Pixel-by-pixel presence diagnostics.
  - scripts/validation/analyze_intersections.py — Count hits/misses and distances within artifacts.
  - scripts/validation/analyze_png_log.py — Analyze oPNG.txt coverage and estimate missing pixels.
  - scripts/validation/surface_intersections.sh — Two-ray per-object intersection probe for quick sanity checks.
### Tools
  - scripts/tools/to_opng.py — Concatenate Logger artifacts into oPNG-shaped stream for Writer.
  - scripts/tools/build_two_rays.py — Build a minimal two-pixel CSV for per-object hit/miss tests.
  - scripts/tools/pixel_tools.py — Pixel helpers (extract from PixelFactory, generate toward-object ray).
  - scripts/tools/cleanup_runs.sh — Prune old runs and remove runtime logs from truth, preserving artifacts.
  - scripts/tools/deep_clean.sh — Remove build *.o and dupes.
  - scripts/tools/pipeline.cfg.sh — Centralized topics and sockets `(single source-of-truth)`.

### Script Consolidation targets (minimal command set)
- Start/Stop pipeline
  - Direct connect: scripts/pipeline/all_up.sh (start), scripts/pipeline/all_down.sh (stop)
  - Stepwise: scripts/pipeline/all_up_stepwise.sh (or scripts/pipeline/only_one_up.sh for a stage), scripts/pipeline/all_down.sh (stop)
- Validate artifacts and visuals
  - scripts/tools/stage_to_png.sh, scripts/artifacts/rerender_from_artifacts.sh, scripts/validation/count_png_colors.sh, scripts/validation/compare_artifacts.sh, scripts/validation/pretty_logs.sh
- Truth/config management
  - scripts/artifacts/freeze_truth.sh, scripts/artifacts/capture_objects.sh, scripts/pipeline/regenerate_truth.sh, scripts/artifacts/regenerate_ref_pixels.sh
- Cleanup and housekeeping
  - scripts/tools/cleanup_runs.sh, scripts/tools/deep_clean.sh

### Script consolidation plan
- [ ] Consolidate runners with a consistent interface:
  1) all_up: full pipeline with direct connections.
  2) only_one_up: run a single stage from a given input artifact to that stage's output (accepts input path, writes o*.txt).
  3) stage_to_png: given any stage's o*.txt, convert to oPNG.txt (to_opng.py) and run Feeder→Writer to render PNG.
  4) all_up_stepwise: orchestrate (2) across stages 2→6 with RESUME_STAGE and default inputs from runs/complete; emit artifacts and final PNG.

### Script Consolidation work to complete
- [ ] Unify “truth run” into regenerate_truth.sh with dynamic-port rendering and optional per-stage resume.
- [ ] Reduce overlap between run_objects_serial.sh and regenerate_truth.sh (single code path with flags).
- [ ] Provide a single entrypoint for per-stage execution (extend only_one_up.sh to stages 1–3).
- [ ] Standardize artifact file naming (no double ‘o’ prefix) and directory layout under runs/truth/{objects,stages}.
- [ ] Add cucumber feature(s) to exercise Stage 2–6 stepwise chain and PNG rendering from artifacts.
- [ ] Teach pretty_logs.sh to recognize runs/truth/objects and runs/truth/stages summaries.
- [ ] Integrate cleanup_runs.sh into regeneration flows (final step).

## Work items
[ ] - add a linter
### Pipeline
- [ ] Plan data-driven pipeline config (e.g., Pipeline.json) to centralize ports, topics, and stage parameters.
[x] Developer tooling (all_up/all_down idempotent; SMOKE_MODE gating)
    - Add consolidated build.sh or Makefile target to start all processes; add healthcheck pings;
[ ] - PixelFactory-driven smoke instead of Feeder, re-enable PixelFactory message sending and add a “small image” World.json override for fast runs.

#### Stepwise
[ ] - add a test to demonstrate stepwise rendering
[x] Tests for pipeline (smoke test)
    - Add inproc ZMQ integration tests that simulate a few pixels through: PixelFactory → IntersectWith (mock) → IntersectResults routing → Black/Lit/Background → ColorResults → Writer (mock).
    - Add stepwise I/O tests: given input files, ensure outputs match expected CSV and PNG checksum for a tiny scene.
[x] Resume from any stage
  - Stepwise now accepts RESUME_STAGE{1..5}_FILE and defaults to runs/complete/* for missing values.
  - Stage 3 binds Shader subscriber in stepwise so Feeder can connect; Stage 6 can generate PNG directly from oPNG.txt.
[ ] Fix Stage 4, 5 stability in stepwise
- [ ] Add single-pixel replay helper
  - Script to replay one Pixel/Intersection pair into any bus for debugging.
- [ ] Unskip stepwise feature
  - Remove @skip and adjust thresholds for minimal smoke in test/features/stepwise_pipeline.feature.
- [ ] Complete stepwise pipeline (PR #18 intent)
    - Define a stable CSV schema for Pixel and Intersection (reuse Print/Read utilities).
    - For each stage, add: read i*.txt → process → write o*.txt; exit with EOF row.
#### Performance
[ ] Replace busy-waits/usleep
    - Remove or gate usleep in hot loops; rely on ZMQ HWM or poll timeouts; add metrics to gauge throughput.
[ ] Performance and scaling
    - Consider replacing broadcast bus with per-edge direct PUB/SUB or ROUTER/DEALER graph; horizontally scale IntersectWith pool by object and by spatial partition.

#### Port Sanity
- [ ] Add `scripts/validation/ports_sanity.sh` to assert all fixed ports are free/in-use as expected, with hints to resolve conflicts.
- [ ] Document fixed-port policy and update README with a brief “tear down first” reminder and port map.

### Data handling
- [ ] Config and schema
    - Move hardcoded colors/weights/ports to World.json (or a new Pipeline.json). Include topics, ports, and world properties.

### Rendering
[x] Baseline ambient/emissive path
    - Shader currently computes baseline but doesn’t publish a COLOR for it; either Shader should publish it or a dedicated Ambient stage should.
[ ] Reflection integration
    - Wire Reflection into the standard launch set; ensure DepthChart expects layers up to world.maxdepth; include refractive path stub.
[ ] Shadow correctness bug
    - In IntersectResults.storeIntersection: shadow tests compare i.distance[0] < pixel.distance; ensure this can’t “leak” farther occluders; early-out anyhit is better. provide minimal scene in bin/World.json.
[x] - multiple IntersectWith objects in the smoke (e.g., box1 + sphere2), expand all_up.sh to launch them and set FEEDER_LIMIT.
[x] - Audit shadow test routing and per-light contribution generation to ensure ColorResults gets all expected messages (added SMOKE_MODE fast path for completion).
[x] - Refine DepthChart’s layer accounting when reflections are disabled to avoid unnecessary waits (SMOKE_MODE primary-layer completion).

### Verification and stats
- [x] Confirm Shader produces output in stepwise Stage 3
  - Verified with RUN_ID=verify-stage3 using reference input at runs/complete/oShader.txt; produced runs/verify-stage3/oCOLOR.txt with 19616 lines.
- [x] Confirm oShader.txt is parsed properly by Shader
  - Feeder -> Shader -> Logger path succeeded; Shader consumed 9808 pairs and Logger captured COLOR; EOF handled.
- [x] Add start/stop timestamp to logs and runtime at EOF
  - Implemented in base Widget: logs now include [TIMING] START/STOP and elapsed seconds; visible in shader and logger logs.
- [x] Writer color/object histograms
  - Writer now logs [STATS] distinct_colors and per-object pixel counts at EOF; robust to malformed payloads.
  
## Current tasks (live checklist)
- [x] Stage 5 isolated (complete oDEPTH → oPNG)
- [x] Stage 6 isolated (complete oPNG → PNG file)
- [x] pretty_logs utility for runs (scripts/validation/pretty_logs.sh)
- [ ] Able to push an image of arbitrary HxW size through pipeline using integration test harness
- [ ] Stepwise run from complete oIntersectResult (RESUME_STAGE=2 → PNG)
- [ ] Stepwise run from complete oShader (RESUME_STAGE=3 → PNG)
- [ ] Stepwise run from complete oCOLOR (RESUME_STAGE=4 → PNG)
- [ ] Stepwise run from complete oDEPTH (RESUME_STAGE=5 → PNG)
- [x] Stepwise run from complete oPNG (RESUME_STAGE=6 → PNG)
- [x] Confirm Writer [STATS] shows >1 distinct color and per-object counts
- [ ] Analyze oPNG.txt for missing pixels and trace to earliest missing stage (scripts/validation/trace_missing_pixels.py)

Truth data and artifact comparisons
- [ ] Regenerate a full set of stepwise artifacts from data/pixels.txt (truth run)
  - Start: feed pixels.txt to IntersectWith via proxy; capture oIntersectResult.txt
  - Stage 2: run IntersectResults and capture Shader, BKG, BLACK, LIT (oShader.txt, oBKG.txt, oBLACK.txt, oLIT.txt)
  - [x] Added scripts/pipeline/produce_intersectresults.sh to aggregate truth per-object IntersectResult into oIntersectResults.txt and render intersectresults.png using fixed BUS_SHADER.
  - Stage 3: run Shader + Background + Black + Lit; feed their inputs; capture oCOLOR.txt
  - Stage 4: ColorResults → capture oDEPTH.txt
  - Stage 5: DepthChart → capture oPNG.txt
  - Stage 6: Writer → test.png
  - Script: scripts/pipeline/regenerate_truth.sh
- [ ] Compare the truth artifacts with runs/complete for diffs and counts
  - Script: scripts/validation/compare_artifacts.sh <truth_run_dir>
- [ ] Visualize each step as a 150x100 PNG with purple marking missing pixels
  - Script: scripts/tools/stage_to_png.sh (Writer-based)
  - Script: scripts/artifacts/render_frames.sh <run_dir> <out_dir>



## Changelog (2025-10-29)
- Switched `scripts/tools/stage_to_png.sh` to fixed BUS_PNG and absolute OUT path; verified PNG output.
- Switched `scripts/validation/count_png_colors.sh` to fixed BUS_PNG; verified it reports counts.
- Switched `scripts/pipeline/produce_intersectresults.sh` to fixed BUS_SHADER; verified aggregate and PNG render.


