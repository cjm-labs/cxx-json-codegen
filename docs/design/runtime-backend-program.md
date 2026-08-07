# Runtime Backend Program

This document records the agreed post-v0.5 runtime backend plan.

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

v0.5.2 completed recursive schema coverage and documented the runtime semantic,
decode-error, conformance-fixture, and static-selection foundation. That
foundation defines the experiment boundary; it does not implement a
high-performance runtime backend.

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

- completed through v0.5.2

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

The program should be tracked as a broad direction, but implemented as small
epics. Its completed foundation epic was:

```text
v0.6 Foundation - Runtime Semantics and Conformance
```

That foundation defines what CJM means by JSON runtime behavior; it does not
implement a high-performance backend. The current v0.6.0 work is the first
limited simdjson scalar decode spike built on that foundation.

Public releases should represent user-consumable capability snapshots, such as:

```text
v0.5.2
    recursive schema coverage and runtime foundation

v0.6.0
    limited simdjson On-Demand scalar decode spike

later v0.6 releases
    generated-codec vertical slices, native baselines, encode, or promotion
    only when the corresponding evidence is complete
```

The exact tags may change. No release number is reserved for Glaze, yyjson, or
any other backend before its experiment justifies a user-consumable result.

---

# v0.6 Work Packages

## Work Package A - Runtime Semantics and Conformance Foundation

Status: completed in v0.5.2.

It is deliberately documentation- and test-shape-heavy. It does not introduce
simdjson, Glaze, yyjson, or a universal runtime facade.

Define:

- backend-neutral runtime JSON semantic profile
- minimal decode error and path model
- backend taxonomy
- capability matrix as documentation and conformance expectations
- conformance fixture layout with core and optional strict capabilities
- static backend selection shape for CLI and CMake
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

The foundation epic produced these work items:

```text
1. docs(runtime): define runtime JSON semantic profile
2. docs(runtime): define decode error and path model
3. test(runtime): sketch conformance fixture layout
4. test(mapping): verify multiline declarations and recursive type closure
5. docs(runtime): define static backend selection shape
```

The runtime semantic profile lives in
[Runtime JSON Semantic Profile](runtime-json-semantic-profile.md).

The decode error shape lives in
[Runtime Decode Error Model](runtime-decode-error-model.md).

The conformance fixture shape lives in
[Runtime Conformance Fixture Layout](runtime-conformance-fixtures.md).

The static backend selection shape lives in
[Static Backend Selection](static-backend-selection.md).

## Work Package B - simdjson On-Demand Decode Spike

Prove feasibility with the smallest generated model subset.

Initial scope:

- one root object
- required `bool`, signed integer, and unsigned integer fields
- presence bits
- one-pass object dispatch
- unknown-field policy
- root-field decode error and path propagation
- parser/input lifetime wrapper shape

This spike does not make simdjson an official backend.

The spike boundary lives in
[simdjson On-Demand Decode Spike](simdjson-ondemand-decode-spike.md).
Floating point, strings, optionals, containers, enums, and nested objects remain
outside this spike. They belong to later vertical slices after the scalar
control flow and error translation are understood.

## Work Package C - simdjson Native Baselines And Generated Vertical Slice

Before broader generated-codec claims, compare the generated path with the
strongest relevant native paths in the pinned simdjson release.

Candidate baselines:

- handwritten On-Demand traversal
- pre-C++20 `get<T>` specialization
- C++20 `tag_invoke` customization
- experimental C++20 `simdjson::from`, when useful as a labeled comparison
- C++26 static reflection when a suitable compiler is available

The first meaningful generated vertical slice should cover:

- required unsigned integer and string fields
- one optional integer
- one nested generated object
- arbitrary input field order
- presence tracking
- owned strings
- unknown-field policy
- missing, null, type, and nested-path failures
- new-object output guarantee
- generated compile and runtime tests

Different C++ standards and semantic options must be reported explicitly. This
work package tests whether CJM adds value over native typed conversion; it does
not assume that simdjson lacks model binding.

## Work Package D - simdjson Decode MVP

Expand only after the generated vertical slice is understood.

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

## Work Package E - simdjson Builder / Encode Spike

Keep simdjson decode and encode work contiguous.

Prove:

- deterministic object field order
- string escaping through runtime primitives
- enum string encode
- vector and array writing
- nested object writing
- optional omission behavior
- output buffer ownership

## Work Package F - simdjson Experimental Backend

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
- native baseline comparison is recorded
- runtime API use and version coupling are documented

## Work Package G1 - Glaze Metadata Adapter Evaluation

Evaluate CJM-generated Glaze metadata as an optional adapter backend.

The evaluation should answer:

- how CJM-generated metadata maps to Glaze customization points
- how rename, ignore, enum strings, optional, and unknown-field policy align
- how C++23 requirements stay isolated to selected backend targets
- whether generated metadata removes meaningful user maintenance
- what compile-time reflection, lookup-table, and typed-engine work remains

Glaze must not raise CJM core or nlohmann users to C++23.

## Work Package G2 - Glaze Generated Custom Codec Evaluation

Evaluate generated custom serialization separately from metadata generation.

The comparison should distinguish:

- Glaze automatic reflection
- handwritten explicit `glz::meta<T>`
- CJM-generated `glz::meta<T>`
- handwritten custom Glaze codec
- CJM-generated custom Glaze codec

Before generator work, classify every Glaze API used as documented public,
documented extension, public-header helper, or internal detail. The generated
codec experiment should stop if the public customization surface is
insufficient, native metadata already provides equivalent value, or upgrade
cost is not bounded.

