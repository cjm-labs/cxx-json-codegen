# Tree-sitter Frontend Recommendation

This document records the v0.3.5 frontend parser research recommendation.

Decision:

> CJM should adopt Tree-sitter as the C++ frontend implementation.

This is a product and architecture decision, not an immediate production parser
switch inside the research spike.

---

# Evidence

The v0.3.5 spike demonstrated that Tree-sitter can represent the current CJM
C++ parser surface while preserving the existing compiler architecture.

The isolated prototype can:

- build Tree-sitter runtime and `tree-sitter-cpp` generated sources behind an
  internal, off-by-default CMake option;
- avoid requiring Node.js, Rust, npm, Cargo, Python, or the Tree-sitter CLI from
  normal CJM users;
- keep Tree-sitter headers, nodes, and parser handles inside the C++ frontend
  layer;
- parse C++ source into a frontend-owned adapter result;
- produce the existing `SourceFileSyntax` shape used by Semantic Analysis;
- extract namespace paths;
- extract struct declarations;
- extract ordinary non-static fields;
- preserve field type spellings, including nested template spellings;
- attach supported same-line field comments;
- extract enum declarations;
- extract `using` type aliases;
- locate concrete Tree-sitter syntax error markers for source-aware diagnostics;
- reject unsupported static fields before reporting parse success.

The practical syntax surface test covers the key forms required by the current
handwritten parser contract:

- preprocessor lines that are ignored by CJM extraction;
- nested namespace spelling such as `company::model`;
- `enum class`;
- `using` aliases;
- multiple generated structs;
- ordinary fields;
- same-line `json:"..."` comments;
- scalar type spellings;
- user-defined type spellings;
- `std::vector`;
- `std::map`;
- `std::optional`;
- `std::unordered_map`;
- nested template type spellings.

This is enough evidence to stop treating Tree-sitter as merely speculative.

---

# Rationale

The handwritten parser was a useful bootstrap tool for early CJM milestones.
However, continuing to grow it would increase long-term maintenance risk.

Tree-sitter is a better C++ frontend foundation because it provides:

- a tested C++ concrete syntax grammar;
- structured syntax nodes instead of line-oriented parsing;
- source byte ranges and line/column positions;
- explicit comment nodes;
- explicit syntax error recovery markers;
- support for common real-header shapes such as nested templates and multiline
  declarations;
- a lightweight C runtime that can remain an implementation detail.

CJM should invest engineering effort in a strict adapter over Tree-sitter
rather than expanding a handwritten C++ parser.

The important boundary remains unchanged:

```text
tree-sitter-cpp
    |
    v
CJM C++ frontend adapter
    |
    v
SourceFileSyntax
    |
    v
Semantic Analysis
```

Tree-sitter must not leak into Semantic Analysis, Metadata IR, backends, public
CLI behavior, CMake user APIs, or generated C++.

---

# Recommendation

CJM should adopt Tree-sitter as the C++ frontend implementation and replace the
handwritten parser through a dedicated migration milestone.

The production switch should be staged rather than bundled into the research
spike.

Reasons:

- the research spike has proven parser-surface parity for current supported
  syntax;
- the public CLI and CMake workflow deserve a focused migration PR;
- generated output and downstream behavior should be reviewed independently;
- staged migration keeps rollback simple;
- handwritten parser removal should happen only after Tree-sitter is the
  production default and tests remain green.

The recommended decision is:

> Adopt Tree-sitter, migrate production parsing in a follow-up milestone, then
> remove the handwritten parser after parity confidence.

---

# Migration Plan

1. Keep the v0.3.5 Tree-sitter code isolated behind the internal spike option.
2. Add a follow-up migration milestone for switching the production C++ parser
   implementation to Tree-sitter.
3. Run handwritten parser and Tree-sitter adapter against the same fixture
   corpus before the switch.
4. Preserve the existing `SourceFileSyntax` boundary so Semantic Analysis does
   not change during the parser migration.
5. Switch the CLI and CMake generation path to the Tree-sitter-backed parser in
   a dedicated PR.
6. Run golden tests, parser tests, semantic tests, CLI tests, generated compile
   tests, and downstream dogfood after the switch.
7. Remove the handwritten parser only after the Tree-sitter parser has become
   the default and the migration has stabilized.

---

# Version Strategy

CJM should not track Tree-sitter latest automatically.

CJM should pin a tested version pair:

```text
Tree-sitter runtime
tree-sitter-cpp grammar
```

Upgrades should be deliberate compatibility work. Each upgrade should run:

- adapter tests;
- parser fixture comparison;
- fail-closed diagnostics tests;
- generated golden tests;
- downstream dogfood where practical.

Readable release tags are acceptable for development. Production dependency
updates should record the exact version pair in documentation and review notes.

---

# Build Strategy

The research spike used pinned FetchContent with a CJM-owned CMake wrapper.

For production adoption, CJM should decide between:

- pinned FetchContent with a private wrapper; or
- vendored generated Tree-sitter runtime and grammar sources.

The production build must preserve these constraints:

- no Tree-sitter CLI requirement for normal users;
- no upstream grammar-generation step in user builds;
- no public Tree-sitter dependency in generated code;
- no parser-specific types outside the frontend layer;
- no change to `cjm_generate(...)` user-facing semantics.

Vendored generated sources remain the most stable long-term option if CJM wants
fully reproducible offline builds.

---

# Metadata Comment Policy

Tree-sitter makes additional comment forms technically feasible, including:

```cpp
// json:"name"
std::string name;

std::string name; /* json:"name" */

/*
 * json:"name"
 */
std::string name;
```

These forms should not be enabled merely because Tree-sitter can parse them.

Comment binding is CJM product semantics, not parser capability. Same-line
comments should remain the initial production rule until broader metadata
binding semantics are explicitly designed.

---

# Risks

Known risks:

- Tree-sitter node shapes may change across grammar versions;
- C++ preprocessor constructs require careful fail-closed policy;
- unsupported declarator forms must not be silently skipped;
- comment binding may become ambiguous if expanded beyond same-line metadata;
- FetchContent-based integration may be less reproducible than vendored
  generated sources;
- the adapter must remain strict even though Tree-sitter can parse much more
  C++ than CJM supports.

These risks are manageable and are lower than continuing to grow the
handwritten parser indefinitely.

---

# Non-Goals

This recommendation does not require:

- full C++ grammar support at the CJM product level;
- template instantiation;
- full C++ semantic analysis;
- Clang/LLVM integration;
- CJM-C;
- a frontend plugin system;
- new metadata syntax;
- changing Metadata IR;
- changing JSON backend behavior.

Tree-sitter should improve CJM's C++ source understanding while preserving the
same compiler pipeline and product boundary.
