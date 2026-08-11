# AGENTS.md

Gargantuan is a C++23 game engine scriptable from Luau. The user may override these defaults, except for the documentation-related ones.

## Design defaults

- Study how established products and this project already do things before designing something new. Reuse a proven pattern instead of inventing another one.
- Pick the simplest implementation that fully meets current requirements. Don't add unnecessary abstractions, config options, indirection, or one-off helpers.
- Don't preserve backward compatibility. Delete old and unused code paths instead of layering in compatibility shims, fallbacks, or migrations.
- Grow the system in layers. Start from the smallest version that works end to end, and add each capability on top of a system that already works. Don't leave the product in a half-working state while you build out complexity.
- Keep things modular. Reach for a well-maintained library when it cuts complexity, but check what's already a dependency (and its docs/types) before reimplementing something yourself.
- Aim for long-term architectural solutions instead of quick, temporary patches.

## Boundaries and task mode

- Never edit human-facing documentation, even if explicitly asked. This includes `docs/**`, `README*`, `CONTRIBUTING*`, licenses, changelogs, release notes and issue & PR text. Instead, read the relevant sources and give the user verified facts or suggested wording in chat. The one exception is `AGENTS.md`, which can be edited, but only when explicitly requested.
- Treat any unexpected changes in the working tree as the user's own work. Do not discard them. Don't commit, push, file issues, or open pull requests unless asked to.
- For a one or two-file task, skip the formal plan, todo list, subagent, or repo inventory. Just read the named implementation and its closest test, make the edit, verify it, and report back.
- Treat the stated commands and generation rules as verified. Do not rediscover them unless a task exposes a mismatch or needs unlisted detail.
- Keep changes focused. Small cleanup is fine, but only inside code you're already working on, and only if it reduces complexity without changing behavior more broadly.

## Repository orientation

- Use the "Contribute to Gargantuan" developer article in the docs as the starting point for components, conventions, and contribution workflows. Find it by title instead of guessing its path. Once you've read it, only inspect the code directly relevant to the task; don't map out the whole codebase beforehand.
- Reflected classes and services are schema-generated, so never edit the generated output directly. Change the schema instead, or the handwritten behavior alongside it. To regenerate, run `just build` rather than invoking the generator directly.
- Generated output may not exist yet in a fresh checkout or worktree. Don't search for generated examples or treat missing generated files as a problem; use the schemas and handwritten code as the source of truth, then let the normal build or generation command create the output.

## Implementation and tests

- Follow the nearest existing implementation and test pattern. Do not introduce a second convention.
- Make surgical edits: insert one sibling entry or test instead of retyping a function, dispatch table, or test block. Trust a successful edit result; do not re-read solely to confirm it landed.
- Do not add comments. Prefer clear names and structure; preserve only comments that remain accurate.
- Add focused behavioral coverage for new observable behavior and bug fixes. Refactors with unchanged behavior need no new test.
- Tests must fail on the plausible bug. Add new behavior as a sibling `it` block; do not repurpose existing coverage or change its inputs and assertions unless the old contract changed.
- Never edit generated or vendored files to make verification pass.

## Build and verification

The command file is `.justfile`.

1. In a fresh checkout or worktree only: run `just submodules` if dependencies are missing, then `rokit install` if pinned tools are missing.
2. Run `just configure` before your first build in a fresh checkout or worktree.
3. `just build` builds the project and regenerates reflected classes/services.
4. `just test_core` runs the Luau behavior tests; `just test` currently aliases it.
5. For a quick runtime check, use whichever is narrowest for the change: `just run_example <file>`, `just run_studio`, or `just gargantuan <args>`.

Match your verification to the change:

- C++ source-only refactor: format and compile only the changed file. Configure first if needed, but don't run the full build or broader tests when behavior hasn't changed.
- Luau API or datatype behavior: format, then make one build and test attempt if the baseline is known to work. If the build fails in unrelated code, stop there, compile the changed file if possible, and report that the behavior tests couldn't run.
- Runtime behavior: build and exercise the relevant example or project.
- Schema change: run generation, then build. Only check the generated output itself if you're debugging the generation step.

Format touched C++ with `clang-format -i` and Luau with `stylua`, following `.clang-format` and `stylua.toml`.

If a command fails, read the first actionable error and don't run the same underlying operation again through another wrapper, job count, or direct CMake/Ninja command. If the failure is unrelated, switch immediately to the narrowest unaffected check you can run, then report the blocker instead of trying to fix it.