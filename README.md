## HOW TO RUN THE RAYTRACER
* Zeroth) ZeroMQ, libpng, GLM, msgpack, nlohmann-json and libPNGWriter are required (use brew except for pngwriter which needs to be git clone / cmake'd)
* First) Widget.o is required to link (this is my implementation of ZeroMQ over IPC, using a "setup/loop" methodology)
* Second) ControlChannel (found in zmq_widgets/bin) is required and must be started first! (this links the "broadcast" and "control" channels together -- note, this is by far the biggest performance bottleneck of the build -- should probably use separate channels for each function)
* Third) start Black, Background, ColorResults, DepthChart, IntersectResults, Lit, Reflection, Shader, and Writer (these are all services, so start them in separate windows / processes)
* Fourth) start IntersectWith sphere1 and IntersectWith sphere2 and ... (one instance per World object in libRayTracer2014/src/World.hpp)
* Finally) start PixelFactory (which creates the rays and starts running them through the channels)

PixelFactory will log lots of output
So will Writer and DepthChart
ColorResults will output lots when the objects get hit

once all the pixels have run through, a file called test.png will appear in the main folder. That's your traced file.

Change world objects by changing World.hpp (requires rebuild of most of the code)

Use build.sh to build all objects and put the executables in bin/ (you will have to create this folder the first time)

## New for 2025: AI Support
https://chatgpt.com/g/g-68900eb6c144819184ca3a14c566b8b5-raytracer-development

## Orchestration scripts

- Continuous smoke test (end-to-end):
	- Script: `scripts/pipeline/all_up.sh`
	- Starts the proxy, all services, fan-out IntersectWith workers, and a Feeder that drives a quick test set. Produces `bin/test.png` on success. Set `SMOKE_MODE=1` for simplified aggregator completion and extra debug logs; default stays quiet.

- Stepwise mode (stage-by-stage with file checkpoints):
	- Script: `scripts/pipeline/all_up_stepwise.sh`
	- Runs the pipeline in 6 stages. Each stage uses a `Logger` to capture bus traffic into a run directory and a `Feeder` to feed that captured data into the next stage. This lets you pause after any stage, inspect files, and even trace a single pixel.
	- Usage:
		- Optional: `RUN_ID=<label>` to name the run directory (default is timestamp)
		- Optional: `SMOKE_MODE=0|1` to control simplified completion in aggregators (default 1 for stepwise)
	- Outputs:
		- Run artifacts under `runs/<RUN_ID>/` as `o<CHANNEL>.txt` (e.g., `oIntersectResult.txt`, `oCOLOR.txt`, ...)
		- Final PNG at `bin/test.png`

Notes:
- `Logger` now respects `LOG_BASEDIR` environment variable at runtime to choose the output folder; defaults to the compile-time base if unset.
- `Feeder/start.sh` accepts an optional third argument to override the output socket (default is the proxy XSUB at `tcp://127.0.0.1:1314`).

## Testing (approved commands)

Use these commands from the test directory to run the suite and individual features:

- Full test suite (skip @skip-tagged scenarios):

	```bash
	cd /Users/jimbo/Documents/code/RayTracer/test; ./bin/AllSteps & cucumber --tags ~@skip
	```

- Single feature:

	```bash
	cd /Users/jimbo/Documents/code/RayTracer/test; ./bin/AllSteps & cucumber features/<featurename>.feature
	```

The stepwise pipeline feature is currently tagged `@skip` while stabilization is in progress.
