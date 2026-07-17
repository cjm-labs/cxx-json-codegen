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

Each feature branch should correspond to a single major task or GitHub Issue whenever practical.

Examples:

```
feature/metadata-model

feature/code-generator

feature/parser

feature/semantic-analysis

feature/cmake-integration
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

Code changes must optimize for correctness, maintainability, and human comprehension. Completing an issue quickly is not sufficient if the resulting implementation is difficult to understand incrementally.

### 1. Keep each issue and commit cognitively small

Each commit should introduce one primary concept or behavior.

A commit should normally be understandable without requiring the reader to absorb several unrelated mechanisms at once.

Avoid combining changes such as:

* metadata parsing;
* symbol-table construction;
* alias resolution;
* recursive type analysis;
* diagnostics;
* generator behavior;

in a single commit unless they are inseparable.

Large issues must be decomposed into ordered sub-issues or commits. Each step must build naturally on the previous step.

### 2. Explain the design before implementing it

Before producing code, describe:

1. the exact goal of the change;
2. why the change is needed;
3. the input and expected output;
4. the affected modules and files;
5. what is explicitly out of scope;
6. how this change connects to the previous and following commits.

Do not begin with a large implementation dump.

### 3. Design top-down and implement bottom-up

Start by defining the high-level workflow.

Example:

```text
Collect symbols
    ↓
Resolve field types
    ↓
Validate JSON metadata
    ↓
Build Metadata IR
```

Then define small functions representing the individual steps. Implement and test one step at a time.

Orchestration functions should show the algorithmic structure clearly and delegate details to focused helper functions.

### 4. Document non-trivial functions

Every non-trivial function must have a short documentation comment describing:

* its purpose;
* each important input parameter;
* its return value or output effect;
* its failure behavior;
* what responsibility does not belong to the function.

Example:

```cpp
/**
 * Resolve a parser field type spelling into the Metadata IR type model.
 *
 * Inputs:
 *   symbols        - Known declarations, enums, and aliases.
 *   namespace_path - Namespace containing the field declaration.
 *   field          - Parser syntax containing the original type spelling.
 *
 * Output:
 *   Returns a normalized FieldType on success.
 *   Returns an error diagnostic for unsupported or unresolved types.
 *
 * This function performs semantic mapping only. It does not generate C++ code.
 */
```

### 5. Show the implementation stages inside orchestration functions

Use numbered comments for important algorithmic stages.

Example:

```cpp
bool place_elephant_in_refrigerator(
    Refrigerator& refrigerator,
    const Elephant& elephant) {

    // 1. Validate that the refrigerator can contain the elephant.
    if (!can_contain(refrigerator, elephant)) {
        return false;
    }

    // 2. Open the refrigerator.
    refrigerator.open();

    // 3. Place the elephant inside.
    refrigerator.put_in(elephant);

    // 4. Restore the refrigerator to its closed state.
    refrigerator.close();

    return true;
}
```

Comments should reveal the structure of the algorithm. Do not comment every obvious statement.

### 6. Prefer focused functions and modules

Avoid multi-page functions that combine:

* string parsing;
* name lookup;
* symbol management;
* validation;
* diagnostics;
* IR construction.

Extract cohesive concepts into named functions or modules.

A high-level function should primarily coordinate operations. Detailed parsing, lookup, classification, and validation should live in focused components.

### 7. Make tests follow the same incremental structure

Each commit must contain focused tests for the behavior introduced in that commit.

Do not introduce a large integration test as the first explanation of several new concepts.

Recommended order:

1. data model test;
2. helper-function unit test;
3. single-feature semantic test;
4. combined integration test.

### 8. Preserve learning boundaries

When implementing a requested issue, do not silently add adjacent future features.

If an adjacent feature becomes necessary, stop and explain why. Propose a separate follow-up issue or commit.

Prefer an incomplete but well-defined step over a large speculative implementation.

### 9. Present code in reviewable increments

When guiding the developer interactively:

1. explain the current step;
2. show the function skeleton;
3. explain the skeleton;
4. implement one helper;
5. add its focused test;
6. summarize what was learned;
7. only then proceed to the next helper.

Do not present hundreds of lines of code before the developer has understood the design.

### 10. Definition of a good commit

A good CJM commit should answer all of the following clearly:

* What single behavior was added?
* Why does it belong in this layer?
* What are its inputs and outputs?
* Which function or module owns it?
* How is it tested?
* What remains intentionally unsupported?
* What should the next commit build on?