Keeping only the metadata adapter is a valid outcome.

## Work Package H - yyjson Compact-DOM Evaluation

Evaluate yyjson as a compact document / DOM backend or control group.

yyjson should not be described as no-DOM.

The evaluation should answer:

- whether a compact DOM path has enough product value
- whether generated binding meaningfully improves over handwritten yyjson binding
- whether yyjson helps future C frontend work
- what ownership and string lifetime rules are required

## Work Package I - Backend Comparison Report

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

# Recursive Composition And Test Risk

Generated type handling should recurse through the existing Metadata IR
constructors rather than add emitters for individual type combinations.

Implementation can therefore remain compositional while semantic tests still
form a large cross-product. Optional, container, nested-object, null, missing,
wrong-type, cleanup, and structured-path behavior must be expanded through
constructor tests, pairwise combinations, selected deep stress cases, failure
corpora, and differential tests rather than one immediate Cartesian product.

---

# Runtime Semantic Profile

The first runtime backend must not invent backend-local semantics.

The normative v0.6 foundation profile is defined in
[Runtime JSON Semantic Profile](runtime-json-semantic-profile.md).

The simdjson spike and every later backend must consume the minimum portable
runtime profile for:

- missing required field
- missing optional field
- missing defaulted field, reserved for future metadata
- explicit `null` for optional fields
- explicit `null` for non-optional fields
- unknown fields
- duplicate keys
- numeric overflow
- floating-point range and malformed number behavior
- invalid enum string
- JSON type mismatch
- fixed-array extent mismatch
- trailing content
- nested error path
- partial output after decode failure

Presence and nullability must remain separate concepts.

Initial policy:

```text
ordinary non-optional field
    presence: required
    nullability: null forbidden

std::optional<T>
    presence: missing allowed
    nullability: null maps to disengaged optional

default metadata
    future feature

explicit required/optional override
    future feature
```

This is a current runtime policy, not a permanent binding between C++ type shape
and all future metadata semantics.

Some behaviors are core errors. Others are capabilities or policies.

For v0.6, duplicate-key handling should not be forced into the core profile until
each backend's parse path can actually observe duplicates. The existing
nlohmann/json DOM binding may not be able to reject duplicates from inside
generated `from_json` after parsing has already completed.

Capability examples:

```text
core
    missing required field fails
    null for non-nullable field fails
    integer overflow fails
    invalid enum string fails

optional strict capability
    duplicate key detection
    strict unknown-field rejection
    transactional decode guarantee
```

v0.7 may enrich and stabilize diagnostics, but v0.6 must define enough behavior
for runtime backends to conform.

---

# Decode Error Model

Runtime backends should not return unrelated backend-local error strings as the
only public failure surface.

The normative v0.6 foundation error shape is defined in
[Runtime Decode Error Model](runtime-decode-error-model.md).

v0.6 should define a minimal backend-neutral error model:

```text
decode_error_code
structured path
expected type
actual type, when knowable
runtime-specific detail, optional and non-semantic
```

The path model should be structured rather than only a formatted string:

```text
field segment
index segment
```

This lets a nested decoder report a child error and let the parent prepend its
own field segment without losing structure.

The exact C++ ABI does not need to freeze in the first foundation epic, but the
semantic shape must be defined before backend code starts returning errors.

---

# Decode Output Guarantee

The default public decode shape should prefer returning a new object:

```cpp
auto result = cjm::decode<User>(json_input);
```

This avoids exposing a partially modified caller-owned object after failure.

An in-place API such as:

```cpp
cjm::decode_into(json_input, existing_user);
```

may be added later, but it must advertise its output guarantee explicitly.

Possible guarantees:

```text
strong
    failure leaves the target object unchanged

basic
    failure leaves the target object valid but possibly partially updated

new-object
    failure produces no user object
```

The first simdjson spike should not silently choose an in-place partial-update
model merely because it is easiest to implement.

---

# Backend Priority

The agreed priority is:

```text
1. v0.5.x semantic foundation
2. runtime JSON semantic profile
3. minimal decode error and path model
4. conformance fixture skeleton
5. static backend selection shape
6. simdjson On-Demand decode spike
7. simdjson native baselines and generated vertical slice
8. simdjson decode MVP
9. simdjson builder / encode spike
10. simdjson experimental backend evidence review
11. Glaze metadata adapter evaluation
12. Glaze generated custom codec evaluation, only if justified
13. yyjson compact-DOM evaluation
14. backend comparison and promotion report
```

Rationale:

- simdjson-first best validates CJM's generated-codec differentiation while its
  native typed paths provide fair comparison baselines
- decode and encode should stay contiguous to preserve implementation context
- Glaze metadata and custom-codec paths answer different product questions
- yyjson is valuable but represents a compact document / DOM model
- runtime candidates should be compared after shared semantics and conformance
  exist

---

# Non-Goals

Do not add:

- a universal `JsonRuntime` facade
- runtime backend selection through dynamic runtime polymorphism
- a second type algebra parallel to Metadata IR
- mandatory optional runtime dependencies
- C++23 requirements for CJM core or nlohmann users
- native scanner, parser, formatter, or generic DOM code in CJM
- all runtime candidates in one PR
- performance claims without fair benchmark methodology
- claims that generated codecs outperform native runtime binding without
  equivalent semantics and toolchain labels
- new STL container mappings merely to expand surface area

The best near-term work is to make the existing Metadata IR semantics explicit,
portable, and testable across backend boundaries.
