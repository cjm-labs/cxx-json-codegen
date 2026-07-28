# CJM Research Evaluation Plan

This document defines future evaluation methodology for CJM research.

It does not implement experiments, baselines, corpora, or benchmark harnesses.

---

# Evaluation Principles

Future evaluations should require:

- exact release and commit pinning;
- reproducible builds;
- immutable experiment configurations;
- raw-result preservation;
- negative-result preservation;
- separation of product code and baseline code;
- distinction between correctness, usability, and performance;
- documented hardware and environment for benchmark claims;
- no unsupported causal claims.

Product code remains the source of real engineering evidence. Research-only
baselines and corpora should stay isolated from production code.

---

# Corpus Design

Future parser and frontend studies should use a versioned corpus with explicit
ground truth labels.

Do not create the corpus in the product repository unless it remains small and
directly useful for product tests. Larger corpora should live in a future
research artifact repository.

## Valid and Supported

Examples should cover:

- scalar fields;
- strings;
- enums;
- aliases;
- namespaces;
- nested records;
- supported sequences;
- optionals;
- maps;
- supported combinations;
- valid metadata attachment.

## Valid but Unsupported

Examples may include:

- unsupported pointer models;
- multiple declarators;
- unsupported template declarations;
- inheritance;
- member pointers;
- function pointers;
- bit-fields;
- unsupported unions;
- unsupported access patterns;
- valid preprocessor-controlled models outside the supported contract.

## Malformed Source

Mutation categories may include:

- missing semicolon;
- missing `>`;
- unclosed string;
- unclosed comment;
- malformed namespace;
- malformed alias;
- truncated declaration;
- malformed attribute;
- parser-recovery boundaries.

## Ambiguous Metadata

Examples may include:

- metadata on the previous line;
- block-comment metadata;
- duplicated metadata;
- CJM-like misspellings;
- comment separated from a field;
- metadata after unsupported declarations;
- multiple comments on one line;
- strings containing `// json:`;
- comments inside initializers or macros.

## C-Specific Cases

Future C-focused categories may include:

- `typedef struct`;
- named and anonymous struct tags;
- fixed arrays;
- fixed string buffers;
- pointer ownership ambiguity;
- pointer-plus-length relationships;
- union and tagged-union boundaries.

These categories are planning notes only. They do not imply that the C frontend
exists today.

---

# Ground Truth

Each corpus case should be labeled independently from CJM's current behavior.

Candidate labels:

```text
supported
unsupported
malformed
ambiguous
outside managed surface
```

Ground truth sources may include:

- documented CJM syntax and mapping specifications;
- compiler validation where applicable;
- independent manual review;
- disagreement adjudication;
- immutable label files.

Ground truth should not be derived solely from what one CJM version accepts or
rejects.

---

# Parser and Frontend Baselines

Candidate baselines:

```text
A. handwritten bootstrap parser
B. naive Tree-sitter adapter
C. strict CJM Tree-sitter adapter
D. Clang AST extraction baseline
```

## A. Handwritten Bootstrap Parser

Intended to measure the behavior and limitations of CJM's original bootstrap
approach.

Not intended to prove that all handwritten parsers are unsafe.

## B. Naive Tree-sitter Adapter

Intended to measure plausible unsafe adapter behavior, such as accepting a
non-null syntax tree as success and extracting basic nodes without strict
fail-closed validation.

This baseline must remain research-only.

## C. Strict CJM Tree-sitter Adapter

Intended to measure CJM's production fail-closed adapter behavior for the
documented managed syntax surface.

It does not prove full C++ parser correctness.

## D. Clang AST Baseline

Intended to compare against compiler-grade source understanding with a different
dependency and integration profile.

Clang is expected to provide stronger compiler fidelity. The comparison should
study tradeoffs, not caricature the baseline.

---

# Correctness Metrics

Candidate metrics:

- false acceptance rate;
- false rejection rate;
- supported-case recall;
- unsupported-case rejection rate;
- malformed-case rejection rate;
- crash rate;
- hang or timeout rate;
- partial-output leakage;
- incorrect-field extraction;
- incorrect-comment attachment;
- incorrect-source-location rate.

False acceptance is the highest-severity parser outcome:

> A tool accepts a malformed or unsupported managed declaration and generates
> incomplete or incorrect integration code.

False rejection is still important, but it is usually less dangerous for CJM
than silent acceptance.

---

# Diagnostic Metrics

Diagnostic evaluation may measure:

- correct file;
- correct line;
- correct declaration;
- meaningful category;
- distinction between malformed and unsupported;
- actionable explanation;
- no output overwrite after failure.

Human evaluation of diagnostics should use a documented rubric. The rubric
should separate objective location accuracy from subjective message quality.

---

# Extensibility Study

For adding frontends and backends, record:

- added files;
- modified files;
- added source lines;
- modified source lines;
- reused IR kinds;
- reused semantic rules;
- duplicated logic;
- cross-layer dependencies;
- frontend-specific logic appearing in a backend;
- backend-specific logic appearing in a frontend;
- regressions in existing frontend/backend combinations.

The comparison should distinguish:

```text
shared-IR architecture
```

from:

```text
direct frontend-to-backend architecture
```

Do not assume `N + M` behavior merely because an IR exists. Measure actual
implementation effects.

---

# Competing Product Strategies

Compare with categories rather than weak versions of alternatives:

- direct source code generation;
- macro registration;
- compile-time reflection libraries;
- runtime reflection;
- Clang-based source extraction;
- schema-first generation.

Each approach's advantages should be acknowledged.

Examples:

- macro registration can be simple and explicit inside one codebase;
- Clang can provide compiler-grade source fidelity;
- schema-first generation can produce strong cross-language contracts;
- runtime reflection can enable dynamic behavior that CJM intentionally avoids.

CJM's goal is not to dominate all alternatives. It should define a useful
tradeoff for ordinary source-owned model declarations.

---

# Practicality Study

Future measurements may include:

- clean configure time;
- clean build time;
- incremental generation time;
- executable size;
- dependency size;
- runtime serialization and deserialization throughput;
- allocations;
- generated code size;
- generated code readability;
- onboarding steps;
- downstream CMake complexity;
- issue and user feedback.

Runtime performance should not be the sole or primary product criterion.
Correctness and developer experience remain primary CJM goals.

---

# Reproducibility

Future experiments should record:

```text
CJM version and commit
Tree-sitter runtime version
tree-sitter-cpp grammar version
compiler and version
CMake version
OS
hardware
backend dependency version
build options
corpus version
baseline versions
raw-result hashes
```

Raw results should be preserved even when they contradict the expected
hypothesis.

---

# Statistical and Reporting Policy

Where repeated measurements are used:

- report distributions, not only best values;
- report sample counts;
- report variance or confidence intervals where appropriate;
- preserve failures;
- avoid cherry-picking;
- distinguish exploratory and confirmatory experiments.

Avoid benchmark claims without enough environment detail for reproduction.

---

# Threats to Validity

Threats to track:

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
- operating-system and compiler differences;
- dependency-version effects;
- generalization beyond metadata code generation.

These threats should be reported with any future study, not only after positive
results.
