# Default Field Mapping

This document defines the v0.5.x field-mapping semantics for CJM, released in
v0.5.1.

The goal is to remove redundant same-name JSON tags while preserving explicit
metadata for exceptions.

---

# Goal

Users should be able to write ordinary C++ fields without repeating the same
name in JSON metadata.

```cpp
struct User {
    std::string first_name;
    int age = 0;
    std::optional<std::string> nickname; // json:",omitempty"
    std::string display_name;            // json:"displayName"
    int internal_id;                     // json:"-"
};
```

Effective JSON field names:

```text
first_name   -> "first_name"
age          -> "age"
nickname     -> "nickname"
display_name -> "displayName"
internal_id  -> ignored
```

No case conversion is part of this feature.

---

# Managed Model Boundary

For the v0.5.x design, a CJM-managed model is:

> A supported public `struct` declaration found in a header explicitly supplied
> to CJM.

This keeps the near-term workflow simple:

```bash
cjm generate --input user.hpp --output user.cjm.hpp
```

All supported fields in that explicitly supplied model surface are managed by
CJM unless explicitly ignored.

Consequences:

- completely tag-free models are supported
- mixed tagged and untagged fields are supported
- helper structs inside input headers are treated as managed when they appear as
  supported public structs
- unsupported included fields fail during Semantic Analysis
- unsupported fields may be explicitly excluded with `json:"-"`

A future type-level opt-in syntax may be designed if mixed implementation/model
headers become common enough to require it. v0.5.x does not add such syntax.

---

# Field Name Normalization

Semantic Analysis owns field-name normalization.

Backends must not inspect comments or decide whether a JSON name came from an
explicit tag or a default.

For each managed field, Semantic Analysis computes:

```text
cpp_name
effective_json_name
ignored
omit_empty
source_location
type
```

Rules:

```text
json:"-"
    ignored = true

json:"name"
    ignored = false
    effective_json_name = "name"

json:"name,omitempty"
    ignored = false
    effective_json_name = "name"
    omit_empty = true

json:",omitempty"
    ignored = false
    effective_json_name = cpp_name
    omit_empty = true

no json metadata
    ignored = false
    effective_json_name = cpp_name
    omit_empty = false
```

Explicit metadata remains authoritative when it provides a JSON name.

---

# Ignored Fields

Ignored fields remain in Metadata IR as explicit semantic facts.

The IR must not use an empty JSON name as the marker for ignored fields.

Rationale:

- `json:"-"` is an explicit user decision
- generated model-contract metadata may expose ignored fields to downstream tools
- diagnostics can distinguish explicitly ignored fields from fields CJM never
  saw
- default field mapping needs an explicit exclusion mechanism
- source location remains useful for audit and documentation tooling

Runtime JSON and schema backends skip ignored fields when producing JSON-facing
surfaces.

The generated model contract may expose ignored fields with:

```text
ignored = true
json_name = ""
```

or another stable public representation selected before contract stabilization.
The important rule is that ignored intent comes from explicit IR semantics, not
from an empty string convention.

---

# Duplicate Names

Duplicate checks operate on effective JSON field names.

Example:

```cpp
struct User {
    std::string first_name;
    std::string legacy_name; // json:"first_name"
};
```

Expected diagnostic:

```text
duplicate JSON field name: first_name
```

Ignored fields do not participate in duplicate JSON name checks because they do
not contribute JSON object properties.

---

# Fail-Closed Behavior

A managed model must be completely understood under the supported mapping rules.

Example:

```cpp
struct RuntimeConfig {
    std::string name;
    UnsupportedType state;
};
```

This must fail during Semantic Analysis unless the unsupported field is
explicitly ignored:

```cpp
struct RuntimeConfig {
    std::string name;
    UnsupportedType state; // json:"-"
};
```

The ignored-field policy allows CJM to keep model selection simple without
silently dropping fields.

---

# Backend Responsibilities

Backends consume normalized Metadata IR facts.

Runtime JSON backends:

- skip ignored fields
- use `effective_json_name` for object keys
- use `omit_empty` only for serialization omission policy
- do not parse comments

JSON Schema backend:

- skips ignored fields
- uses `effective_json_name` for properties
- keeps optional/null semantics independent from `omitempty`

Model Contract backend:

- may expose ignored fields
- exposes C++ names, JSON names, type facts, source locations, and `omit_empty`
- must not infer ignored status from an empty JSON name

---

# Out of Scope

v0.5.x does not add:

- automatic case conversion
- global naming policies
- type-level model opt-in syntax
- new container mappings
- custom converters
- runtime backend implementation
- native JSON parsing or formatting

---

# Required Tests

Implementation should add focused tests for:

- all-untagged simple model
- mixed tagged and untagged fields
- explicit same-name tag
- explicit rename
- `json:",omitempty"`
- `json:"name,omitempty"`
- `json:"-"`
- duplicate default/default names when possible
- duplicate default/renamed names
- unsupported untagged included field
- unsupported ignored field
- nested untagged model
- enum field without tag
- vector, array, optional, and map fields without tags
- multiple input headers
- namespace models
- type aliases
- empty struct behavior
- repeated deterministic generation

The same normalized semantics must be visible through Semantic Analysis,
nlohmann generated code, generated model contract, JSON Schema, CLI, and CMake
integration.
