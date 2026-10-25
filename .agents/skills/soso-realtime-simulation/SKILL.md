---
name: soso-realtime-simulation
description: Build or change so-so Engine real-time simulation, physics, field, or gameplay update behavior. Use for stepping, state, interaction, stability, and simulation performance; exclude rendering-only changes and research-grade solver validation unless explicitly requested.
---

# Real-time simulation and gameplay

Read the root `AGENTS.md` and `docs/development-validation.md`. Optimize for
responsive, visually compelling interaction with deliberate fidelity and bounded
cost. State approximations and supported conditions without claiming scientific
accuracy beyond demonstrated behavior.

## Define the simulation contract

Specify state, units/conventions, initialization, boundary conditions, external
inputs, reset behavior, update frequency, expected workload, and memory/frame-time
budget. Describe how the user interacts with it and how invalid settings fail.
Choose numerical fidelity and stability criteria appropriate to the experience.

Keep simulation state and stepping testable without ImGui or a graphics context
where practical. Editor controls submit explicit inputs; rendering consumes a
defined state or snapshot. For GPU simulations, expose initialization and readback
for small verification cases without forcing readback on the normal frame path.

## Control time and work

- Separate simulation time from rendering cadence. Use a fixed step or justified
  bounded adaptive/substep scheme, accounting for the method's stability limits.
  Do not feed arbitrary display-frame deltas into a solver without justification.
- Define catch-up limits and behavior under overload, pause, single-step, reset,
  and resume. Limit accumulated debt so a slow frame cannot trigger unbounded work.
  State whether overload slows simulated time or discards elapsed wall time.
- Apply inputs at defined update boundaries. Interpolation for presentation must
  not mutate authoritative state or change results based on display FPS.
- Bound object/grid counts and scratch memory. Prefer reusable, contiguous state
  in hot loops where appropriate. Define overflow/capacity behavior and handle
  allocation failure rather than silently dropping required state.
- Validate dimensions, time scales, parameter ranges, singular/degenerate cases,
  and non-finite inputs before they corrupt state. Detect unstable output and
  surface a useful diagnostic rather than hiding NaNs with unexplained clamping.
- Make randomness explicitly seedable when replay or testing requires it. State
  the reproducibility scope; do not promise cross-device bitwise floating-point
  equality. Define safe ownership at CPU/GPU or worker/render boundaries.

## Test the intended experience

Use small controlled cases with independently justified expected behavior, such
as constant motion, equilibrium, symmetry, collision separation, or a simple field
solution. Check relevant invariants and justified tolerances; conservation is only
an expectation when the chosen model and boundary conditions imply it. Full
research validation and duplicate production CPU/GPU solvers are not prerequisites.

For changed stepping, compare simulated progress under different rendering rates
and a long-frame spike. Exercise reset/restart, pause/single-step, empty/minimum and
capacity workloads, invalid inputs, and repeated creation/destruction as relevant.
Tests should observe state/behavior, not just echo update formulas.

Run the interactive example and inspect responsiveness, visual artifacts,
stability over representative runtime, frame-time spikes, and memory growth.
Expose useful diagnostics such as step time, substeps, active count, or instability
status when they help development or user decisions. Use the OpenGL/performance
skill for GPU implementation and measured optimization, and `soso-change-review`
before an authorized commit. Report limits and the checks actually performed.
