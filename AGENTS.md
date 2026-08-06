# AGENTS.md

# CJM Development Guide for AI Assistants

This document defines the engineering principles for AI assistants contributing to CJM.

All implementation decisions should follow these rules.

> **This repository values long-term architecture over short-term implementation convenience.**

---

# Project Overview

CJM is a build-time code generation tool for Modern C++.

The initial goal is to provide Go-style JSON metadata and generate ordinary C++ serialization code.

The long-term goal is to become a production-quality build-time code generation platform for Modern C++.

---

## Repository Priority

Before making architectural or implementation changes, always consult the following documents in order:

1. AGENTS.md
2. ARCHITECTURE.md
3. docs/philosophy.md
4. docs/vision.md
5. docs/design/*
6. ROADMAP.md

Implementation must follow the documented architecture.
---

# Product Philosophy

CJM follows one fundamental principle:

> **Standard C++ in. Standard C++ out.**

Users write ordinary C++.

CJM generates ordinary C++.

Everything else is an implementation detail.

## Product Identity

CJM is a product.

Parser implementations, AST libraries, JSON libraries, and code generation techniques are implementation details.

Users should think about CJM—not about the technologies used to implement it.

---

# Architecture


The high-level architecture is:

```
User Source

↓

Parser

↓

Semantic Analysis

↓

Metadata Model

↓

Code Generator

↓

Generated C++

↓

User Compiler
```

Each layer has a single responsibility.

Responsibilities must never be mixed.

## Architecture Is the Product

This repository is architecture-driven.

Architecture is considered part of the product.

Implementation exists to realize the architecture—not to redefine it.

When implementation convenience conflicts with architecture, architecture always wins.


---

# Architectural Rules

The following rules are mandatory.

## Parser

Responsible for:

- parsing source code
- exposing syntax information
- preserving source locations

The parser must NOT:

- validate metadata
- generate code
- resolve generation order
- implement serialization logic

---

## Semantic Analysis

Responsible for:

- metadata validation
- type resolution
- dependency analysis
- diagnostics
- Metadata Model construction

Semantic Analysis owns all business logic.

---

## Metadata Model

The Metadata Model is the IR (Intermediate Representation) of CJM.

All parser implementations produce it.

All generators consume it.

Never bypass the Metadata Model.

Parser implementations and generators must never depend directly on each other.

---

## Code Generator

Responsible only for:

```
Metadata Model

↓

Generated C++
```

The generator must assume that semantic validation has already completed.

The generator should never depend on parser-specific AST nodes.

---

# Layering

Dependencies must always flow downward.

```
Parser

↓

Semantic Analysis

↓

Metadata Model

↓

Generator
```

Never introduce circular dependencies.

Never merge multiple stages into one implementation for convenience.

## Metadata Model Is the Core

The Metadata Model is the Intermediate Representation (IR) of CJM.

Every parser produces it.

Every generator consumes it.

Never bypass the Metadata Model.

Parser implementations and generators must never directly depend on each other.

---

# Build Philosophy

CJM is CMake-first.

The preferred workflow is:

```
Write C++

↓

Run CMake

↓

Build

↓

Done
```

Users should never need to manually invoke internal tools during normal development.

---

# Task Execution Modes

Different tasks require different execution strategies.

Unless explicitly instructed otherwise, choose the smallest execution mode that satisfies the task.

---

## Mode: Fast Patch

Use this mode for:

- small bug fixes
- documentation updates
- typo fixes
- localized refactoring
- isolated test fixes

Behavior:

- inspect only directly relevant files
- do not perform repository-wide analysis
- do not redesign existing architecture
- make the smallest correct change
- run only targeted tests
- stop if architectural changes become necessary

Avoid:

- unrelated cleanup
- speculative improvements
- broad refactoring
- updating unrelated documentation

Goal:

Minimize turnaround time.

---

## Mode: Implementation

Use this mode for normal feature development.

Examples:

- implementing a GitHub Issue
- completing a roadmap milestone
- adding a new component already described by the design documents

Behavior:

Treat the following documents as authoritative:

- AGENTS.md
- ARCHITECTURE.md
- ROADMAP.md
- design documents under docs/design/

Do not redesign established architecture.

Implement the approved design faithfully.

Keep changes within the current Issue scope.

Run only relevant tests.

If implementation reveals an architectural conflict:

- explain the issue
- propose alternatives
- do not silently redesign the architecture

Goal:

Deliver production-quality implementation while preserving architectural consistency.

---

## Mode: Design Review

Use this mode only when explicitly requested or when a design document is being created or reviewed.

Behavior:

- do not write production code
- review architecture
- identify inconsistencies
- identify unnecessary abstractions
- identify missing requirements
- propose tradeoffs
- preserve v0.1 scope

Do not expand project scope merely because a more generic design is possible.

Goal:

Improve design quality before implementation.

---

## Mode: Architecture Evolution

Use this mode only when the maintainer explicitly requests an architectural redesign.

Behavior:

Architecture may evolve only when:

- implementation exposes structural problems
- long-term maintainability clearly improves
- existing design goals remain satisfied

Every architectural change should include:

- motivation
- tradeoff analysis
- impact on existing components
- migration strategy

Avoid architecture changes based solely on hypothetical future features.

Goal:

Evolve the architecture deliberately rather than continuously redesigning it.

---

## Repository Inspection Policy

Unless the task explicitly requires repository-wide analysis:

- inspect only files relevant to the current task
- avoid reading unrelated design documents
- avoid modifying unrelated files

Large repository reviews should be performed only for:

- architecture review
- design review
- major refactoring
- release preparation

This reduces unnecessary reasoning time while keeping implementations focused.

---

## General Principle

Prefer focused execution over exhaustive analysis.

Only increase reasoning scope when the task genuinely requires broader architectural consideration.


## Scope Discipline

For v0.1:

Do not introduce new abstractions solely for future extensibility.

When uncertain:

implement the smallest design that satisfies the approved v0.1 requirements.

Future capabilities should be added only when required by future milestones.

--- 

# Development Workflow

CJM follows a feature-branch workflow.

Major features should never be implemented directly on the `main` branch.

Recommended workflow:

```
Issue

↓

Feature Branch

↓

Implementation

↓

Pull Request

↓

AI Review

↓

Merge Commit

↓

Delete Feature Branch
```

For maintainer-led milestone work, each epic or release slice should use one
feature branch. Complete its child issues as focused commits on that branch.

Examples:

```
feature/metadata-model

feature/code-generator

feature/parser

feature/semantic-analysis

feature/cmake-integration

feature/v0.6.0-simdjson-decode-spike
```

---

## Pull Request Strategy

CJM preserves development history.

Merge commits are preferred over squash or rebase merges.

Reasons include:

- preserving feature development history
- improving long-term traceability
- simplifying future debugging and `git bisect`
- supporting future multi-contributor development
- documenting architectural evolution

The `main` branch represents the project's historical development, not only its final state.

---

## Commit Messages

Feature branch commits should remain clean and meaningful.

Avoid commit messages such as:

```
fix

update

tmp

again

oops
```

Prefer Conventional Commits:

```
feat(metadata): add type model

feat(parser): preserve source locations

test(generator): add golden tests

refactor(cli): simplify argument parsing

docs(architecture): update parser pipeline
```

Feature branches are expected to contain multiple logical commits.

Do not squash commits simply to reduce commit count.

---

## Pull Request Reviews

Before merging a Pull Request:

- verify architecture consistency
- verify layer separation
- verify documentation changes (if applicable)
- verify tests
- request AI-assisted review when appropriate

For this project, AI review is considered an important part of the development workflow.

Final architectural decisions remain the responsibility of the maintainer.

---

## Branch Lifecycle

After a Pull Request is merged:

- delete the feature branch
- close the related GitHub Issue
- update the corresponding Milestone automatically when applicable

The `main` branch should only contain reviewed, merge-based feature integration.

---

# Public API

The public API should remain small and stable.

Current user-facing interfaces include:

- `cjm_generate(...)`
- `cjm` CLI
- `cjm::` namespace
- `*.cjm.hpp` generated files

Implementation details should never leak into the public API.

---

## Recommended Development Order (v0.1)

To minimize architectural churn, major components should be implemented in the following order.

1. **Metadata Model**
   - Define the core Intermediate Representation (IR).
   - Keep it parser-independent and generator-independent.
   - This is the foundation of the entire project.

2. **Code Generator**
   - Consume the Metadata Model.
   - Produce deterministic, readable C++ code.
   - Do not depend on parser-specific data structures.

3. **CLI**
   - Establish the public command-line interface.
   - Keep the CLI thin; business logic belongs in library components.

4. **CMake Integration**
   - Integrate the CLI into normal CMake workflows.
   - Validate the complete build pipeline.

5. **Parser**
   - Parse standard C++ source.
   - Expose syntax information only.
   - Do not perform semantic validation.

6. **Semantic Analysis**
   - Transform parser output into the Metadata Model.
   - Validate metadata.
   - Resolve types and dependencies.

7. **End-to-End Example**
   - Demonstrate the complete workflow:
     Source → Parser → Semantic Analysis → Metadata Model → Generator → Generated C++.

Each stage should be functional and testable before moving to the next one.

Avoid implementing multiple major stages simultaneously.

---

# Parser Independence

The parser implementation is replaceable.

Current implementation choices do not define the product.

Avoid exposing parser-specific types outside the parser layer.

## Replaceable Components

Implementation technologies are replaceable.

Examples include:

- parser implementation
- AST representation
- JSON backend
- build integration internals

The public developer experience should remain stable even if internal implementations evolve.

---

# Backend Independence

The initial backend may target `nlohmann/json`.

However, CJM should not become permanently coupled to any third-party JSON library.

Long-term architecture should support a native backend.

## Native Backend Direction

The first implementation may target a third-party JSON library.

However, CJM should eventually provide a native backend to reduce dependency risk and strengthen long-term product stability.

The MVP should prioritize validating the build-time code generation pipeline before introducing a native backend.

---

# Generated Code

Generated code should be:

- deterministic
- readable
- debuggable
- inspectable

Generated code is considered part of the product.

Never intentionally generate unreadable code.

---

# Simplicity

Prefer:

- simple implementations
- explicit ownership
- deterministic behavior
- maintainable code

Avoid:

- unnecessary templates
- hidden magic
- global state
- premature optimization
- parser-specific abstractions

---

# Documentation

Architecture documentation is part of the implementation.

When architectural changes are introduced:

1. Update the relevant design documents.
2. Then update the implementation.

Documentation should remain consistent with the codebase.

## Design Documents Are Source of Truth

Design documents define the intended architecture.

If implementation and documentation disagree:

- verify whether the implementation is incomplete, or
- update the documentation before changing architecture.

Do not silently diverge from the documented design.

---

# Testing

Every subsystem should be testable independently.

Suggested layers:

- parser tests
- semantic analysis tests
- metadata model tests
- generator tests
- integration tests
- golden tests

Golden tests are preferred for generated output.

---

# Long-Term Vision

CJM is intended to become a production-quality developer tool.

Short-term implementation convenience should never compromise:

- architecture
- maintainability
- portability
- developer experience

When in doubt, optimize for long-term engineering quality.

---

# Decision Checklist

Before implementing any non-trivial feature, verify:

- Does it preserve the documented architecture?
- Does it respect layer separation?
- Does it improve the developer experience?
- Does it keep the Metadata Model independent?
- Does it preserve parser independence?
- Does it keep generated code readable?
- Does it follow "Standard C++ in. Standard C++ out."?

If any answer is "No", redesign before implementing.

---

## Long-Term History

Git history is considered part of the project's engineering documentation.

Preserve meaningful commit history whenever practical.

Future contributors should be able to understand how major architectural decisions evolved by reading the commit history.


## Learning-Oriented Development Protocol

CJM is both a production-oriented developer tool and a learning project.

Code changes must optimize for correctness, maintainability, and the maintainer's
understanding. Completing an issue quickly is not sufficient if the maintainer
cannot explain the resulting design without reading the implementation.

### 1. Preserve maintainer ownership of code

During guided development, the maintainer writes and applies production code,
tests, and CMake changes. The AI assistant must not edit these files unless the
maintainer explicitly asks it to perform the implementation.

The assistant may implement documentation tasks assigned to it. After a larger
documentation change, summarize the decisions and contracts that the maintainer
needs to understand instead of requiring a line-by-line reading of repetitive
explanatory text.

When code is requested for manual application, present it as a focused `git diff`
with exact file paths and enough hunk context to locate the change. Explain the
purpose of the change before showing the diff. A complete implementation may be
shown for complex or unfamiliar code only after the maintainer requests it.

### 2. Use one branch per epic or release slice

State the current branch before beginning a task. Child issues belonging to the
same epic or release slice normally share one feature branch and are separated by
focused commits rather than additional short-lived branches.

End each completed coding step with one proposed Conventional Commit message.
Commit messages must describe the project change and must not contain assistant
branding or identity.

### 3. Follow the guided development loop

Use this order for every non-trivial change:

1. explain the problem and the existing architecture;
2. present the meaningful design options and tradeoffs;
3. let the maintainer decide unresolved semantics;
4. define tests and invariants;
5. implement one small slice;
6. explain what the diff proves and changes;
7. confirm understanding before entering the next slice.

Do not skip directly from a requirement to a large implementation.

### 4. State the task contract before code

Before each task, state:

1. which architectural layer owns the change;
2. why that layer owns it;
3. which existing invariants could be broken;
4. how the change will be verified;
5. which assumptions or semantics remain undecided;
6. the exact files and functions the maintainer will touch.

Instructions must use concrete actions such as add, remove, rename, or replace.
Avoid ambiguous directions such as "keep or adjust".

### 5. Require understanding before completing public abstractions

The AI assistant must not advance a public abstraction to a complete
implementation while the maintainer cannot explain its purpose and contract
without code.

Before implementation, the maintainer should be able to explain:

* what problem the abstraction solves;
* which layer owns it;
* its inputs and outputs;
* the invariants it establishes;
* why a simpler alternative is insufficient.

If these points are unclear, remain in design discussion or implement only a
small experimental slice.

### 6. Keep issues and commits cognitively small

Each commit should introduce one primary concept or behavior. Avoid combining
metadata parsing, symbol lookup, type resolution, diagnostics, and generator
behavior in one commit unless they are inseparable.

Large issues must be decomposed into ordered child issues or commits. Each step
must build naturally on the previous step and must not silently include adjacent
roadmap features.

### 7. Design top-down and implement bottom-up

Explain the high-level workflow first. Then identify the small functions that
represent its stages and implement one stage at a time.

Orchestration functions should expose important stages with numbered comments
and delegate details to focused helpers. A function comment should state only
what the function does. Do not fill comments with unrelated responsibilities or
lists of things the function does not do.

### 8. Give precise implementation guidance

Before asking the maintainer to edit code, identify:

* the exact file;
* the existing function that is the entry point;
* every function to add or change;
* the purpose of each function;
* the order in which the edits should be made;
* the focused test that validates the slice.

For a simple learning exercise, provide function names, signatures when needed,
and purpose comments while leaving implementation bodies to the maintainer. For
complex or unfamiliar library integration, provide a complete `git diff` when
the maintainer asks for one, but still explain each API used.

### 9. Make tests focused and independently runnable

Each test case should prove one function contract or one observable behavior.
Test names should identify the function and scenario, for example
`generate_header.required_integer`.

CTest remains the repository-wide test runner. New C++ unit tests should use the
approved test framework and register individual cases with CTest. Existing
standalone `main()` and `assert`-based tests remain valid CTest tests and must not
be migrated as part of unrelated feature work.

Migrate legacy tests in a dedicated stabilization or release-closing task after
the new framework has proved reliable. Until then, the legacy and framework-based
test executables are expected to coexist.

The test infrastructure should allow the maintainer to run one exact test case
without executing unrelated unit tests. Use targeted tests during each slice and
run broader suites only at integration or completion boundaries.

Golden tests must report useful mismatch context. A failure should identify the
relevant line or difference instead of printing only a boolean equality failure.
Do not expose private production helpers solely to test them; test them through
their owning public or module-level contract unless they have an independent
stable contract.

### 10. Explain what tests actually prove

Tests must be written around approved invariants rather than implementation
incidents. For each test, explain:

* the behavior being exercised;
* the invariant it proves;
* why the assertion is sufficient;
* which nearby cases remain unsupported or untested.

Prefer focused tests before combined integration tests. A large integration test
must not be the first explanation of several new concepts.

### 11. Use a completion gate

Before moving to the next issue, the maintainer should be able to explain:

* why the chosen design belongs in its layer;
* which alternatives were considered;
* which new invariant was added;
* which cases are explicitly unsupported;
* what the tests genuinely prove.

If any answer remains unclear, explain the current slice again or reduce it
further. Do not continue merely because the tests pass.

### 12. Definition of a good commit

A good CJM commit should answer all of the following clearly:

* What single behavior was added?
* Why does it belong in this layer?
* What are its inputs and outputs?
* Which function or module owns it?
* Which invariant does it add or preserve?
* How can its focused test be run?
* What does the test prove?
* What remains intentionally unsupported?
* What should the next commit build on?
