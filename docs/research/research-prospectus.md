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

Formal experimentation or paper writing should not be treated as the default
future path. It should wait for technical maturity and explicit go/no-go review,
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

# Publication Is Conditional

This prospectus defines a possible research direction, not a commitment to
produce a paper.

CJM may produce several valid outcomes.

## Outcome A - Product Evidence Is Useful, but Research Novelty Is Weak

Possible result:

```text
IR improves local architecture
Tree-sitter reduces parser maintenance
the product works well
but no broadly generalizable research result emerges
```

Recommended response:

```text
continue product development
publish engineering documentation or a technical article
do not force a research paper
```

This is a successful product outcome.

## Outcome B - Results Support a Tool or Experience Report

Possible result:

```text
CJM is mature and reproducible
the architecture is useful
evaluation is credible
but the contribution is primarily a tool or engineering experience
```

Possible outputs:

```text
tool paper
demo paper
experience report
workshop paper
technical report
artifact publication
```

This is a valid research outcome and should not be treated as a failed
top-venue attempt.

## Outcome C - Strong Generalizable Research Findings Emerge

Examples may include:

```text
systematic silent-acceptance failures in naive error-recovering-parser adapters

a reusable fail-closed adapter method with significant empirical benefit

strong and measurable reduction in duplicated semantic rules and cross-layer
coupling through the shared Metadata IR

new evidence about which C/C++ metadata semantics can and cannot be normalized
across frontends

a corpus or benchmark independently useful to other source-tool researchers
```

Only this kind of evidence justifies considering a high-cost full research
study and a top-tier venue.

These results do not currently exist.

---

# Research Investment Gates

Research investment should progress through explicit gates.

## Gate 0 - Evidence Preservation

Status:

```text
Active now
```

Work includes:

- recording exact releases and commits;
- recording frontend and backend evolution;
- preserving fail-closed regression cases;
- recording downstream dogfood evidence;
- recording external bug reports;
- preserving architecture decisions;
- distinguishing observations from claims.

Properties:

```text
low incremental cost
high product value
high option value
no publication commitment
```

This work should continue as part of normal product development.

## Gate 1 - Natural Product Maturity

A research pilot should not begin until the product naturally provides enough
evidence.

Possible prerequisites:

- stable production C++ frontend;
- documented supported subset;
- stable Metadata IR for the evaluated release;
- reliable diagnostics;
- deterministic generated output;
- downstream use;
- a second frontend developed for product reasons;
- a second backend developed for product reasons.

The second frontend and backend must not be implemented solely to unlock a
paper.

## Gate 2 - Bounded Research Pilot

Before building a large corpus or multiple production-quality baselines, run a
time-boxed pilot.

Possible pilot questions:

```text
Does a naive Tree-sitter adapter exhibit meaningful false acceptance?

Does the strict adapter materially reduce high-severity failures?

Can frontend/backend extension cost be measured reproducibly?

Does the Metadata IR reduce duplicated mapping logic in practice?

Are the results large and consistent enough to justify deeper evaluation?
```

A bounded pilot budget should be proposed when this gate is reached. CJM should
not pre-commit hundreds of hours before pilot evidence exists.

The pilot must have explicit stop criteria.

## Gate 3 - Research Signal Review

After the pilot, classify the evidence:

```text
Weak:
expected engineering observations only

Moderate:
credible tool-level contribution

Strong:
non-trivial, generalizable, empirically measurable result
```

The review must answer:

- Is the result surprising or non-obvious?
- Is it useful outside CJM?
- Can it be measured reproducibly?
- Does it survive meaningful baselines?
- Is the effect large enough to justify a full study?
- Can the corpus and ground truth be made credible?
- Is there a coherent single paper thesis?
- Does an independent researcher find the question publishable?

No full research investment should begin before this review.

## Gate 4 - External Sanity Review

Before committing to a full study, obtain feedback from at least one person with
relevant research experience in:

```text
compilers
programming languages
source-code tooling
empirical software engineering
software architecture
```

The purpose is not to request endorsement.

The purpose is to test:

- novelty;
- related-work positioning;
- experimental validity;
- likely reviewer objections;
- whether the contribution is a research result or mainly an engineering
  artifact.

External review does not prove that a paper will be accepted.

## Gate 5 - Full Study Commitment

Only after Gates 1 through 4 pass should the owner decide whether to invest in:

- full corpus construction;
- independent labeling;
- Clang and other baselines;
- repeated measurements;
- statistical analysis;
- research artifact repository;
- manuscript preparation;
- formal submission.

At this gate, produce a fresh cost estimate and expected benefits.

The decision may still be:

```text
do not proceed
```

That is an acceptable outcome.

## Gate 6 - Venue Selection

Select a publication category only after the contribution and evidence are
known.

Possible categories include:

```text
technical report
workshop
tool/demo track
experience report
research conference
research journal
```

Do not optimize the product or evaluation for a named venue in advance.

Do not describe a top journal as the default destination.

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
