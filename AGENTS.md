# so-so Engine development contract

## Product and priorities

Build production-grade, real-time simulations and games that are responsive,
lightweight, performant, and visually polished. Physical fidelity is a deliberate
feature choice; reliable behavior and understandable limitations are required.
Balance visual quality, frame time, memory, and iteration speed against the
workload being developed. Do not substitute a research simulation platform for
this goal.

OpenGL is the current backend and should be pushed as far as practical. Before
implementing another backend or replacing OpenGL, discuss the measured limitation,
OpenGL alternatives, compatibility impact, and migration cost with the user and
obtain agreement. Architecture and dependencies may evolve to meet the product
goals; existing code is context, not an obligation to preserve a poor design.

## Repository map and skills

- `so-so/src/so-so/Core/`, `Events/`: application, windows, layers, input, events.
- `so-so/src/so-so/Renderer/`: rendering interfaces, scene submission, meshes.
- `so-so/src/so-so/RenderAPI/OpenGL/`: backend implementations.
- `so-so/src/so-so/Resource/`, `ImGui/`: resources and editor integration.
- `sandbox/src/`: client application, editor, and interactive examples.
- `sandbox/Resources/Shader/`: runtime shader sources loaded by the renderer.
- `sandbox/assets/`: example assets; `so-so/vendor/`: dependencies.
- `CMakeLists.txt`, `scripts/`, `.github/workflows/build.yml`: build and CI.

Read the relevant repository skill before its workflow. Skills live in
`.agents/skills/`; read the file directly if automatic discovery is unavailable.

| Work | Skill |
| --- | --- |
| C++ architecture, resources, build, or dependency changes | [soso-engine-development](.agents/skills/soso-engine-development/SKILL.md) |
| Rendering, GLSL, graphics debugging, or performance work | [soso-opengl-performance](.agents/skills/soso-opengl-performance/SKILL.md) |
| Simulation stepping, physics, fields, or gameplay state | [soso-realtime-simulation](.agents/skills/soso-realtime-simulation/SKILL.md) |
| Code review, verification, commit, or push | [soso-change-review](.agents/skills/soso-change-review/SKILL.md) |

Read [development validation](docs/development-validation.md) for executable
commands, checks by change type, and current infrastructure limitations. Read
only the skills relevant to the task; their workflows complement this contract.

## Implementation standards

- Use C++23 and the surrounding first-party style: tabs for C++ indentation,
  braces on the declaration/control-statement line, `soso` namespace, PascalCase
  types and functions, `m_` members, and `s_` static storage. Match local naming
  and GLSL conventions. Keep formatting changes out of unrelated code.
- Headers include what they use and compile without an accidental include order
  or precompiled header. Use `#pragma once`; include `sspch.h` first in engine
  implementation files where the target requires it.
- Make ownership and lifetime explicit. Prefer value members and `unique_ptr`;
  use `shared_ptr` for actual shared lifetime. Separate borrowed views from owning
  storage. Raw pointers and references must have a clear valid lifetime.
- Release GPU resources while the owning context is valid. Define initialization,
  partial failure, detach, and shutdown behavior. Do not run reentrant callbacks
  under internal locks unless the contract explicitly supports it.
- Validate external input, sizes, formats, and runtime capabilities in Release as
  well as Debug. Assertions express programmer invariants; they do not replace
  recoverable error handling. Avoid unchecked arithmetic for allocation sizes.
- Keep simulation/game state independent of editor widgets and backend handles.
  Prefer small, explicit interfaces. Add abstractions, jobs, or data structures
  for concrete needs; account for their runtime and maintenance costs.
- Bound work and memory in real-time paths. Avoid unnecessary steady-state
  allocation, blocking I/O, GPU readback, and synchronization. Measure suspected
  bottlenecks before optimizing; document intentional costs and capacity limits.
- Preserve color-space, material, lighting, and numerical conventions explicitly.
  Quality reductions must be deliberate settings with validated behavior.
- Keep dependency changes focused. Record purpose, pinned version/revision,
  license, platform support, build/distribution impact, and measured costs when
  relevant. Preserve unrelated vendor/submodule edits.
- Add build settings to the owning CMake target with correct visibility. Preserve
  supported configurations and document platform prerequisites. Keep runtime
  resources portable and writable state separate from installed assets.

## Change, verification, and delivery

1. Inspect the working tree and relevant callers. Establish the intended behavior,
   failure cases, ownership, and applicable performance/visual acceptance criteria.
2. Implement a complete, bounded change. Correct root causes; do not hide errors
   with silent fallbacks, success-shaped stubs, arbitrary delays, or unexplained
   constants. Document intentional approximations, supported limits, and tradeoffs.
3. Add meaningful unit tests for changed logic and bug regressions where applicable;
   use integration, graphics, and benchmark checks for behavior they actually cover.
   Exercise error paths. Do not add tests that merely mirror implementation details.
4. Run appropriate checks and inspect their results. Fix failures introduced by the
   change. Do not weaken assertions, tolerances, baselines, warnings, or checks to
   obtain a pass. Disclose existing failures and unavailable hardware/platforms.
5. Review the final diff before committing, including tests, build changes, and
   resources. Resolve correctness, lifetime, portability, and scope findings; rerun
   affected checks after fixes. A build alone does not establish runtime correctness.
6. Stage explicit paths and inspect the staged diff. Commit/push only when requested
   or otherwise authorized; preserve unrelated work and never force-push by default.
   Report the change, actual validation, remaining limitations, and commit identity.

Documentation-only changes need document/skill/link validation, not artificial C++
unit tests. An unrelated baseline build failure may remain for such a change when
clearly disclosed. Behavioral changes require evidence covering the affected
behavior; missing test infrastructure is a gap to address, not a test pass.

Keep this contract and the relevant skills current when commands, architecture,
or supported behavior change. These files guide development; they do not establish
that CI gates, tests, benchmarks, or release guarantees already exist.
