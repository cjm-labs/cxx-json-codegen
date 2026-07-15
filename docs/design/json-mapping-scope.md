# JSON Mapping Scope

This document defines the intended JSON mapping scope for CJM.

It is not a promise to support the entire C++ type system.

The goal is to define a production-oriented, documented, and tested subset of
common Modern C++ model patterns that map cleanly to JSON.

---

# JSON Data Model

JSON has a small data model:

- string
- number
- boolean
- null
- object
- array

CJM maps C++ model declarations onto this data model.

The Metadata IR should represent the mapping semantics explicitly so that
multiple backends can consume the same model.

---

# v1.0 Supported Mapping Target

By v1.0, CJM should support a documented production JSON mapping matrix.

The target matrix should cover common C++ model patterns used by real JSON APIs
and configuration files, while keeping unsupported cases explicit.

---

# Milestone Allocation

The v1.0 mapping matrix should be implemented gradually.

v1.0 is the stabilization target, not the point where all mapping features are
introduced for the first time.

## v0.2 - Practical Models

Initial production mappings:

- `bool`
- signed integer types
- unsigned integer types
- floating-point types
- `std::string` as UTF-8 JSON strings
- nested supported structs
- namespaces
- multiple input headers
- `std::vector<T>`
- `std::optional<T>`
- field rename metadata
- ignore metadata
- `omitempty` metadata
- basic unsupported-type diagnostics

Purpose:

- make CJM useful for realistic but simple production models

## v0.3 - Adoption

Mapping scope:

- no new core mapping requirements
- examples should cover the full v0.2 mapping surface
- installation and CMake workflows should make generated headers easy to consume

Purpose:

- make the existing mapping surface easy to try in other projects

## v0.4 - Extensibility

Expanded mappings and extension points:

- `std::array<T, N>`
- `std::map<std::string, T>`
- `std::unordered_map<std::string, T>`
- `enum` and `enum class` string mappings
- explicit type mapping policies
- custom converter design for future non-core types

Purpose:

- extend the supported model surface while stabilizing the Metadata IR and
  backend boundary

## v0.5 - Schema

Schema output for supported mappings:

- primitive values
- structs as objects
- vectors and arrays as JSON arrays
- optionals and nullability
- string-keyed maps
- enum string values
- required metadata when available
- default value metadata when available
- documented time string formats when available

Purpose:

- validate that the Metadata IR can support non-C++ output backends

## v0.6 - Performance

Mapping scope:

- no major new mapping requirements
- benchmark realistic model sets using mappings introduced through v0.5
- preserve deterministic output and avoid unnecessary rewrites

Purpose:

- make supported mappings fast enough for real projects

## v0.7 - Reliability

Complete semantic mapping behavior:

- required metadata
- optional metadata
- default value metadata
- documented time string mappings
- missing required diagnostics
- type mismatch diagnostics
- invalid enum string diagnostics
- duplicate JSON property diagnostics

Purpose:

- make supported mappings predictable, diagnosable, and safe to use in larger
  projects

## v0.8 - Documentation and Ecosystem

Mapping documentation:

- complete supported type matrix
- examples for every supported mapping
- troubleshooting entries for common mapping failures
- generated code explanation for supported mappings

Purpose:

- make the mapping surface understandable to users and contributors

## v0.9 - Release Hardening

Mapping stabilization:

- freeze the v1.0 mapping matrix
- complete golden tests for supported mappings
- complete schema backend tests for supported mappings
- mark unfinished mappings as experimental or move them to Future Ideas

Purpose:

- ensure v1.0 contains no surprise mapping features and no undocumented
  supported behavior

---

## Primitive Values

Supported:

- `bool`
- signed integer types
- unsigned integer types
- floating-point types
- `std::string`

JSON mapping:

- C++ booleans map to JSON booleans.
- C++ integer and floating-point values map to JSON numbers.
- `std::string` maps to JSON strings.

Notes:

- JSON does not distinguish integer and floating-point numbers at the syntax
  level.
- CJM should document numeric range and precision expectations for each backend.
- `NaN` and infinity are not valid JSON numbers and should not be silently
  generated.

## Unicode and String Encoding

Supported:

- JSON strings as Unicode text through UTF-8 `std::string` by default
- escaped Unicode sequences handled by the selected JSON backend

Out of scope for v1.0 by default:

- `std::u8string`
- `std::u16string`
- `std::u32string`
- `std::wstring`
- Unicode normalization
- locale-sensitive string behavior
- grapheme-cluster-aware length validation

Notes:

- CJM should not implement a Unicode conversion library as part of v1.0.
- Native UTF-16 or UTF-32 string support may be provided later through adapters
  or backend-specific extensions.

## Objects

Supported:

