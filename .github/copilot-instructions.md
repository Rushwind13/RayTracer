# Copilot Instructions for RayTracer

## Architecture Overview

This is a **distributed, process-per-stage ray tracer** where each rendering step runs as an independent process communicating via ZeroMQ message passing. The pipeline is built on a "setup/loop" pattern using the `Widget` base class from the separate `zmq_widgets` repository.

### Core Pipeline Flow

```
PixelFactory → INTERSECT → IntersectWith (N workers, one per object)
  → RESULT → IntersectResults (aggregator)
  → SHADE → Shader (spawns shadow tests) ┐
  → BKG → Background                     │→ COLOR → ColorResults
  → BLACK → Black                        │    → DEPTH → DepthChart
  → LIT → Lit                            ┘         → PNG → Writer
```

Each component is a separate executable in `bin/`. See `WORKLIST.md` for detailed component responsibilities and data flows.

## Two Execution Modes

1. **Streaming mode**: Components publish/subscribe over TCP sockets in real-time via `ControlChannel` proxy (bottleneck; see README.md)
2. **Stepwise mode**: Pipeline runs stage-by-stage with file checkpoints (`runs/<RUN_ID>/o*.txt`). Use `scripts/pipeline/all_up_stepwise.sh`

## Critical Data Format: "Rendering Artifact CSV"

All inter-component data follows the **interleaved two-line format** (see `docs/Rendering_Artifact_CSV.md`):

**Format specification:**
- **Line 1 (odd)**: Pixel CSV (40 fields: x, y, oid, primaryRay, r, normal, position, distance, lid, NdotL, depth, weight, color, gothit)
- **Line 2 (even)**: Intersection CSV (13 fields: gothit, anyhit, oid, normal, position, distance[2]) **OR a blank line**

**Critical format rules:**
- **Blank line 2 = valid default Intersection** (gothit=false, oid=-1, distance=1e9, vectors zero)
- A blank Intersection line is the standard output from PixelFactory (primary rays have no hit data yet)
- When reading: blank line → call `ReadIntersection()` with empty string, which synthesizes defaults
- When writing: default Intersection → emit blank line (or call `PrintIntersection()` with default-constructed Intersection)
- **Line count invariant**: N pixels = 2N lines exactly (even if half are blank)
- **EOF convention**: single Pixel with `type=iInvalid`, `x=y=-1`, followed by blank line

**IMPORTANT: How to Verify Format Compliance**
When examining artifact files, ALWAYS verify format using proper tools:
```bash
# Check line count (should be 2N for N pixels)
wc -l runs/truth/pixels.txt  # Should be 30000 for 15000 pixels

# Verify blank lines exist using od (octal dump)
head -10 runs/truth/pixels.txt | od -c | head -30
# Look for "\n \n" patterns showing blank lines between Pixel records

# Check specific line content
awk 'NR==1 || NR==2 || NR==3 || NR==4' runs/truth/pixels.txt | while read line; do echo "Line $((++n)): [$line]"; done
# Lines 2 and 4 should be empty []
```

**DO NOT** assume format is wrong based on visual inspection alone. Terminal display can make blank lines invisible or confusing. Files like `runs/truth/pixels.txt` (30,000 lines) ARE correctly formatted when they have exactly 2N lines with alternating Pixel CSV and blank lines.

**Example (150×100 image = 15,000 pixels = 30,000 lines):**
```
0,0,-1,-0.727028,-0.506823,-0.463207,0,0,6,0,1,1,0,-0.727028,...,0,0,1
                                              <-- blank line (actually present as \n)
1,0,-1,-0.722378,-0.510433,-0.466506,0,0,6,0,1,1,0,-0.722378,...,0,0,1
                                              <-- blank line (actually present as \n)
...
149,99,-1,...,0,0,1
                                              <-- blank line (actually present as \n)
```

**NEVER parse CSV manually.** Always use `ReadPixel()`/`ReadIntersection()` and `PrintPixel()`/`PrintIntersection()` from `libRayTracer2014/src/{Pixel,Intersection}.hpp`.

## Dependencies & Setup

### zmq_widgets Repository
The pipeline requires the separate `zmq_widgets` repository at the same directory level:

```bash
# Directory structure:
# ~/Documents/code/
#   ├── RayTracer/        (this repo)
#   └── zmq_widgets/      (separate repo)

# Build zmq_widgets first:
cd ../zmq_widgets/Widget/src
make clean && make  # produces Widget.o

# Then build RayTracer:
cd ../../../RayTracer
./build.sh
```

