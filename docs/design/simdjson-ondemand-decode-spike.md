# simdjson On-Demand Decode Spike

Status: completed as an internal experimental spike.

This document defines the first simdjson On-Demand decode spike for CJM and
records its outcome.

It remains the boundary for the merged implementation. It does not define a
public C++ ABI or promote simdjson to an official backend.

---

# Goal

The spike should prove the smallest useful generated-codec path:

```text
Metadata IR
    |
    v
generated scalar field dispatch
    |
    v
simdjson On-Demand traversal
    |
    v
complete C++ object or portable CJM decode error
```

The purpose is to learn whether CJM's normalized Metadata IR can drive a
forward-only runtime backend without changing parser, Semantic Analysis, or the
default nlohmann backend.

This spike is not a complete JSON backend.

---

# Outcome

The spike completed successfully. It proved that CJM Metadata IR can generate a
C++17 simdjson On-Demand decoder that:

- owns padded input for the duration of one decode
- iterates root object fields once regardless of their JSON order
- constructs a new strongly typed object
- decodes required booleans and signed, unsigned, and narrow integers
- rejects missing required fields, scalar type mismatches, integer overflow,
  and trailing content with portable CJM errors
- preserves root-field paths independently of simdjson diagnostic strings
- remains isolated behind an optional CMake dependency gate

Generated bool and integer headers compile and run, and both the default and
simdjson-enabled repository test configurations pass. The spike did not evaluate
runtime-native typed-conversion baselines or establish a performance result.

No public v0.6.0 release was produced. Public backend selection, broader type
coverage, encode, and an official support commitment remain later work.

---

# External simdjson Constraints

The selected research dependency is simdjson v4.6.4. The generated spike stays
on C++17 and uses documented On-Demand APIs exposed through `simdjson.h`.
Pinning this release makes the experiment reproducible; it does not freeze a
future CJM compatibility window.

The simdjson On-Demand API is not a compact DOM. It avoids materializing a full
JSON tree and parses values as they are used.

The spike must design around these constraints:

- input must have simdjson padding
- the input buffer must remain alive during document iteration
- the parser must remain alive while its document is being iterated
- a parser has one active document iteration at a time
- object and array traversal is forward-oriented
- values should be consumed once
- not all malformed input is reported at `iterate()` time because validation
  happens as values are used

References:

