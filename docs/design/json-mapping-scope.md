# JSON Mapping Scope

This document defines the intended JSON mapping scope for CJM.

It is not a promise to support the entire C++ type system.

The goal is to define a production-oriented, documented, and tested subset of
common Modern C++ model patterns that map cleanly to JSON.

---

# Product Boundary

CJM targets strongly typed, structured, modelable JSON.

The supported mapping matrix should focus on C++ declarations whose JSON shape
can be known at build time:

- named object fields
- explicit C++ field types
- deterministic arrays and objects
- documented optional, required, and default semantics
- explicit converter policies for domain-specific types

CJM should not treat arbitrary dynamic JSON, arbitrary C++ object graphs, or
unbounded runtime type containers as part of the default mapping surface.
Dynamic input should be handled by user-owned validation and conversion code
before it enters CJM's generated strongly typed model path.

Future support for dynamic payloads should be tracked as JSON value
passthrough, not as `std::any`. Future support for `std::variant` should be
tracked as explicit union/sum-type mapping with a documented discriminator
policy.

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

# Field Metadata Semantics

The v0.5.x field-mapping design is defined in
[Default Field Mapping](default-field-mapping.md).

The stable direction is:

- supported fields in CJM-managed models are included by default
- an untagged field uses its exact C++ field name as the effective JSON name
- explicit `json:"name"` metadata renames a field
- `json:",omitempty"` keeps the default field name and records `omit_empty`
- `json:"-"` records explicit ignored-field intent
- duplicate checks operate on effective JSON names
- unsupported included fields fail during Semantic Analysis

Backends consume normalized Metadata IR field facts. They must not parse source
comments or independently decide default field names.

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

## v0.3 - Practical Type Coverage

Status:

- implemented for v0.3.0

Mapping scope:

- `std::map<std::string, T>`
- `std::unordered_map<std::string, T>`
- nested supported map value types
- common fixed-width integer spellings
- composed supported containers and generated structs

Purpose:

- cover common JSON object shapes with dynamic string keys
- keep unsupported dynamic JSON shapes explicit and fail-closed

## v0.3.5 - Frontend Parser Research

Mapping scope:

- no new public mapping semantics
- evaluate whether Tree-sitter can parse current and near-future mapping inputs
  more safely than the handwritten parser
- preserve current metadata syntax and Metadata IR boundaries

Purpose:

- decide whether CJM should migrate the C++ frontend implementation to
  Tree-sitter before larger extensibility work

## v0.4 - Extensibility

Expanded mappings and extension points:

- `std::array<T, N>`
- `enum` and `enum class` string mappings
- explicit type mapping policies
- custom converter design for future non-core types

Purpose:

- extend the supported model surface while stabilizing the Metadata IR and
  backend boundary

## v0.5 - Schema

Status:

- implemented for v0.5.0

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

## v0.5.x - Default Field Mapping

Mapping scope:

- exact C++ field-name defaulting for managed fields
- explicit rename metadata as an override
- default field names combined with `omitempty`
- explicit ignored-field semantics
- duplicate effective JSON name diagnostics
- unsupported included fields fail closed

Purpose:

- make the normal model-authoring path less repetitive
- make field participation explicit before runtime backend work begins

## v0.6 - Canonical Runtime Semantics and Backend Program

Mapping scope:

- no new parallel type algebra
- formally define the JSON semantics of existing Metadata IR `FieldType`
  combinations
- define a minimum runtime semantic profile for missing, null, unknown,
  duplicate, overflow, enum, fixed-array, trailing-content, nested error path,
  and partial-output behavior
- build backend capability documentation and conformance fixtures before runtime
  backend claims
- preserve deterministic output and avoid unnecessary rewrites

Purpose:

- prove that normalized Metadata IR can drive optional runtime JSON backends
  without backend-specific semantic drift

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
- `std::array<T, N>` preserves the fixed-size C++ container type in generated
  code and generated model-contract metadata.
- The current Metadata IR records the normalized array element type. The
  original C++ spelling preserves the visible fixed-size type, including `N`.

Out of scope for v1.0 by default:

- arbitrary custom containers
- heterogeneous tuple-like arrays
- unordered sequence semantics
- C-style arrays

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
- v0.4 maps supported enum fields to JSON strings by default
- the JSON string is the C++ enumerator spelling, such as `Active`
- generated `from_json` should reject unknown enum strings through the
  generated backend conversion helper
- unknown enum strings should produce clear diagnostics or documented parse
  failures
- numeric enum representation may be supported as an option, but string mapping
  should be the primary v1.0 user-facing mode

Out of scope for v1.0 by default:

