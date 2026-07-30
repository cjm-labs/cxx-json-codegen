# High-Performance JSON Strategy

CJM is a build-time metadata compiler.

Its product center is:

```text
Source model
    |
    v
Frontend
    |
    v
Semantic Analysis
    |
    v
Metadata IR
    |
    v
Backend-specific generated artifacts
```

High-performance JSON support is a backend strategy, not a change to CJM's
identity.

The Metadata IR remains the boundary between source-language understanding and
backend-specific output.

---

# Strategic Decision

CJM should support high-performance JSON through multiple optional backend
strategies.

The CJM repository should not, by default, own a full experimental
high-performance JSON engine.

The governing boundary is:

> CJM owns model knowledge. A JSON runtime owns generic JSON primitives.

For a possible future native engine, the current working repository name is:

```text
cjm-labs/cjm-json
```

This name is provisional. The repository does not exist as part of this
decision, and this document does not create or commit to it.

---

# Responsibility Boundary

## CJM Responsibilities

CJM owns:

- source frontends
- semantic analysis
- Metadata IR
- backend selection
- model-specific generated code
- generated model contracts
- JSON Schema generation
- source-aware diagnostics

Generated model-specific code knows:

- record names
- field names
- JSON keys
- field ordering
- field types
- optional semantics
- enum representation
- nested object relationships
- unknown-field policy when documented
- required and default policy when documented

Conceptual generated code might eventually look like:

```cpp
bool read_user(
    cjm::json::reader& reader,
    User& output,
    cjm::json::error& error);

void write_user(
    cjm::json::writer& writer,
    const User& value);
```

These examples are conceptual only. They do not define a public API.

## JSON Runtime Responsibilities

A runtime such as a possible `cjm-json` owns:

- JSON syntax primitives
- structural scanning
- output buffers
- writer primitives
- scanner or tokenizer logic
- scalar parsing and formatting
- string escaping and unescaping
- error positions
- depth and size limits
- allocation and buffer hooks
- optimized SIMD or SWAR kernels
- fuzzing
- differential testing
- performance benchmarks

It must not depend on:

- CJM parser nodes
- Tree-sitter nodes
- CJM semantic analysis classes
- Metadata IR implementation types
- CJM private generator headers
- CJM CMake internals

The intended dependency direction is:

```text
CJM Metadata IR
    |
    v
CJM backend
    |
    v
generated model-specific codec
    |
    v
public JSON runtime API
```

The dependency direction must not reverse.

---

# Avoid a DOM-First Native Identity

The initial native high-performance direction should not start as:

```cpp
cjm::json::value document = cjm::json::parse(input);
```

A full mutable DOM expands scope into:

- dynamic object storage
- iterators
- mutation
- JSON Pointer
- JSON Patch
- generic conversions
- allocator-aware trees
- arbitrary document editing

The stronger CJM-aligned direction is:

> Generated high-performance typed JSON codecs for known C and C++ models.

A future DOM may be explored only if independently justified.

---

# Backend Taxonomy

This taxonomy is directional. It is not a commitment to implement every
backend.

Each backend requires independent product justification.

## nlohmann/json Backend

Role:

- compatibility
- adoption
- C++17-friendly workflow
- readable generated integration
- mature ecosystem integration

Trade-off:

- generic DOM cost
- allocation cost
- not intended to be CJM's peak typed JSON throughput path

`nlohmann/json` remains CJM's first official backend. It should not be removed
merely because faster libraries exist.

## Glaze Backend

Conceptual flow:

```text
Metadata IR
    |
    v
generated Glaze metadata or adapter
    |
    v
Glaze read/write runtime
```

Potential value:

- high-performance typed JSON for Modern C++ projects
- reuse of a mature optimized implementation
- one CJM metadata source across multiple backends
- backend-independent rename, ignore, enum, optional, schema, and contract
  policies

CJM must not claim that it enables basic aggregate serialization in Glaze.
Glaze already provides that.

Before implementation, CJM would need a semantic mapping review for:

- rename
- ignore
- omitempty
- missing fields
- null
- required
- default
- unknown fields
- enum strings
- custom converters
- partial update behavior

Unsupported or mismatched semantics must fail closed.

## yyjson Backend

Conceptual flow:

```text
C++ model --\
             +--> generated integration --> yyjson runtime
C model  ---/
```

Potential value:

- mature high-performance C library
- possible shared runtime for future C and C++ frontends
- custom allocator support
- strong performance baseline

The exact C and C++ generated APIs may differ while sharing the same runtime
dependency.

yyjson is a future experiment, not a selected official backend.

## cjm-json Native Backend

Conceptual flow:

```text
Metadata IR
    |
    v
generated specialized codec
    |
    v
cjm-json runtime
```

Position:

- experimental
- model-aware
- typed
- generated
- performance-research oriented

It must not be described as production-ready, faster, safer, or superior to
Glaze or yyjson without evidence.

## JSON Schema Backend

The JSON Schema backend is language-neutral and independent of runtime JSON
library choice.

It should consume:

- Metadata IR, or
- stable generated model-contract data

It should not consume Glaze-specific, nlohmann-specific, yyjson-specific, or
cjm-json-specific implementation metadata.

---

# Mature Libraries and Native Research Can Coexist

A mature backend is a product choice.

A native backend is a performance research choice until proven otherwise.

CJM may support both.

Possible future matrix:

| Backend | Product Role |
| --- | --- |
| nlohmann/json | Compatibility and adoption |
| Glaze | High-performance Modern C++ |
| yyjson | High-performance C/C++ shared runtime |
| cjm-json | Native generated typed codec experiment |
| JSON Schema | Language-neutral schema artifact |
| Model contract | Downstream tooling artifact |