- [simdjson v4.6.4 basics](https://github.com/simdjson/simdjson/blob/v4.6.4/doc/basics.md)
- [simdjson v4.6.4 builder](https://github.com/simdjson/simdjson/blob/v4.6.4/doc/builder.md)
- [simdjson v4.6.4 release](https://github.com/simdjson/simdjson/releases/tag/v4.6.4)

These are runtime constraints. They must not leak into Metadata IR.

---

# Layer Ownership

The simdjson backend owns:

- generated simdjson-specific traversal
- selected simdjson headers and runtime calls
- translation from simdjson-local errors to CJM decode errors
- simdjson parser, document, and input lifetime wrappers

The backend must consume:

- validated Metadata IR
- runtime JSON semantic profile
- decode error/path model
- static backend selection policy
- conformance fixture expectations

The backend must not own:

- parser syntax
- comment parsing
- field-name normalization
- ignored-field semantics
- a universal `JsonRuntime` facade
- a second type algebra

---

# Initial Model Subset

The first decode prototype supports one root object with required scalar fields.

Supported field kinds:

```text
Bool
SignedInteger
UnsignedInteger
```

Rules:

- all fields in the selected model are required
- the JSON root must be an object
- each supported field is decoded from its effective JSON field name
- ignored fields are skipped because they are already represented in Metadata IR
- scalar type aliases may work only when Semantic Analysis has already
  normalized them into a supported scalar `FieldTypeKind`

Unsupported in this spike:

- optional fields
- floating-point fields
- strings
- arrays
- vectors
- maps
- enums
- generated user-defined objects
- fixed arrays
- nested paths beyond root fields
- borrowed strings or bytes
- custom converters
- default values
- encode
- round trip

Unsupported types must fail at generation time when practical.

---

# Native simdjson API Classification

In this program, `typed` does not mean `reflective`. simdjson v4.6.4 exposes
several distinct layers that must not be treated as one design:

| Path | Toolchain boundary | Member discovery | Role in CJM |
| --- | --- | --- | --- |
| Explicit `get_bool()`, `get_int64()`, `get_uint64()`, and scalar `get(T&)` calls | C++17 | None | Selected primitive for explicit generated decoding |
| Custom `value::get<T>` or `document::get<T>` specialization | Pre-C++20 | None; the specialization spells out each field | Same-standard native model-binding comparison |
| `tag_invoke(deserialize_tag, ...)` | C++20 concepts | None; the customization spells out each field | Labeled cross-standard comparison only |
| `simdjson::from` | Experimental C++20 | No independent model-binding strategy | Optional convenience-API comparison only |
| Automatic `get<T>` and `extract_into` with `SIMDJSON_STATIC_REFLECTION` | Experimental C++26 reflection | Public members are discovered at compile time | Excluded from the current generated-backend route |

CJM's selected implementation boundary remains build-time model discovery into
Metadata IR followed by explicit generated C++17 traversal. The generated code
may call simdjson's typed scalar extraction functions, but the user compiler is
not asked to rediscover model fields or instantiate a generic reflective model
walker.

The first same-standard native model-binding baseline should therefore use an
explicit C++17 `get<T>` specialization for the same scalar model. This is a
comparison control, not a CJM integration design. It can show the ownership,
error, and maintenance differences between native binding and CJM-generated
portable decoding without introducing reflection or changing the language
standard.

The C++20 `tag_invoke` path remains relevant because it is the pinned release's
recommended custom-type interface for C++20 users. It is not reflection, but it
would change the generated artifact's toolchain and customization-point
contract. This issue may record that API difference without raising CJM's C++
standard merely to compile the comparison.

C++26 static reflection is not an acceptance baseline for this work. It asks the
user compiler to discover model members, which is a different architecture from
CJM's build-time Metadata IR and generated-code approach. It may be studied in a
future compiler experiment, but it must not justify replacing explicit generated
code in the current backend program.

The current scalar spike does not establish a compile-time or runtime
performance result. Any later claim must measure the relevant path and label its
C++ standard, compiler, semantic options, and API stability.

---

# Input Mode

The spike input mode is complete raw JSON text.

The simdjson-specific adapter may copy or wrap the input into a padded buffer.
That choice is an implementation detail of the spike.

Required properties:

- the padded input lives at least as long as simdjson document iteration
- the parser lives at least as long as the active document
- each decode operation uses one active document iteration
- the generated decoder does not return views into simdjson-owned scratch memory
- trailing non-whitespace content is reported as `trailing_content`

The first spike does not expose a borrowed-input public API.

---

# Output Guarantee

The spike should use the new-object guarantee from the runtime semantic profile:

```text
success: returns a complete object
failure: returns no object and a portable decode error
```

The exact C++ result type is not frozen by this document.

An in-place API is out of scope for the first spike because it would require a
strong or basic failure guarantee before the traversal model is understood.

---

# Generated Decode Shape

The generated decoder should conceptually perform these steps:

```text
1. Prepare simdjson input lifetime.
2. Start one On-Demand document iteration.
3. Verify that the root value is an object.
4. Initialize presence bits for required fields.
5. Iterate object fields once.
6. Dispatch recognized field names to generated scalar decoders.
7. Skip unknown fields in non-strict mode.
8. After iteration, report missing required fields.
9. Return the completed object.
```

This is algorithmic shape only. It does not freeze function names or public API.

Presence bits belong to the generated model-specific decoder, not Metadata IR.

---

# Unknown Fields And Duplicate Keys

The first spike should not claim optional strict behavior.

Unknown fields:

```text
default spike behavior: skip unknown fields
strict unknown-field rejection: unsupported_capability
```

Duplicate keys:

```text
strict duplicate-key detection: unsupported_capability
```

The spike must not document a portable duplicate-key result unless it implements
and tests that result explicitly.

---

# Error Mapping

The spike must translate failures into the runtime decode error model.

Initial portable mappings:

```text
root is not an object
    expected_object
    path: []

missing required field
    missing_required_field
    path: field("<json-name>")

wrong scalar JSON kind
    expected_bool, expected_integer, or expected_unsigned_integer
    path: field("<json-name>")

integer outside supported range
    integer_overflow
    path: field("<json-name>")

malformed JSON
    syntax_error
    path: [] unless simdjson can identify a more specific value context

trailing non-whitespace content
    trailing_content
    path: []
```

Explicit `null` must fail for these required non-optional fields. A dedicated
`null_for_non_nullable` result is part of the shared runtime profile but is not
a required conformance claim for this limited scalar spike. It must be added
before a broader generated-codec backend claims profile conformance.

Backend-local simdjson status codes or diagnostic strings may appear in
`runtime_detail`. Tests should not depend on exact simdjson diagnostic text.

---

# String Policy

The current scalar spike does not decode string fields.

When string decode enters the next generated-codec vertical slice, its initial
policy is owned `std::string`.

Borrowed string views are out of scope.

Reasons:

- simdjson string extraction may involve unescaping
- extracted views can depend on input or runtime-managed storage
- borrowed lifetime rules belong to a later explicit ownership design

That later slice may measure or record copy costs, but it must not claim
zero-copy string decode.

---

# Backend Selection Boundary

simdjson remains explicitly selected.

The spike must preserve these static backend selection invariants:

- default generation remains nlohmann
- simdjson is not required by users that do not select it
- simdjson headers do not leak into CJM core, parser, semantic analysis, or
  non-simdjson generated artifacts
- simdjson C++ standard requirements attach only to selected backend targets
- unsupported types fail before confusing generated-code compiler errors when
  practical

The first spike may use a test-only backend selection hook before the final
CLI/CMake spelling is implemented.

---

# Relationship To Conformance Fixtures

The first conformance slice should target scalar decode cases:

```text
scalar_success
required_missing
type_mismatch
integer_overflow
trailing_content
```

Optional strict cases are not required:

```text
unknown_field
duplicate_key
transactional_decode
byte_for_byte_encoding
```

The backend may report `unsupported_capability` for optional strict cases.

---

# Non-Goals

Do not add in this spike:

- complete simdjson backend support
- encode support
- optional fields
- vectors, arrays, maps, enums, or nested generated objects
- borrowed fields
- bytes logical type
- zero-copy claims
- public runtime facade
- runtime plugin selection
- Glaze, yyjson, RapidJSON, or binary format work
- C frontend work
- public API stability promise
- performance claim

---

# Implementation Risks To Carry Forward

The first code slice must keep these risks visible:

- simdjson On-Demand traversal order may shape generated field dispatch
- skipping unknown nested values may become harder once nested objects and arrays
  are supported
- duplicate-key behavior must not become accidental semantics
- string extraction may copy or borrow depending on runtime behavior
- parser and document lifetimes can be easy to violate in helper APIs
- trailing-content detection must be tested if raw text decode is claimed
- error paths should stay CJM-structured, not simdjson-string-based

---

# Acceptance Criteria For This Design

Status: satisfied by the completed scalar spike.

This boundary is complete when:

- the first supported scalar subset is explicit
- the selected simdjson release and documented API surface are explicit
- unsupported types and capabilities are explicit
- input, parser, document, and output lifetimes are named
- output guarantee is selected
- unknown-field and duplicate-key policy are not left implicit
- error mapping targets CJM's portable decode error model
- conformance fixture relationship is clear
- native typed-conversion paths are identified as future comparison baselines
- no user-facing simdjson support is claimed yet

These criteria close the feasibility spike only. They do not satisfy the
promotion criteria for an experimental or official user-facing backend.