- structs with supported public fields
- nested supported structs
- namespaces
- multiple input headers
- deterministic generated include ordering

JSON mapping:

- C++ structs map to JSON objects.
- Field metadata determines JSON property names and behavior.

Out of scope for v1.0 by default:

- arbitrary private field access
- inheritance and polymorphic dispatch
- union-like object layouts
- automatic handling of arbitrary class invariants

## Arrays

Supported:

- `std::vector<T>`
- `std::array<T, N>`

where `T` is itself a supported JSON-mappable type.

JSON mapping:

- C++ sequence containers map to JSON arrays.

Out of scope for v1.0 by default:

- arbitrary custom containers
- heterogeneous tuple-like arrays
- unordered sequence semantics

## Object Maps

Supported:

- `std::map<std::string, T>`
- `std::unordered_map<std::string, T>`

where `T` is itself a supported JSON-mappable type.

JSON mapping:

- string-keyed C++ maps map to JSON objects with dynamic property names.

Out of scope for v1.0 by default:

- arbitrary `std::map<K, V>` where `K` is not `std::string`
- lossy key conversion
- ordered JSON object semantics

Notes:

- JSON object member names are strings.
- CJM should not infer arbitrary key serialization rules for non-string map
  keys in v1.0.

## Optional, Null, and Missing Values

Supported:

- `std::optional<T>`
- missing JSON properties
- explicit JSON `null`
- `omitempty` metadata

Required behavior:

- CJM must document the difference between an absent property and a property
  whose value is `null`.
- `std::optional<T>` should provide the default representation for nullable or
  missing values.
- `omitempty` should affect serialization only unless explicitly documented
  otherwise.

## Field Metadata

Supported:

- field rename
- ignore
- omitempty
- required
- optional
- default value

Required behavior:

- metadata should live next to the field it describes
- invalid metadata should produce source-location aware diagnostics
- duplicate JSON property names should be diagnosed

## Enum Mapping

Supported:

- `enum`
- `enum class`
- string-based JSON representation

Required behavior:

- enum string mappings must be deterministic
- unknown enum strings should produce clear diagnostics or documented parse
  failures
- numeric enum representation may be supported as an option, but string mapping
  should be the primary v1.0 user-facing mode

## Time Values

Supported target:

- documented time-related mappings based on standard C++ time types where the
  JSON representation is explicit

Initial direction:

- string-based date/time representation
- documented RFC3339 or ISO-8601-compatible format choices

Out of scope for v1.0 by default:

- automatic timezone inference
- locale-sensitive parsing
- all `std::chrono` types
- calendar/timezone policy beyond documented supported cases

Notes:

- JSON has no native date or time type.
- Time support must be designed as explicit metadata and conversion policy, not
  as hidden magic.

## Validation Metadata

Supported target:

- required field validation
- optional field behavior
- default value application
- enum value validation
- type mismatch diagnostics

Possible later validation metadata:

- numeric min/max
- string min/max length
- string pattern
- array min/max items
- map/object additional property policy

Notes:

- CJM should avoid becoming a general runtime validation framework.
- Validation metadata should first serve generated serialization,
  deserialization, diagnostics, and schema output.

---

# Schema Backend Alignment

The schema backend should reflect the supported mapping matrix.

Examples:

- C++ scalars map to JSON Schema primitive types.
- `std::vector<T>` and `std::array<T, N>` map to arrays.
- structs map to objects with named properties.
- string-keyed maps map to objects with additional property schemas.
- enum string mappings map to JSON Schema `enum` values.
- required metadata maps to JSON Schema `required`.
- time mappings should use string schemas with documented `format` annotations
  where appropriate.

The schema backend should not imply support for runtime validation features that
CJM does not generate or document.

---

# Out of Scope for v1.0

CJM v1.0 should not promise:

- support for arbitrary C++ templates
- support for arbitrary custom containers
- arbitrary pointer ownership semantics
- inheritance or polymorphic serialization
- arbitrary map key conversion
- all `std::chrono` types
- automatic timezone inference
- locale-sensitive parsing
- native UTF-16 or UTF-32 string conversion
- Unicode normalization
- complete JSON Schema validation runtime
- a custom JSON parser or JSON DOM library

These may be explored later only when they fit the architecture and product
boundary.

---

# v1.0 Success Criteria

CJM v1.0 should have:

- a documented supported type matrix
- golden tests for representative generated C++ output
- schema backend tests for supported mapping features
- diagnostics for unsupported types and invalid metadata
- examples covering the supported production model surface
- stable generated code conventions for supported mappings

The product claim should be:

> CJM supports a documented, tested subset of common production C++ model
> patterns and JSON mapping features.

The product claim should not be:

> CJM supports most C++ types.
