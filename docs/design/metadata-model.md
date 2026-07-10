# Metadata Model

The Metadata Model is the central internal representation used by CJM.

It is the stable boundary between semantic analysis and code generation:

```text
C++ Source
    |
    v
Parser
    |
    v
Semantic Analysis
    |
    v
Metadata Model
    |
    v
Code Generator
    |
    v
Generated C++
```

Every parser implementation produces information that semantic analysis converts into the Metadata Model.
Every code generator consumes the Metadata Model.

The parser must not generate C++ directly.
The generator must not depend on parser-specific AST objects.

---

# Purpose

The Metadata Model represents the user's C++ program from CJM's point of view.

For v0.1, its purpose is narrow:

- describe C++ structs selected for JSON code generation
- describe the fields that participate in generation
- store validated JSON metadata needed by the generator
- preserve source locations for diagnostics
- remain independent of parser and backend implementation details

The model does not attempt to represent the complete C++ language.

---

# Design Goals

The Metadata Model should be:

- C++-specific
- parser-independent
- backend-independent
- deterministic
- simple enough for the v0.1 generator
- easy to inspect while debugging
- stable enough to serve as an internal IR

It should not become:

- a general reflection system
- a multi-language IR
- a complete C++ AST
- a plugin framework
- a speculative abstraction layer for future features

---

# Core Design Principle

The Metadata Model stores semantic information, not parser syntax.

Raw parser facts enter semantic analysis.
Validated meaning enters the Metadata Model.
Generated C++ is produced from the Metadata Model.

For example, this source:

```cpp
struct User {
    std::string name; // json:"displayName,omitempty"
};
```

should conceptually become metadata such as:

```text
json_name = "displayName"
omit_empty = true
```

The raw comment text belongs to parser output.
Validation and interpretation belong to semantic analysis.
The generator consumes the validated result.

---

# Logical Structure

The v0.1 model should be intentionally small:

```text
ProjectModel
    |
    +-- TypeModel
            |
            +-- FieldModel
```

Supporting concepts may include:

- Metadata
- SourceLocation
- FieldType or TypeRef

This structure is enough to support the first JSON generation target without requiring a namespace tree, template IR, dependency graph, or relationship model.

---

# Project Model

`ProjectModel` represents one CJM generation unit.

For v0.1, it should primarily contain an ordered collection of types selected for generation.

Conceptually:

```text
ProjectModel
    types: [TypeModel]
```

The order must be deterministic.
It may reflect semantic-analysis ordering, source ordering, or another explicitly defined stable order.

`ProjectModel` should not expose parser-specific objects or backend-specific generation details.

---

# Type Model

`TypeModel` represents one C++ type selected for generation.

For v0.1, the primary target is a simple C++ struct with supported fields.

Conceptually:

```text
TypeModel
    name
    namespace_path
    qualified_name
    fields
    source_location
```

Where:

- `name` is the unqualified C++ type name, such as `User`
- `namespace_path` is an ordered list of namespace components, such as `["company", "model"]`
- `qualified_name` is the full C++ name, such as `company::model::User`
- `fields` is an ordered list of fields selected for generation
- `source_location` points to the user source declaration when available

The v0.1 `TypeModel` should not require `template_parameters`.
Arbitrary templates are outside the v0.1 scope.
Template support may be introduced later when there is a concrete generator and semantic-analysis requirement.

---

# Field Model

`FieldModel` represents one C++ data member selected for generation.

Conceptually:

```text
FieldModel
    name
    type
    json
    source_location
```

Where:

- `name` is the C++ field name
- `type` is a parser-independent representation of the field's semantic type
- `json` is validated JSON metadata for this field
- `source_location` points to the user source field when available

Field order must be stable.
For v0.1, generated JSON field order should follow the model's field order.

---

# Metadata Representation

The model should store validated semantic metadata, not only raw tag text.

For v0.1, JSON metadata may be represented directly as a small typed structure rather than a generic string-to-string map.

Conceptually:

```text
JsonFieldMetadata
    name
    omit_empty
```

Where:

- `name` is the JSON field name used by generated code
- `omit_empty` records whether an omit-empty option was requested, if v0.1 supports that option

Semantic analysis owns:

- parsing tag text
- validating tag syntax
- rejecting unsupported options
- detecting duplicate JSON names
- converting raw text into typed metadata

The generator should not parse raw tag strings.

The Metadata Model should remain backend-independent.
Storing `json_name` is acceptable because it describes user semantics.
Storing a `nlohmann::json` expression as the primary representation is not acceptable.

---

# Field Type Representation

`FieldModel.type` is a key open design question for v0.1.

It must be:

- parser-independent
- sufficient for the generator
- limited to supported v0.1 types
- smaller than a complete C++ type system

The initial supported type set may include:

- `bool`
- signed integer types
- unsigned integer types
- floating-point types
- `std::string`
- user-defined CJM types, if required by the v0.1 generator

The v0.1 roadmap explicitly leaves containers, inheritance, arbitrary templates, polymorphism, and private fields out of scope.
Pointers and ownership semantics should also remain out of scope unless a concrete v0.1 requirement appears.

The model should not expose parser-specific type objects such as `clang::QualType`.

Possible v0.1 representations include:

```text
FieldType
    kind
    qualified_name
```

or:

```text
TypeRef
    category
    spelling
    canonical_name
```

