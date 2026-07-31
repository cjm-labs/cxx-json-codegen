# Custom Converter Boundaries

CJM intentionally keeps its built-in mapping matrix small.

The core mapping surface should cover strongly typed, structured, modelable
JSON. Domain-specific scalar types should not automatically become built-in CJM
features.

Custom converters are the planned extension mechanism for types that are
important in real applications but should not expand CJM's core type system.

This document defines the boundary. It does not implement converters.

---

# Goals

Custom converters should let users map domain types such as:

- UUID types
- decimal or money types
- `std::filesystem::path`
- duration wrappers
- time-like domain wrappers
- project-specific identifier types
- third-party scalar wrapper types

without making CJM permanently responsible for each type as a built-in mapping.

The design must preserve the pipeline:

```text
Parser
    |
    v
Semantic Analysis
    |
    v
Metadata IR
    |
    v
Backend
```

No generator should inspect parser syntax to discover converter behavior.

---

# Responsibility Split

## CJM Owns

CJM should own:

- recognizing converter metadata after parsing
- validating that a converter reference is well-formed
- recording converter-backed field intent in Metadata IR or generated contract
- generating backend calls to the selected converter
- reporting source-aware diagnostics for invalid converter usage
- exposing converter-backed type facts to downstream tools when stable

## User Code Owns

User code should own:

- the actual conversion functions or customization points
- domain validation rules
- lossy or lossless conversion policy
- formatting choices
- parsing choices
- error behavior inside the converter
- business-specific normalization

CJM should not guess how to parse or format domain-specific data.

---

# Conceptual Shape

The exact syntax is not selected in v0.4.

A future design might express converter intent near the field:

```cpp
struct Payment {
    Money amount; // json:"amount", converter:"money_as_string"
};
```

or through a C++ customization type:

```cpp
struct Payment {
    Money amount; // json:"amount"
};
```

with a separately registered converter.

These examples are conceptual only. They do not define accepted syntax.

---

# Semantic Model

Semantic Analysis should decide whether a field is:

- built-in mapped
- generated object mapped
- converter-backed
- unsupported

A converter-backed field should still be represented through the Metadata IR
boundary. Backends should receive a normalized semantic fact such as:

```text
field type: domain::Uuid
JSON representation: string
converter: project-defined UUID converter
```

The parser should not validate converter availability.

The backend should not perform source-language lookup.

---

# Backend Behavior

A backend may eventually generate code like:

```cpp
j["id"] = user_uuid_to_json_string(value.id);
```

and:

```cpp
value.id = user_uuid_from_json_string(j.at("id").get<std::string>());
```

The exact generated shape depends on the backend and selected converter API.

The important rule is that the backend consumes converter metadata from the
validated model. It must not inspect comments, attributes, or parser nodes
directly.

---

# Generated Model Contract

Converter-backed fields should eventually be visible to downstream tools.

Possible contract facts include:

- original C++ type spelling
- resolved C++ type name
- JSON representation category
- converter identifier
- source location

The contract should not expose parser implementation details.

Schema backends may later use converter metadata to decide whether a domain type
maps to a JSON string, number, object, or other supported schema shape.

---

# Diagnostics

Future converter diagnostics should be source-aware.

Examples:

- unknown converter reference
- converter not valid for the field type
- converter selected for an unsupported JSON representation
- conflicting converter metadata
- converter-backed type used where built-in mapping is required

Converters should fail closed when CJM cannot prove the mapping is supported.

---

# Not Dynamic JSON

Custom converters are not a dynamic typing escape hatch.

They are intended for closed, concrete C++ types whose JSON representation is
known.

Out of scope for the converter design:

- arbitrary dynamic JSON documents
- `std::any`
- pointer ownership policies
- polymorphic serialization
- `std::variant` without an explicit discriminator policy
- automatic runtime type guessing

Dynamic JSON payloads should be handled by user code or a future explicit JSON
value passthrough policy.

---

# v0.4 Decision

For v0.4:

- converter boundaries are documented
- converter implementation is deferred
- built-in mappings remain the only supported generation path
- unsupported domain scalar types should fail during semantic analysis unless
  already covered by the supported mapping matrix

Future implementation should begin with one narrow converter shape and focused
tests before expanding to broader customization.