Each component's `Makefile` links against `../zmq_widgets/Widget/src/Widget.o`. The `ControlChannel` proxy executable lives in `zmq_widgets/bin/`.

### Required Libraries
Install via Homebrew (except PNGwriter which needs manual cmake):
- ZeroMQ, libpng, GLM, msgpack-cxx, nlohmann-json
- PNGwriter: `git clone` + cmake
- For testing: GTest (googletest), cucumber-cpp

## Building & Running

```bash
# Build everything (creates bin/ directory if needed)
./build.sh

# Run smoke test (streaming, produces bin/test.png)
scripts/pipeline/all_up.sh

# Run stepwise with checkpoints (default 1 pixel for SMOKE_MODE)
RUN_ID=mytest scripts/pipeline/all_up_stepwise.sh

# Testing (BDD with Cucumber-CPP)
cd test
./bin/AllSteps & cucumber --tags ~@skip
```

Shared compiler flags: `-std=c++11`, `-DBASEDIR`, includes for GLM, msgpack, nlohmann-json, ZeroMQ, libpng, PNGwriter.

## Widget Pattern (All Components Inherit)

Every pipeline component subclasses `Widget` and implements:
- `local_setup()`: Initialize publishers/subscribers, load world config
- `local_work(msgpack::sbuffer *header, msgpack::sbuffer *payload)`: Process one message; return false to stop loop
- `local_shutdown()`: Cleanup (optional)

Main loop: `run()` → `setup()` → `loop()` (calls `local_work()` until `running=false`) → `shutdown()`

### Message Protocol
- **Header**: msgpack-serialized `Pixel` struct
- **Payload**: msgpack-serialized `Intersection` struct (often blank for primary rays)
- Topics match component names (e.g., "INTERSECT", "SHADE", "COLOR")
- EOF handling: `pixel.type = iInvalid; pixel.x = pixel.y = -1`

## Key Conventions

1. **No bash/Python for data parsing**: Business logic in C++; bash only for process orchestration (see `docs/Rendering_Artifact_CSV.md` housekeeping rules)
2. **World configuration**: `bin/World.json` defines objects, lights, colors, patterns. Loaded by `World::load_world()`
3. **Object/Light IDs**: `oid`/`lid` are indices into `World.objects`/`World.lights` arrays; all processes share identical world state
4. **Depth/Weight tracking**: `pixel.depth` (0-`maxdepth`), `pixel.weight` (0.0-1.0) control recursion cutoff
5. **Ray types**: `iPrimary=0`, `iShadow=1`, `iReflection=2`, `iRefraction=3`, `iInvalid=-1` (EOF)

## World.json Schema

Scene configuration in `bin/World.json`:

```json
{
  "filename": "test.png",
  "camera": {
    "eye": [0.0, 6.0, 0.0],
    "lookAt": [0.0, 1.0, -100.0],
    "up": [0.0, 1.0, 0.0],
    "fovy": 90.0,
    "width": 150.0,
    "height": 100.0
  },
  "colors": {
    "red": [1.0, 0.0, 0.0],
    "green": [0.0, 1.0, 0.0]
  },
  "patterns": {
    "red": {
      "type": "solid",
      "color": "red"
    },
    "vert": {
      "type": "stripe",
      "patterns": ["red", "green"],
      "scale": [0.2, 0.2, 0.2]
    }
  },
  "objects": {
    "sphere1": {
      "type": "sphere",  // or "box", "plane"
      "material": {
        "color": "red",
        "reflective": 0.5,
        "specular": 0.9,
        "usePattern": false,
        "pattern": {
          "name": "vert",
          "scale": [0.2, 0.2, 0.2],
          "axis": [0, 0, 1],
          "degrees": 45.0
        }
      },
      "center": [0.0, 1.0, -5.0],
      "scale": [1.0, 1.0, 1.0],
      "axis": [0, 0, 1],
      "degrees": 0.0
    }
  },
  "lights": {
    "light0": {
      "center": [0.0, 5.0, -1.0],
      "material": {
        "color": "white"
      }
    }
  }
}
```

**Critical**: Object order in JSON determines `oid` indices; light order determines `lid`. All processes must load identical `World.json` for message `oid`/`lid` references to be valid.

## Testing Strategy (BDD Focus)

