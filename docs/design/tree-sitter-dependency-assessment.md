# Tree-sitter Dependency Assessment

This document records the v0.3.5.2 dependency assessment for evaluating
Tree-sitter as a possible future CJM C++ frontend implementation detail.

This is not an adapter design and does not change the production parser.

---

# Goal

Before writing a Tree-sitter adapter prototype, CJM needs a clear answer to one
practical question:

> Can Tree-sitter be evaluated without making CJM's normal build or downstream
> CMake workflow depend on parser-development tooling?

The answer should guide the prototype build strategy, version pinning, and
license handling.

---

# Required Upstream Components

The spike needs two upstream components:

- `tree-sitter`: the C runtime used to parse source text.
- `tree-sitter-cpp`: the C++ grammar and generated parser sources.

Both projects are MIT licensed. If CJM later includes or vendors source files
from either project, `THIRD_PARTY_NOTICES.md` must be updated from an
informational notice to a concrete included-dependency notice.

---

# Build Constraints

The production CJM build must keep the current user experience:

- no Node.js requirement;
- no Rust requirement;
- no npm requirement;
- no Cargo requirement;
- no Python requirement;
- no Tree-sitter CLI requirement;
- no network access requirement for the default build;
- no public Tree-sitter types in Semantic Analysis, Metadata IR, backends, CLI,
  or generated code.

Tree-sitter must remain isolated inside the C++ frontend layer.

---

# Observed Upstream Build Shape

The Tree-sitter runtime provides a CMake build for the runtime library.

The `tree-sitter-cpp` repository contains generated parser sources, but its
upstream CMake file also runs the Tree-sitter CLI to regenerate `src/parser.c`
from `src/grammar.json`.

That upstream CMake behavior is useful for grammar development, but it is risky
for CJM's downstream workflow because a normal CJM user should not need the
Tree-sitter CLI just to build a project that uses CJM.

For CJM, generated parser sources should be consumed as generated sources, not
regenerated during a normal CMake configure or build.

---

# Integration Options

## Option A: Use Upstream CMake Directly

Advantages:

- least custom CMake code;
- follows upstream project layout.

Risks:

- may require the Tree-sitter CLI;
- may regenerate parser sources during build;
- may expose upstream CMake options to CJM users;
- depends on upstream CMake layout remaining stable.

Decision:

> Not recommended for CJM's user-facing build.

## Option B: Pinned FetchContent With A CJM Wrapper

Advantages:

- keeps the CJM repository smaller during research;
- pins exact upstream commits or releases;
- allows CJM to compile only the required generated C sources;
- avoids invoking upstream grammar-generation steps.

Risks:

- still needs network access when the spike option is enabled;
- CJM must track upstream source paths;
- the wrapper must be kept private to the frontend spike.

Decision:

> Good for the v0.3.5 prototype if it remains off by default.

## Option C: Vendored Generated Sources

Advantages:

- reproducible offline builds;
- no dependency on upstream CMake behavior;
- no Tree-sitter CLI during normal builds;
- easiest path to a stable future production parser dependency.

Risks:

- adds third-party source files to the CJM repository;
- requires explicit license notice maintenance;
- upstream upgrades become manual and should be tested carefully.

Decision:

> Best candidate if Tree-sitter becomes the production frontend foundation.

---

# Recommended v0.3.5 Strategy

Use an internal, off-by-default spike path.

The preferred research sequence is:

1. Add a private build option such as `CJM_ENABLE_TREE_SITTER_SPIKE`, default
   `OFF`.
2. Compile the Tree-sitter runtime and `tree-sitter-cpp` generated sources
   behind that option only.
3. Do not call the Tree-sitter CLI from CJM's normal CMake build.
4. Keep all Tree-sitter includes and types inside `src/frontends/cxx`.
5. Record the exact runtime and grammar versions as a tested pair.
6. If vendoring source files, update third-party notices in the same commit.

This lets CJM answer the frontend research questions without changing the
public build, CLI, Metadata IR, semantic layer, or backend behavior.

---

# Prototype Questions

The prototype should answer:

- Can the runtime and C++ grammar compile cleanly on macOS and Linux?
- Can generated `parser.c` and `scanner.c` be compiled without the CLI?
- Which include directories are required?
- Are the runtime and grammar ABI versions compatible?
- What is the build-time and binary-size impact?
- Can the default CJM build remain network-free?
- Can all Tree-sitter symbols stay inside the frontend implementation?

---

# Acceptance Criteria

This task is complete when CJM has:

- documented the required upstream components;
- documented why upstream `tree-sitter-cpp` CMake should not be used directly
  as the user-facing build path;
- compared FetchContent and vendored-source strategies;
- chosen an off-by-default v0.3.5 prototype strategy;
- documented external tooling that must not be required from users;
- documented license notice expectations;
- left production code unchanged.

---

# Out Of Scope

This assessment does not:

- implement a Tree-sitter adapter;
- replace the handwritten parser;
- change Semantic Analysis;
- change the Metadata IR;
- change generated output;
- add new JSON mapping support;
- introduce a public Tree-sitter dependency.