The final representation should be chosen only after confirming what the first generator needs.

---

# Namespace Representation

Namespace information is required because generated C++ must refer to the user's types correctly.

For v0.1, a standalone `NamespaceModel` or recursive namespace tree is not required.
Namespace identity should be represented on each `TypeModel`.

Example:

```text
name = "User"
namespace_path = ["company", "model"]
qualified_name = "company::model::User"
```

This still represents real C++ namespace information.
It simply avoids adding namespace-level model objects before CJM has namespace-level semantics to store.

A future `NamespaceModel` should be introduced only if there is a concrete requirement, such as:

- namespace-level metadata
- namespace-level diagnostics
- namespace-level generation policy
- grouping behavior that cannot be expressed on types alone

---

# Source Locations

The model should preserve parser-independent source locations whenever practical.

Conceptually:

```text
SourceLocation
    file
    line
    column
```

Source locations enable useful diagnostics and may help generated comments later.

The model must not retain parser-specific source-location objects such as `clang::SourceLocation`.

---

# Relationships and Dependencies

The v0.1 Metadata Model should not require a standalone `RelationshipModel`, `DependencyModel`, or `DependencyGraph`.

For a field such as:

```cpp
struct User {
    Address address;
};
```

the initial relationship can be represented by the semantic type of the field.
Dependency information may be derived later from field type references.

Explicit dependency modeling may be introduced later for concrete needs such as:

- generation ordering
- recursive type detection
- cycle diagnostics
- multi-file generation planning

Until such a need exists, dependency infrastructure should not be added only for future extensibility.

---

# Parser Independence

The Metadata Model must not expose parser-specific types.

It must not contain:

- `clang::Decl`
- `clang::CXXRecordDecl`
- `clang::FieldDecl`
- `clang::QualType`
- `clang::SourceLocation`
- `clang::ASTContext`

Parser-specific information must be converted before entering the model.

Parser independence means the parser implementation is replaceable.
It does not mean CJM is a multi-language framework.
CJM remains a C++ product whose input is ordinary C++ source code.

---

# Backend Independence

The Metadata Model must not depend on `nlohmann/json` types.

The initial generator may target `nlohmann/json`, but the model should remain usable by a future CJM-native backend.

The model may store backend-independent semantic facts such as:

- the C++ type being generated
- the C++ field name
- the validated JSON field name
- whether an option such as omit-empty was requested

The model should not store backend-specific generated expressions as its primary semantic representation.

---

# Determinism

The same Metadata Model and generator configuration must produce identical output.

The model should support deterministic generation by ensuring:

- type order is stable and defined
- field order is stable and defined
- metadata interpretation is deterministic
- unordered container iteration does not determine generated output order

Determinism is required for readable diffs, reliable golden tests, reproducible builds, and predictable incremental generation.

---

# Validation Boundary

User-facing semantic validation belongs primarily to semantic analysis.

Semantic analysis should validate:

- malformed tags
- conflicting options
- unsupported field types
- duplicate JSON field names
- unresolved user-defined types
- invalid option use

The generator may check structural model invariants, such as a missing required field in the model.
Those checks should be treated as internal consistency checks, not as the main semantic-analysis layer.

If invalid user metadata reaches the generator, that usually indicates a semantic-analysis bug.

---

# v0.1 Scope

The v0.1 model must be sufficient to support:

```cpp
struct User {
    std::string name; // json:"name"
    int age;          // json:"age"
};
```

The first implementation should prioritize:

- simple structs
- scalar fields
- `std::string`
- explicit JSON names
- deterministic output
- useful diagnostics

It should not attempt to fully support:

- arbitrary templates
- inheritance
- polymorphism
- pointers
- arbitrary containers
- namespace-level metadata
- a complete C++ type system
- speculative dependency infrastructure

---

# Evolution Rules

The Metadata Model should evolve only when a concrete implementation need appears.

When extending it:

- preserve parser independence
- preserve backend independence
- keep semantic validation in semantic analysis
- avoid exposing parser AST objects
- avoid storing generated backend code as semantic data
- prefer small typed concepts over vague generic maps when behavior is already known
- update this design document before changing model architecture

Future features may require:

- richer type references
- nested user-defined types
- container support
- optional fields
- enum handling
- template-aware metadata
- explicit dependency modeling

Those features should be added incrementally.

---

# Open Design Questions

The following v0.1 questions should be answered by the first implementation:

- What exact `FieldModel.type` representation is sufficient for the first generator?
- Should v0.1 include user-defined CJM field types, or only scalar and `std::string` fields?
- Should `omit_empty` be accepted in v0.1, or deferred until optional/container semantics exist?
- What stable ordering rule should `ProjectModel.types` use across multiple input headers?
- How much original spelling should be preserved alongside canonical type names for diagnostics?

These questions should be resolved conservatively.
If a feature is not required to generate the first `User` example, it should remain deferred.

---

# Summary

The Metadata Model is CJM's semantic IR.

For v0.1, it should be minimal:

```text
ProjectModel
    |
    +-- TypeModel
            |
            +-- FieldModel
```

It should store validated C++ and JSON semantics, preserve source locations, and remain independent of both parser implementation details and JSON backend details.

This keeps the first implementation focused on CJM's core promise:

```text
Standard C++ in.
Standard C++ out.
```
