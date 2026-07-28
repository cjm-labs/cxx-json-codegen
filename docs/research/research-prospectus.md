# CJM Research Prospectus

Status:

```text
Exploratory research prospectus
Not a product roadmap
Not a publication claim
```

This document records candidate research directions that emerge from CJM's real
product development. It does not create product commitments.

---

# Product Context

CJM is a build-time metadata compiler for Modern C++.

Implemented on `main` at the time this prospectus was created:

- C++ source models remain ordinary user-owned C++.
- The production C++ frontend uses a Tree-sitter-backed adapter.
- Semantic Analysis builds a parser-independent Metadata IR.
- The implemented backend generates `nlohmann/json` C++ integration.
- Golden tests cover generated output.
- `ull-md-engine` has dogfooded CJM through the public `FetchContent` and
  `cjm_generate(...)` workflow.

Documented product direction:

- v0.4 explores a generated model contract for downstream tools.
- v0.5 plans JSON Schema output for supported mappings.
- Future frontends or backends may be added when product needs justify them.

Candidate future research evidence:

- a C frontend, if developed for product reasons;
- a JSON Schema backend, if developed for product reasons;
- reusable malformed and unsupported source corpora;
- baseline implementations isolated from production code;
- downstream adoption data beyond the initial dogfood report.

Planned C or JSON Schema work must not be described as implemented capability.
Research should study these features only when normal product development makes
them real.

---

# Problem Statement

C and C++ do not provide a uniform, ergonomic, portable metadata facility
comparable to mechanisms commonly available in languages such as Go, Rust,
Java, or C#.

Existing strategies include:

- handwritten integration code;
- macro registration;
- framework-specific DTOs;
- runtime reflection;
- compiler-specific extensions;
- Clang or LLVM tooling;
- schema-first generated models;
- direct frontend-to-backend generators.

Each strategy has advantages. CJM explores a different point in the design
space:

```text
ordinary user-owned source models
        |
        v
source frontend
        |
        v
semantic analysis
        |
        v
shared Metadata IR
        |
        v
independent backends
```

The research question is not whether CJM replaces all of these strategies. The
question is whether CJM's compiler-style architecture offers useful tradeoffs
for strongly typed, structured, modelable data integration.

---

# Research Hypothesis

Central hypothesis:

> CJM can use strict source-language adapters and a shared Metadata IR to
> separate source understanding from backend-specific integration generation,
> while preserving ordinary user-owned C and C++ models.

This is a hypothesis to evaluate, not an established conclusion.

Possible working titles:

```text
CJM: A Fail-Closed Multi-Frontend Metadata Compiler for C and C++
```

```text
Decoupling Source Models from Generated Integrations:
A Metadata Compiler Architecture for C and C++
```

These titles are provisional.

---

# Candidate Contributions

The following are candidate contributions. They are not yet proven
contributions.

1. A fail-closed frontend-adapter method for extracting managed declarations
   from ordinary C and C++ source.
2. A shared Metadata IR for supported C/C++ model semantics.
3. A multi-frontend and multi-backend metadata compiler architecture.
4. A malformed and unsupported C/C++ declaration corpus.
5. An empirical comparison against direct, macro-based, and Clang-based
   approaches.
6. A publicly reproducible open-source artifact.

The product does not need to implement a feature solely to complete this list.

---

# Research Questions

## RQ1 - Frontend Correctness and Safety

Can a strict, fail-closed frontend adapter built over an error-recovering parser
reliably distinguish supported, unsupported, and malformed source declarations?

Important categories:

```text
Supported and valid
Valid C/C++ but unsupported by CJM
Malformed C/C++
Ambiguous CJM metadata attachment
```

The most dangerous failure is false acceptance:

> CJM accepts a malformed or unsupported managed declaration and generates
> incomplete or incorrect integration code.

False acceptance should be treated as more severe than false rejection.

Candidate comparison implementations:

```text
A. Original handwritten bootstrap parser
B. Naive Tree-sitter adapter
C. Strict CJM Tree-sitter adapter
D. Clang AST extraction baseline
```

