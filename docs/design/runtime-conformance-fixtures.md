# Runtime Conformance Fixture Layout

This document sketches the v0.6 foundation fixture layout for CJM runtime
backend conformance tests.

It defines how future backend tests describe models, JSON input, expected
results, and capability expectations. It does not implement a test runner.

---

# Goal

Runtime backends may use different integration strategies:

```text
nlohmann/json
    DOM binding

simdjson On-Demand
    generated forward-only codec

Glaze
    direct typed adapter

yyjson
    compact document / DOM binding
```

The conformance fixture layout gives each backend one shared answer to:

```text
which semantic cases exist
which cases are core
which cases require optional strict capability
which result a backend must report
which unsupported behavior must be declared visibly
```

This keeps backend work evidence-driven. A backend cannot silently redefine CJM
runtime semantics and still appear conformant.

---

# Ownership Boundary

CJM runtime conformance fixtures own:

- semantic case names
- core vs optional strict capability classification
- model shape used by each case
- JSON input used by each case
- portable expected success or error result
- backend declaration of pass or unsupported behavior

Runtime backend tests own:

- constructing the selected backend
- invoking generated decode or encode code
- translating backend-local failures into CJM's portable error model
- reporting backend-specific detail as non-semantic evidence

The fixture layer consumes the runtime semantic profile and decode error model.
It does not introduce a second Metadata IR or a runtime registry.

---

# Directory Shape

The intended fixture root is:

```text
tests/conformance/runtime/
    README.md
    cases/
        scalar_success/
        nested_success/
        optional_missing/
        optional_null/
        required_missing/
        type_mismatch/
        integer_overflow/
        invalid_enum/
        fixed_array_extent/
        unknown_field/
        duplicate_key/
        trailing_content/
        nested_error_path/
```

Each case directory should eventually contain:

```text
model.hpp
input.json
expected.json
case.md
```

Meaning:

```text
model.hpp
    ordinary C++ model used by the case

input.json
    JSON input for decode cases

expected.json
    expected successful JSON value, encoded value, or portable error shape

case.md
    human-readable case metadata and rationale
```

The exact machine-readable metadata format is future work. The fixture contract
should stabilize before choosing whether that metadata becomes TOML, JSON, or a
small CMake-driven convention.

---

# Case Metadata Shape

Every case should record the same conceptual fields:

```text
id
    stable case name

direction
    decode, encode, or round_trip

profile
    core or optional_strict

capability
    none, strict_unknown_field, strict_duplicate_key,
    transactional_decode, byte_for_byte_encoding, or
    backend_owned_trailing_content

model
    model fixture used to generate backend code

input
    JSON input fixture, when applicable

expected
    expected portable success or error result

notes
    human explanation of the semantic point being tested
```

The metadata should describe CJM semantics, not backend APIs.

---

# Result Classification

A backend result should be classified as:

```text
pass
    backend produced the expected portable result

fail
    backend claimed support but produced a different portable result

unsupported_capability
    backend does not claim the optional strict capability required by the case

unsupported_type
    backend does not claim support for the Metadata IR type combination

not_applicable
    case does not apply to the backend input or output mode
```

`unsupported_capability` is acceptable for optional strict cases.

`unsupported_capability` is not acceptable for core cases unless the backend also
rejects the relevant type combination or API mode at generation time.

---

# Core Decode Cases

Initial core decode cases:

```text
scalar_success
    supported scalar fields decode successfully

nested_success
    generated user-defined objects decode recursively

optional_missing
    missing optional field decodes as disengaged optional

optional_null
    explicit null optional field decodes as disengaged optional

required_missing
    missing non-optional field reports missing_required_field

type_mismatch
    JSON kind mismatch reports the matching expected_* error

integer_overflow
    out-of-range integer reports integer_overflow

invalid_enum
    unknown enum string reports invalid_enum_string

fixed_array_extent
    wrong fixed-array length reports fixed_array_extent_mismatch

trailing_content
    raw-text decode rejects trailing non-whitespace content

nested_error_path
    nested failures report a composed structured path
```

These cases are the minimum evidence for a backend that claims support for the
corresponding model shape and raw-input mode.

---

# Optional Strict Cases

Initial optional strict cases:

```text
unknown_field
    strict mode rejects an unknown object field

duplicate_key
    strict mode rejects a duplicate object key

transactional_decode
    in-place decode failure leaves the target unchanged

byte_for_byte_encoding
    encode output exactly matches the expected JSON bytes
```

A backend may report `unsupported_capability` for these cases if it does not
claim the relevant strict capability.

---

# Backend Declaration Shape

The first declaration shape should stay static and simple:

```text
backend
    name
    integration_model
    supported_type_surface
    supported_capabilities
    supported_cases
```

Example integration model values:

```text
dom_binding
generated_forward_codec
direct_typed_adapter
compact_dom_binding
```

This declaration is evidence for tests and documentation. It is not a runtime
plugin registry.

---

# Invariants

Runtime conformance must preserve these invariants:

- every backend is measured against the same semantic profile
- core semantic differences are failures, not backend preferences
- optional strict behavior must be declared before it is tested as required
- backend-local diagnostic strings are not required for conformance
- structured error code and path are the primary portable failure evidence
- unsupported type combinations fail at generation time or appear as unsupported
  in the conformance matrix

---

# Relationship To Other Documents

This fixture layout depends on:

- [Runtime JSON Semantic Profile](runtime-json-semantic-profile.md)
- [Runtime Decode Error Model](runtime-decode-error-model.md)

It feeds:

- [Static Backend Selection](static-backend-selection.md)
- simdjson On-Demand decode spike
- future backend promotion criteria
