---
name: soso-change-review
description: Review and verify so-so Engine changes before a commit or push, or perform a requested code review. Use for final diff inspection, test evidence, regression risk, and scoped Git delivery; invocation alone does not authorize committing or publishing.
---

# Change review and delivery

Read the root `AGENTS.md` and `docs/development-validation.md`. Review the actual
final implementation and evidence against the requested behavior. For review-only
requests, report findings without changing files or publishing unless authorized.

## Establish scope and evidence

Inspect working-tree, staged, and new/untracked task files. Separate existing work
from the task; do not stage unrelated files, submodule changes, generated caches,
build products, logs, or credentials. Read relevant callers, tests, build rules,
and resources, not only changed lines. Check the requirement is actually fulfilled.

Classify the change and select appropriate verification from the validation guide.
Confirm checks ran against the current content and configuration, not an older
revision. Empty CTest discovery, successful compilation, mocked GL, and screenshots
alone cannot establish simulation, GPU, or deployment behavior they do not exercise.

## Review before committing

- Correctness: invalid/empty/boundary inputs, error propagation, partial failure,
  size arithmetic, state transitions, and Release behavior when assertions vanish.
- Lifetime: copy/move and borrowing, callbacks, locks/reentry, thread/context
  affinity, resource reuse, and teardown with the graphics context still valid.
- Architecture/style: root conventions, self-contained headers, clear dependency
  direction, focused scope, complete callers, portable CMake and resource paths.
- Graphics/simulation when affected: buffer layouts/bindings, pass hazards,
  synchronization, shader reload/cache validity, stepping, overload, stability,
  color-space/visual consistency, and defined capacity behavior.
- Performance when affected: representative before/after evidence, allocation,
  memory, synchronization, and spikes; investigate an unexplained regression.
- Tests: observable requirements and regressions, independently justified expected
  values, failure cases, tolerances, repeatability, and platform coverage. Confirm
  tests are actually registered and failures produce a failing exit status.
- Dependencies/delivery: pinned revisions, local vendor edits, licenses/assets,
  runtime files, and a real install/package launch check for deployment changes.
- Documentation/skills when affected: accurate paths and commands, valid metadata,
  resolved references, clear triggers, no contradictory rules or invented tooling.

Record concrete findings with severity, location, trigger, and impact. Resolve
blocking correctness or verification findings before committing; rerun affected
checks after fixes and review the resulting diff. Distinguish existing unrelated
failures from regressions. Documentation-only work may proceed with an explicitly
disclosed baseline build failure after its own validation passes. If behavior
cannot be verified, report that limitation instead of asserting production readiness.

## Commit and push when authorized

1. Inspect `git status --short`, the task diff, and `git diff --check`. Stage
   explicit reviewed paths. Inspect `git diff --cached --check`, the complete
   staged diff, and staged file list; do not use blanket staging in a dirty tree.
2. Confirm no fixes made after verification invalidated the evidence. Commit with
   a concise message describing the resulting change. Preserve hooks and checks.
3. Verify the commit file list and working-tree state. Before a requested push,
   confirm repository/branch/upstream and remote head; push normally with an
   explicit destination. If rejected because the remote advanced, fetch and inspect
   divergence before proceeding. Do not force-push or overwrite unrelated history.
4. Verify the remote accepted the expected commit. Report commit/branch, review
   outcome, checks passed, failures/not-run checks, and material remaining risks.
   Do not claim remote CI passed until its result has actually been observed.

This workflow does not grant permission to commit, push, merge, publish a release,
or change repository protection. Use authorization from the active user task.