- custom enum rename metadata
- enum aliases
- case conversion policies
- numeric enum representation as the default mode
- backend-specific enum policies that bypass Metadata IR

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

The schema backend should reflect the supported mapping matrix without becoming
a schema-first workflow.

The v0.5 schema backend consumes validated Metadata IR. Future downstream schema
tools may also consume stable generated model-contract facts, but the in-tree
backend must not inspect parser syntax, Tree-sitter nodes, or nlohmann/json
backend implementation details.

## Schema Dialect

The initial schema backend should emit JSON Schema Draft 2020-12.

Generated schemas should include:

- `$schema`
- `title`
- `type`
- `properties` for object fields
- `required` only for fields CJM can currently prove are required
- `$defs` only when shared object definitions are needed

The selected dialect is a backend output detail. It must not change parser,
Semantic Analysis, or Metadata IR ownership.

## Deterministic Output

Schema output must be deterministic.

Ordering rules:

- top-level generated model schemas follow Metadata IR model order
- object properties follow `TypeModel.fields` order
- enum values follow Metadata IR enum value order
- `$defs` entries follow the dependency order already known to CJM

The schema backend should not sort user fields alphabetically unless the
Metadata IR explicitly does so first.

## Mapping Table

| CJM mapping | JSON Schema shape |
| --- | --- |
| `bool` | `{ "type": "boolean" }` |
| signed integer | `{ "type": "integer" }` |
| unsigned integer | `{ "type": "integer", "minimum": 0 }` |
| floating point | `{ "type": "number" }` |
| `std::string` | `{ "type": "string" }` |
| supported struct | `{ "type": "object", "properties": ... }` |
| `std::vector<T>` | `{ "type": "array", "items": schema(T) }` |
| `std::array<T, N>` | `{ "type": "array", "items": schema(T), "minItems": N, "maxItems": N }` |
| `std::optional<T>` | field may be missing; present value accepts `schema(T)` or `null` |
| `std::map<std::string, T>` | `{ "type": "object", "additionalProperties": schema(T) }` |
| `std::unordered_map<std::string, T>` | `{ "type": "object", "additionalProperties": schema(T) }` |
| enum string mapping | `{ "type": "string", "enum": [...] }` |
| ignored field | omitted from schema properties |
| `omitempty` | does not by itself make a field required or optional |

## v0.5 Implementation Boundary

The v0.5 implementation intentionally covers the practical subset already
represented in Metadata IR and golden tests.

Implemented schema mappings:

- scalar and string fields
- unsigned integer fields with `minimum: 0`
- `std::vector<T>` and `std::array<T, N>` when `T` is a scalar or string mapping
- `std::optional<T>` when `T` is a scalar or string mapping
- `std::map<std::string, T>` and `std::unordered_map<std::string, T>` when `T`
  is a scalar or string mapping
- enum and enum class fields as JSON string enums
- direct generated-struct fields through `$ref` and `$defs`
- ignored fields omitted from `properties`
- non-optional supported fields listed in `required`

Not yet implemented:

- schema output for nested containers
- schema output for containers whose element or value type is an enum or
  generated struct
- schema output for `std::optional<T>` when `T` is an enum, generated struct, or
  container
- custom enum string policies
- default-value metadata
- time or datetime schema formats
- runtime JSON Schema validation
- OpenAPI route or HTTP endpoint generation

## Optional, Required, And Null

For v0.5, `std::optional<T>` should mean:

- the object property may be absent
- if present, the property may be `null`
- if present and non-null, the property must match `schema(T)`

Fields that are not `std::optional<T>` should appear in `required` only when
CJM's current metadata and decode behavior make that guarantee clear.

`omitempty` affects serialization output. It should not be treated as validation
metadata unless a later milestone defines that behavior explicitly.

## Enum Strings

Enum schemas should use the JSON strings known to Metadata IR or the generated
model contract.

For v0.5, custom enum rename metadata is not supported, so enum schema values
match CJM's generated enum string mapping.

## Examples

For:

```cpp
struct User {
    std::string name; // json:"name"
    std::optional<int> age; // json:"age,omitempty"
};
```

CJM may generate a schema shaped like:

```json
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "title": "User",
  "type": "object",
  "properties": {
    "name": { "type": "string" },
    "age": { "type": ["integer", "null"] }
  },
  "required": ["name"]
}
```

The exact formatting is a backend implementation detail, but the semantic shape
and ordering should be covered by golden tests.

## Non-Goals

The schema backend must not imply support for:

- OpenAPI route generation
- HTTP endpoint policy
- runtime JSON Schema validation
- custom converter implementation
- new required/default/time metadata before those features land
- arbitrary dynamic JSON, `std::variant`, `std::any`, pointers, or inheritance

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