This matrix is not a checklist.

Performance is a first-class backend choice, but no candidate backend is
selected by this document.

---

# Possible cjm-json Development Sequence

This sequence is for a separate experimental project if the owner later creates
one.

It is not a CJM product roadmap requirement.

## Phase 0 - Research and Benchmark Design

No parser implementation.

Define:

- target workloads
- correctness requirements
- baseline libraries
- compiler and hardware matrix
- validation equivalence rules
- measurement methodology
- stop criteria

## Phase 1 - Direct Writer

Scope:

- generated model-specific JSON writer
- output buffer
- bool
- integers
- floating point
- strings
- escaping
- nested records
- arrays
- optional fields
- enums
- buffer reuse

Why start here:

- smaller state space
- easier correctness model
- easier differential testing
- clear performance measurement
- strong learning value

## Phase 2 - Generated Typed Binder over a Mature Parser

Conceptual flow:

```text
mature structural parser
    |
    v
generated field dispatch and typed assignment
    |
    v
C or C++ model
```

Purpose:

- measure generated typed binding before writing a native parser
- separate model-dispatch value from runtime parsing value

## Phase 3 - Native Scanner and Scalar Primitives

Possible scope:

- structural scanning
- whitespace
- strings
- escapes
- integers
- floating point
- nesting
- key dispatch

## Phase 4 - Native Typed Reader

Only begin after earlier stages show worthwhile performance potential.

Require:

- malformed-input correctness
- range checking
- UTF-8 policy
- unknown-field skipping
- depth limits
- size limits
- structured errors
- partial-object cleanup
- allocation policy

## Phase 5 - Stable Engine Evaluation

Only consider stable public release after:

- fuzzing
- sanitizers
- differential tests
- cross-platform CI
- public benchmark methodology
- stable error model
- documented supported JSON surface
- real downstream dogfood

---

# Benchmark Dimensions

Runtime metrics may include:

- write throughput
- read throughput
- roundtrip time
- p50 latency
- p95 latency
- p99 latency
- allocations
- bytes allocated
- branch misses where practical
- cache misses where practical
- instructions per byte
- cycles per byte
- binary size

Build-cost metrics may include:

- clean Debug build
- clean Release build
- incremental build
- peak compiler memory
- generated source size
- template-instantiation cost
- link time

Correctness equivalence must record whether each configuration validates:

- UTF-8
- number ranges
- invalid escapes
- malformed numbers
- unknown fields
- missing fields
- null
- duplicate keys
- nesting depth
- trailing content

A permissive fast path must not be compared against a strict validated path
without labeling the difference.

Possible baselines may include:

- handwritten nlohmann/json
- CJM-generated nlohmann/json
- Glaze
- reflect-cpp
- yyjson plus binding
- simdjson On-Demand plus binding
- RapidJSON
- future cjm-json

This document does not require every baseline before product work can continue.

---

# Correctness and Security Gates

A native engine must not become an official CJM backend solely because it is
fast.

Minimum gates include:

- documented supported JSON standard surface
- differential testing against mature libraries
- property-based tests
- fuzzing
- ASan
- UBSan
- integer overflow tests
- floating-point edge cases
- UTF-8 and escape tests
- deep nesting limits
- large-input limits
- malformed-input corpus
- stable error locations
- no partial output leakage
- failure cleanup
- cross-platform testing

For C decode support, also require:

- allocation ownership
- partial initialization cleanup
- failure rollback
- buffer capacity policy
- string lifetime
- sequence lifetime
- caller-owned versus runtime-owned memory

Performance never overrides correctness or explicit ownership.

---

# Integration Promotion Stages

## Stage A - External Experiment

Properties:

- separate repository
- no CJM dependency
- no official CJM support
- unstable APIs
- research and benchmark focus

## Stage B - Experimental CJM Backend

Promotion requires:

- public runtime API
- stable enough generated-code boundary
- fuzzing and sanitizers
- differential correctness
- bounded supported surface
- demonstrable product value
- optional integration
- no default-behavior changes

## Stage C - Official CJM Backend

Promotion requires:

- stable release
- compatibility policy
- documented support matrix
- cross-platform CI
- real downstream use
- maintainable annual cost
- clear benefit over mature alternatives

Creating a repository does not automatically trigger promotion.

---

# Version Relationship

Before CJM v1.0, CJM should continue its existing product roadmap.

Allowed:

- performance benchmark design
- backend architecture documentation
- small isolated backend experiments when explicitly approved

Not allowed by default:

- native parser implementation in the CJM repository
- making cjm-json a required dependency
- making cjm-json a v1.0 blocker
- changing default runtime behavior
- expanding the v1.0 mapping matrix solely for cjm-json

CJM v1.x may explore:

- Glaze backend experiments
- yyjson backend experiments
- cjm-json generated-code boundary experiments
- performance and build-time comparisons

These are evidence-driven options.

A future C direction must not depend on cjm-json being complete.

CJM v2.0 may still succeed with first-class C and C++ frontends, shared
Metadata IR, language-neutral schema output, and explicit backend performance
choices even if cjm-json remains experimental.

---

# Product Messaging Boundary

CJM should not be marketed as:

- the fastest JSON library
- a replacement for Glaze
- a replacement for yyjson
- a native JSON parser

A more accurate product principle is:

> One source model. One validated Metadata IR. Multiple integration and
> performance choices.

Do not add speculative performance claims without evidence.