The naive Tree-sitter adapter is a research-only baseline. It should represent
a plausible unsafe design, such as accepting a non-null syntax tree as success
without enforcing CJM's full fail-closed contract.

## RQ2 - Frontend/Backend Extensibility

Does a shared Metadata IR reduce the engineering cost and coupling involved in
adding source-language frontends and output backends?

Possible future product evidence may include:

```text
Frontends:
- C++
- C

Backends:
- nlohmann/json
- JSON Schema
```

The C frontend and JSON Schema backend must be developed only when independently
justified by the product roadmap.

When those capabilities naturally exist, the study should evaluate whether CJM
behaves more like:

```text
N frontends + M backends
```

rather than:

```text
N x M direct frontend/backend integrations
```

This must be measured from implementation effects, not inferred from an
architecture diagram.

## RQ3 - Engineering Cost and Coupling

How does CJM's compiler-style architecture compare with direct code generation,
macro-based reflection, and Clang-based extraction in engineering cost,
coupling, and maintenance surface?

Candidate measurements:

- files added;
- files modified;
- source lines added;
- duplicated mapping rules;
- source-language checks appearing inside backends;
- backend-specific rules appearing inside frontends;
- dependency edges;
- cross-layer includes;
- regression failures;
- build configuration complexity;
- external dependency size;
- upgrade surface;
- diagnostic implementation duplication.

Developer wall-clock time should not be the primary metric because it is highly
affected by familiarity, learning effects, tool assistance, and working
conditions.

## RQ4 - Practicality

Is the architecture practical for real C and C++ projects?

Candidate dimensions:

- downstream setup complexity;
- CMake integration friction;
- generated-code readability;
- diagnostic usefulness;
- deterministic output;
- clean and incremental build behavior;
- build time;
- binary size;
- generation time;
- runtime serialization overhead;
- allocation behavior;
- model intrusiveness;
- source-of-truth preservation;
- external user feedback.

The study must not claim that CJM is universally faster or better than existing
solutions. The goal is to identify the tradeoff boundary.

---

# Required Product Maturity

Formal experimentation or paper writing should wait for technical maturity
rather than a version number alone.

Potential gates:

- production C++ frontend;
- documented supported source subset;
- stable public generation workflow;
- reliable diagnostics;
- deterministic generated output;
- downstream dogfooding;
- stable enough Metadata IR for evaluation;
- second frontend developed for product reasons;
- second backend developed for product reasons;
- versioned reproducible releases.

Not every gate is required for every research question. For example, RQ1 may be
studied before a second backend exists, while RQ2 requires meaningful
multi-frontend or multi-backend evidence.

---

# Primary and Secondary Paper Scope

Primary candidate direction:

> Evaluate whether a fail-closed, multi-frontend, multi-backend build-time
> metadata compiler can safely and economically decouple ordinary C and C++
> source models from independently developed integration backends through a
> shared Metadata IR.

Possible secondary direction:

```text
Fail-Closed Domain Adapters over Error-Recovering Parsers
```

Secondary question:

> How can a tolerant, error-recovering concrete-syntax parser be safely adapted
> for source-to-source compilers and code generators that must reject ambiguous
> or partially recovered managed declarations?

This should become an independent paper only if future evidence shows that the
adapter rules generalize beyond CJM and produce meaningful safety results.

---

# Non-Goals

The research effort is not intended to:

- prove that CJM supports full C++;
- replace general-purpose compilers;
- prove that Tree-sitter is compiler-equivalent;
- prove that CJM dominates every serialization tool;
- implement features only for publication;
- turn the product repository into a benchmark archive;
- make Java, Rust, Go, or unrelated language frontends a CJM goal;
- require a universal serialization runtime;
- convert product documentation into academic marketing.

---

# Preliminary Threats to Validity

Known threats include:

- project-author bias;
- author-built baselines;
- limited external users;
- corpus representativeness;
- C and C++ language-subset selection;
- unequal implementation maturity;
- dependence on backend behavior;
- learning effects;
- AI-assisted development effects;
- measurement noise;
- generalization beyond metadata code generation.

These threats should be expanded and revisited before any formal study.