Current work emphasizes **Rendering Artifact Schema BDD** (see `docs/SchemaBDD.md`):
- Validate two-line CSV format compliance in PixelFactory, Feeder, Logger, Writer
- Tests live in `test/features/*.feature` with C++ step definitions in `test/features/step_definitions/`
- Existing features: `matrix.feature`, `tuples.feature`, `rays.feature`, `spheres.feature`, `boxes.feature`, `planes.feature`
- Avoid `system()` calls in tests; prefer pure C++ validation functions

## Debugging Workflows

### Single-Pixel Trace
To trace a single pixel through the entire pipeline:

```bash
# 1. Generate just one pixel (modify PixelFactory loop or use FEEDER_LIMIT)
RUN_ID=single_pixel FEEDER_LIMIT=1 scripts/pipeline/all_up_stepwise.sh

# 2. Inspect artifacts at each stage:
cat runs/single_pixel/oINTERSECT.txt        # primary rays
cat runs/single_pixel/oIntersectResult.txt  # nearest hit per pixel
cat runs/single_pixel/oCOLOR.txt            # color contributions
cat runs/single_pixel/oDEPTH.txt            # depth-merged result
cat runs/single_pixel/oPNG.txt              # final pixel data to writer
```

Each file follows the two-line interleaved format. Use `ReadPixel()`/`ReadIntersection()` in C++ to parse, or manually inspect the CSV for debugging field values.

### Inspecting Intermediate CSV Files
Stepwise artifacts live in `runs/<RUN_ID>/`:
- `oINTERSECT.txt`: Primary rays from PixelFactory (blank Intersection lines)
- `oIntersectResult.txt`: Nearest hits from aggregator
- `oCOLOR.txt`: Accumulated shading contributions
- `oDEPTH.txt`: Depth-weighted pixels ready for PNG
- `oPNG.txt`: Final pixel stream consumed by Writer

**Validation patterns**:
```bash
# Count records (should be 2N lines for N pixels)
wc -l runs/mytest/oCOLOR.txt

# Extract specific pixel (e.g., x=50, y=49)
grep "^50,49," runs/mytest/oCOLOR.txt -A 1

# Convert artifact to PNG for visual inspection
scripts/tools/stage_to_png.sh runs/mytest/oCOLOR.txt output.png
```

### Per-Object Intersection Debugging
```bash
# Capture individual object intersections
scripts/pipeline/run_objects_serial.sh

# Inspect per-object files
cat runs/truth/objects/oIntersectResult.sphere1.txt
cat runs/truth/objects/oIntersectResult.box2.txt
```

## Common Pitfalls

1. **Payload carry-over bug**: Feeder/Logger must reset Intersection to default when line 2 is blank (don't reuse previous payload)
2. **EOF propagation**: Aggregators (IntersectResults, ColorResults, DepthChart) must forward EOF to all downstream topics
3. **Slow joiner**: Components use `usleep(100*1000)` in `local_setup()` before publishing
4. **Socket binding**: Pipeline uses fixed ports (see `scripts/tools/pipeline.cfg.sh`); conflicts possible if processes don't clean up
5. **Object count**: IntersectResults needs `world.object_count` to know when all per-object results arrived (currently hardcoded)

## File Locations

- **Pipeline components**: `{PixelFactory,Shader,IntersectWith,...}/src/*.{cpp,hpp}`
- **Shared library**: `libRayTracer2014/src/` (Math.hpp, Pixel.hpp, Intersection.hpp, World.hpp, Object.hpp, Camera.hpp, Lighting.hpp)
- **Scripts**: `scripts/pipeline/` (orchestration), `scripts/tools/` (utilities)
- **Test data**: `data/` (truth files), `runs/` (stepwise outputs)
- **Dependencies**: `glm-0.9.5.2/` (math), `include/FreeImage.h` (PNG)

## Active Development Focus

Per `WORKLIST.md` and `docs/SchemaBDD.md`, current priorities:
1. Enforce two-line interleaved format across all components
2. Add BDD features for PixelFactory, Feeder, Logger, Writer CSV compliance
3. Eliminate bash/Python data parsing in favor of C++ Read/Print functions
4. Stabilize stepwise pipeline (currently tagged `@skip` in tests)

When making changes, ensure they follow the "Housekeeping rules" in `docs/Rendering_Artifact_CSV.md` (avoid new scripts, reuse existing code, keep artifacts testable).
