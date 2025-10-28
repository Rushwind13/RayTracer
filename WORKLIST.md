Distributed Ray Tracer — Worklist and Component Map (2025-10-27)

Scope
- Goal: Each rendering pipeline stage runs as its own process (ideally on its own CPU core) and is wired via ZeroMQ through the Widget interface.
- Repos in workspace: RayTracer (engine + pipeline stages) and zmq_widgets (ZeroMQ wrapper + broker).

GitHub status (reachability via MCP)
- Owner: Rushwind13
- RayTracer: 0 open issues; 1 open PR (#18 “Feat/stepwise” — converts pipeline to stepwise modules reading/writing text files).
- zmq_widgets: 0 open issues; 0 open PRs.

High-level data flow (streaming mode, over TCP sockets)
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

Network topics and sockets (from start.sh)
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

Pipeline components (files, purpose, I/O)
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

Math, scene, and rendering core (files, synopsis)
- Math.hpp: basic types (Position, Direction, Color, Range), constants (epsilon, deg2rad), linear algebra helpers, TransformRay, ReflectVector, RefractVector, matrix builders (Translate/Scale/Rotate/Shear), lerp.
- Object.hpp: materials, procedural patterns (Solid/Stripe/Gradient/Ring/Perlin-based), primitives (Sphere/Plane/Box) with intersection math; Light type.
- Camera.hpp: camera model; loads from bin/World.json; constructs rays via RayThroughPoint; builds camera frame.
- Lighting.hpp: helpers for light vectors and shading pseudocode references; Shadow(r) stub uses Intersection class.
- Intersection.hpp: Intersection data structure with msgpack, CSV Print/Read helpers.
- Raytracer2014.hpp: legacy prototypes and historical Python notes.

Tests (RayTracer/test)
- Frameworks: GTest + cucumber-cpp; feature files for tuples, rays, shapes, materials, boxes, patterns.
- Step definitions exercise: vector/point ops, normalization, dot/cross, reflect vector, transforms, normals, box intersection, materials.
- Gaps: no integration tests for the ZMQ pipeline, routing, or end-to-end image; no tests for stepwise file I/O format; no tests for per-stage EOF handling.

Actionable next steps (checklist)
[x] Align sockets and proxies
    - Ensure RESULT path connects: IntersectWith outputs 1300; IntersectResults subscribes 1301. Either run a 1300→1301 proxy or change one side for direct connect.
    - Document/run the minimal set of ControlChannel processes needed (or replace with direct links).
[ ] Complete stepwise pipeline (PR #18 intent)
    - Define a stable CSV schema for Pixel and Intersection (reuse Print/Read utilities).
    - For each stage, add: read i*.txt → process → write o*.txt; exit with EOF row.
    - Provide a small runner (run.py or Makefile target) that chains files between stages.
[x] Baseline ambient/emissive path
    - Shader currently computes baseline but doesn’t publish a COLOR for it; either Shader should publish it or a dedicated Ambient stage should.
[ ] Reflection integration
    - Wire Reflection into the standard launch set; ensure DepthChart expects layers up to world.maxdepth; include refractive path stub.
[ ] Shadow correctness bug
    - In IntersectResults.storeIntersection: shadow tests compare i.distance[0] < pixel.distance; ensure this can’t “leak” farther occluders; early-out anyhit is better.
[ ] Replace busy-waits/usleep
    - Remove or gate usleep in hot loops; rely on ZMQ HWM or poll timeouts; add metrics to gauge throughput.
[ ] Config and schema
    - Move hardcoded colors/weights/ports to World.json (or a new Pipeline.json). Include topics, ports, and world properties.
[x] Tests for pipeline (smoke test)
    - Add inproc ZMQ integration tests that simulate a few pixels through: PixelFactory → IntersectWith (mock) → IntersectResults routing → Black/Lit/Background → ColorResults → Writer (mock).
    - Add stepwise I/O tests: given input files, ensure outputs match expected CSV and PNG checksum for a tiny scene.
[ ] Performance and scaling
    - Consider replacing broadcast bus with per-edge direct PUB/SUB or ROUTER/DEALER graph; horizontally scale IntersectWith pool by object and by spatial partition.
[x] Developer tooling (all_up/all_down idempotent; SMOKE_MODE gating)
    - Add consolidated build.sh or Makefile target to start all processes; add healthcheck pings; provide minimal scene in bin/World.json.
[x] - multiple IntersectWith objects in the smoke (e.g., box1 + sphere2), expand all_up.sh to launch them and set FEEDER_LIMIT.
[ ] - PixelFactory-driven smoke instead of Feeder, re-enable PixelFactory message sending and add a “small image” World.json override for fast runs.
[x] - Audit shadow test routing and per-light contribution generation to ensure ColorResults gets all expected messages (added SMOKE_MODE fast path for completion).
[x] - Refine DepthChart’s layer accounting when reflections are disabled to avoid unnecessary waits (SMOKE_MODE primary-layer completion).

Quick file index (selected)
- Pipeline stages: RayTracer/{PixelFactory,IntersectWith,IntersectResults,Shader,Black,Lit,Background,ColorResults,DepthChart,Reflection,Writer,Configurator}/src
- Core math/scene: RayTracer/libRayTracer2014/src/{Math.hpp,Object.hpp,Camera.hpp,Lighting.hpp,Intersection.hpp,World.json}
- Networking: zmq_widgets/Widget/src/{Widget.hpp,Widget.cpp}, zmq_widgets/ControlChannel/src/ControlChannel.cpp
- Tests: RayTracer/test/features + step_definitions

Success criteria
- Streaming: All stages launch; topics deliver; Writer emits final PNG with plausible colors; Ctrl+C or EOF pixel shuts down cleanly.
- Stepwise: Each stage can process a file and produce the next; a simple script chains all into a PNG; unit/integration tests cover both paths.
