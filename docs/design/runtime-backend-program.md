# Runtime Backend Program

This document records the agreed post-v0.5.1 backend plan.

The plan is intentionally not a promise to implement every candidate backend.
It defines the order in which CJM should recover ownership of runtime semantics,
backend conformance, and high-performance JSON integration.

---

# Strategic Direction

v0.5.0 proved that Metadata IR can feed more than one output backend:

```text
C++ source
    |
    v
Parser / Semantic Analysis
    |
    v
Metadata IR
    +--> nlohmann/json generated integration
    +--> generated model contract
    +--> JSON Schema
```

v0.5.1 then froze default field mapping and canonical field participation
semantics for future runtime backends.

The next step is not to add many JSON runtimes at once.

The next step is:

```text
freeze shared field semantics
    |
    v
define runtime semantic profile and conformance
    |
    v
prove one high-performance generated-codec path
    |
    v
compare other runtime integration strategies
```

---

# Version Shape

## v0.5.x - Semantic Foundation

Status:

- completed in v0.5.1

Purpose:

- default field mapping
- managed model boundary
- explicit ignored-field semantics
- effective JSON field-name normalization
- duplicate effective-name diagnostics
- fail-closed unsupported included fields

This phase must happen before runtime backend work because every runtime backend
needs the same answers for field names, ignored fields, and participation.

## v0.6 - Runtime Backend Program

Purpose:

- canonical runtime JSON semantic profile
- backend taxonomy
- capability matrix as specification and test matrix
- conformance fixture skeleton
- simdjson-first generated-codec work
- later runtime backend comparison

v0.6 is a program. Its internal work packages are not automatically release
tags.

Public releases should represent user-consumable capability snapshots, such as:

```text
v0.5.1
    default field mapping and canonical field semantics

v0.6.0
    experimental simdjson backend with decode, encode, conformance, and docs

v0.6.1
    optional Glaze backend, if mature

v0.6.2
    yyjson evaluation or compact-DOM experiment, if useful
```

The exact tags may change. The dependency order should not.

---

# v0.6 Work Packages

## Work Package A - Runtime Semantics and Conformance Foundation

Define:

- backend-neutral runtime semantic profile
- backend taxonomy
- capability matrix as documentation
- conformance fixture layout
- backend selection shape for CLI and CMake
- optional dependency policy
- C++ standard isolation rules
- unsupported capability diagnostics

Do not introduce a second type algebra parallel to Metadata IR.

Instead, define the canonical JSON semantics of the existing `FieldType` model:

```text
Bool
SignedInteger
UnsignedInteger
FloatingPoint
String
Enum
Array
Vector
Map
Optional
UserDefined
```

and its recursive `arguments`.

## Work Package B - simdjson On-Demand Decode Spike

Prove feasibility with the smallest useful model subset.

Initial scope:

- scalar fields
- strings
- required fields
- unknown-field policy
- generated field dispatch
- basic error propagation
- parser/input lifetime wrapper shape

This spike does not make simdjson an official backend.

## Work Package C - simdjson Decode MVP

Expand only after the spike is understood.

Candidate scope:

- enum string decode
- optional fields
- vectors
- nested generated objects
- fixed arrays if the consumption model is clear
- source-independent JSON path or field context
- partial-output policy

Decode-only work may be useful internally, but it must not be marketed as a
complete JSON backend unless clearly labeled experimental and decode-only.

## Work Package D - simdjson Builder / Encode Spike

Keep simdjson decode and encode work contiguous.

Prove:

- deterministic object field order
- string escaping through runtime primitives
- enum string encode
- vector and array writing
- nested object writing
- optional omission behavior
- output buffer ownership

## Work Package E - simdjson Experimental Backend

Promote only after decode and encode have both passed the limited conformance
subset.

Expected evidence:

- generated code compiles
- generated decode tests pass
- generated encode tests pass
- round-trip tests pass
- unsupported combinations fail at generation time
- lifecycle constraints are documented
- user-facing API is clearly experimental

## Work Package F - Glaze Adapter Spike

Evaluate Glaze as an optional direct-typed adapter backend.

The spike should answer:

- how CJM-generated metadata maps to Glaze customization points
- how rename, ignore, enum strings, optional, and unknown-field policy align
- how C++23 requirements stay isolated to selected backend targets
- whether CJM adds value beyond raw Glaze metadata

Glaze must not raise CJM core or nlohmann users to C++23.

## Work Package G - yyjson Compact-DOM Evaluation

Evaluate yyjson as a compact document / DOM backend or control group.

yyjson should not be described as no-DOM.

The evaluation should answer:

- whether a compact DOM path has enough product value
- whether generated binding meaningfully improves over handwritten yyjson binding
- whether yyjson helps future C frontend work
- what ownership and string lifetime rules are required

## Work Package H - Backend Comparison Report

Classify each candidate:

- official
- optional official
- experimental
- research-only
- deferred

Promotion must consider:

- semantic parity
- correctness tests
- malformed-input behavior
- dependency and toolchain cost
- generated code readability
- diagnostics
- performance evidence
- build-cost evidence
- downstream use
- maintenance cost

Performance alone is insufficient.

---

# Runtime Semantic Profile

The first runtime backend must not invent backend-local semantics.

Before simdjson reaches MVP status, CJM should define the minimum portable
runtime profile for:

- missing required field
- missing optional field
- explicit `null` for optional fields
- explicit `null` for non-optional fields
- unknown fields
- duplicate keys
- numeric overflow
- invalid enum string
- fixed-array extent mismatch
- trailing content
- nested error path
- partial output after decode failure

v0.7 may enrich and stabilize diagnostics, but v0.6 must define enough behavior
for runtime backends to conform.

---

# Backend Priority

The agreed priority is:

```text
1. v0.5.x semantic foundation
2. v0.6 conformance foundation
3. simdjson On-Demand decode
4. simdjson builder / encode
5. simdjson experimental backend
6. Glaze adapter spike
7. yyjson compact-DOM evaluation
8. backend comparison and promotion report
```

Rationale:

- simdjson-first best validates CJM's generated-codec differentiation
- decode and encode should stay contiguous to preserve implementation context
- Glaze is valuable but represents a different direct-typed adapter model
- yyjson is valuable but represents a compact document / DOM model
- runtime candidates should be compared after shared semantics and conformance
  exist

---

# Non-Goals

Do not add:

- a universal `JsonRuntime` facade
- a second type algebra parallel to Metadata IR
- mandatory optional runtime dependencies
- C++23 requirements for CJM core or nlohmann users
- native scanner, parser, formatter, or generic DOM code in CJM
- all runtime candidates in one PR
- performance claims without fair benchmark methodology
- new STL container mappings merely to expand surface area

The best near-term work is to make the existing Metadata IR semantics explicit,
portable, and testable across backend boundaries.
