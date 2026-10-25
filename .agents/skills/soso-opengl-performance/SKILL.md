---
name: soso-opengl-performance
description: Develop, debug, review, or optimize so-so Engine OpenGL rendering and GLSL, including visual quality and CPU/GPU performance. Use for graphics changes or measured optimization work, not ordinary C++ edits without a rendering or performance concern.
---

# OpenGL, visual quality, and performance

Read the root `AGENTS.md` and `docs/development-validation.md`. OpenGL is the
backend to develop. Discuss a demonstrated limitation and alternatives with the
user before implementing a backend replacement or addition.

## Define the workload

For performance work, reproduce a representative scene or simulation and identify
whether CPU submission, GPU work, memory traffic, allocation, or synchronization
is limiting it. Record hardware/driver, build, resolution, settings, workload size,
VSync/frame cap, warm-up, sample count, CPU/GPU frame times, and memory use.
Verify profiling instrumentation is active. A Debug CPU timing or average FPS
alone does not establish a Release GPU optimization.

For visual changes, define the desired appearance and acceptance scene/cameras.
Keep lighting, exposure, materials, resolution, and quality settings controlled in
comparisons. Establish the frame-time and memory budget for the intended workload.

## Preserve graphics correctness

- Inspect both `Renderer/` submission and `RenderAPI/OpenGL/` execution, plus the
  GLSL in `sandbox/Resources/Shader/`. Trace actual pass order and bindings rather
  than assuming the abstraction describes all backend state.
- Check required context version/extensions and numeric limits. The intended
  OpenGL baseline does not prove the created context or driver has every feature.
  Fail with actionable diagnostics when a required capability is unavailable.
- Make pass inputs, outputs, viewport, framebuffer attachments, depth, blending,
  culling, and color-space state explicit. Validate framebuffer completeness and
  avoid sampling/writing the same image without a defined supported technique.
- Match C++ buffer layouts to reflected GLSL sizes, offsets, alignment, and binding
  points. Verify UBO versus SSBO targets and actual allocated/uploaded ranges.
- Bound compute invocations against data dimensions, including partial workgroups.
  Choose memory barriers for the producer/consumer resource access. Workgroup
  barriers do not synchronize separate workgroups or replace command-level hazards.
- Keep GPU operations on a thread with the required context. Protect reused buffer
  regions with a correct synchronization strategy; persistent mapping does not
  remove lifetime/fence requirements. Avoid blocking readback in normal frame work.
- Make shader reload transactional: compile/link and validate a replacement before
  replacing the live program; preserve a usable old program on failure. Reconcile
  reflection/material layouts. Cache keys must represent sources, stages, compiler
  settings, and relevant versions; program binaries also depend on driver/device.
- Keep linear lighting, sRGB texture/output handling, HDR formats, exposure, and
  tone mapping consistent. Check shadows, normal maps, environment maps, and
  resizing on the maintained visual scene when affected.

## Optimize and verify

Choose techniques based on measured bottlenecks: batching, instancing, culling,
buffer streaming, indirect draws, compute, or pass/resource changes are options,
not requirements. Account for CPU overhead, GPU cost, VRAM, latency, and complexity.
If an experiment does not help, report the result rather than claiming improvement.

Run actual shader compilation/linking and render the relevant paths, including
reload, resize, disabled/optional passes, and failure behavior when affected. Inspect
GL debug errors; investigate new warnings rather than suppressing them. Pair CPU
unit tests with context-backed tests where GPU behavior is the contract.

Compare before/after captures on the same workload and hardware. Use asynchronous
GPU timer results; do not introduce `glFinish` or immediate query waits into the
normal loop to make timing easier. Review representative frame-time spikes and
memory as well as typical timing. Image comparisons need justified tolerances and
visual inspection; never regenerate expected images merely to silence a failure.

Report visual/correctness evidence, measured tradeoffs, and untested hardware.
Use `soso-change-review` before an authorized commit.
