---
name: soso-engine-development
description: Implement or refactor so-so Engine C++ architecture, ownership, resource loading, CMake, or dependencies. Use for engine and application infrastructure changes; use the specialized simulation or OpenGL skill for those domains.
---

# Engine development

Read the root `AGENTS.md` and `docs/development-validation.md`. Treat their current
baseline notes as observations to recheck, not permanent exceptions to quality.

## Establish the contract

Trace the affected caller, owner, and teardown path before choosing an interface.
Specify success, failure, invalid-input behavior, and thread/context affinity.
For architectural work, explain the concrete problem, dependency direction, and
migration steps. Keep intermediate commits coherent and buildable when the
baseline permits; complete the requested migration, including affected callers.

Architecture and dependencies may change. Compare runtime cost, complexity,
portability, maintenance, licensing, and distribution before selecting a design.
Record substantial decisions in a short document under `docs/` with context,
alternatives, decision, and consequences. Follow the root backend-discussion rule.

## Implement with explicit lifetime

- Use RAII for owned CPU/GPU resources, including partial initialization failure.
  Define copy/move behavior. Keep borrowed spans/references distinct from owners;
  an owning buffer must not accidentally free borrowed memory or shallow-copy it.
- Destroy layers, materials, cached resources, and backend objects in a deliberate
  order before destroying their context. Account for static registries and objects
  retained by callbacks. Initialization should unwind completed steps on failure.
- Make public headers self-contained. The engine's PCH must not mask missing
  includes or cyclic dependencies; check a changed public header in isolation.
- Check sizes, offsets, overflow, asset formats, and missing/corrupt resources
  before use. Provide useful error context and leave objects in a defined state.
- Specify when submitted commands release borrowed state. A queued pointer is
  valid only if its owner outlives execution; document or enforce that lifetime.
- For shared queues, define producer/consumer ownership and shutdown. Execute
  callbacks outside the queue lock when reentry is possible; test the contract.

## Build and dependency changes

Use target-scoped CMake options and explicit dependency visibility. Include new
sources/resources reliably; if retaining current source globs, reconfigure before
claiming a new file was compiled. Keep Debug/Release/Dist and platform differences
intentional. Do not add a dependency to the public interface without accounting
for downstream include/link requirements.

Pin dependency revisions or versions and document retrieval, prerequisites, local
patches, licenses, runtime files, and update procedure. Inspect existing submodule
state before touching it. Verify system-library and vendored modes actually used
by the change. Never solve a portability problem with an undocumented absolute path
or assume successful linking proves deployment works.

Separate read-only installed assets, writable caches, and user settings. Check
launch from outside the source checkout and handle missing/unwritable paths.

## Verify the changed contract

Choose tests that would fail for the original defect or wrong behavior. Cover
repeated reset/load/unload, invalid inputs, empty/boundary sizes, and failure
cleanup where relevant. Use sanitizer checks for lifetime-sensitive work and
integration tests for context-dependent behavior. Test failure reporting in a
Release configuration when it must survive disabled assertions.

Run the relevant builds/checks from the validation guide. Review callers and final
ownership again after the implementation, then use `soso-change-review` before an
authorized commit. State precisely which configurations and platforms were checked.
