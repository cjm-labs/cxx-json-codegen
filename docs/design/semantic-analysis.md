# Semantic Analysis

Semantic Analysis is the stage between parsing and the Metadata Model.

It converts raw source-level information into meaningful CJM metadata.

This stage is responsible for understanding what the user's C++ declarations mean from CJM's perspective.

---

# Overview

```
C++ Source
    │
    ▼
Parser
    │
    ▼
AST
    │
    ▼
Semantic Analysis
    │
    ▼
Metadata Model
    │
    ▼
Code Generator
```

The parser understands syntax.

Semantic Analysis understands meaning.

The Metadata Model stores the result.

---

# Purpose

Semantic Analysis answers questions such as:

- Which declarations are visible to CJM?
- Which structs should generate JSON mapping code?
- Which fields are supported?
- What metadata is attached to each field?
- What is the canonical type of each field?
- Are the user's tags valid?
- Are there duplicate JSON keys?
- Are there type references that later stages may need for ordering or diagnostics?

This logic should not live in the parser.

This logic should not live in the generator.

---

# Responsibilities

Semantic Analysis is responsible for:

- selecting supported declarations
- validating user metadata
- resolving field types
- canonicalizing names
- preserving enough type-reference information for later dependency analysis when needed
- detecting unsupported constructs
- producing diagnostics
- constructing the Metadata Model

---

# Declaration Selection

Semantic Analysis determines which declarations should participate in generation.

For example:

```cpp
struct User {
    std::string name; // json:"name"
};
```

If `User` contains valid CJM metadata, Semantic Analysis may mark it as a generated type.

Rules may include:

- structs only
- public fields only
- explicitly tagged fields only
- supported field types only

The exact rules may evolve over time.

---

# Metadata Validation

User metadata must be validated before code generation.

Example:

```cpp
std::string name; // json:"name"
```

Semantic Analysis validates:

- metadata syntax
- duplicate keys
- unsupported options
- invalid combinations
- missing required information

Invalid metadata should produce clear diagnostics.

---

# Type Resolution

Semantic Analysis resolves C++ field types into CJM Type Models.

Example:

```cpp
std::vector<std::string> names; // json:"names"
```

Semantic Analysis should understand this as:

```text
vector<string>
```

rather than a raw parser-specific type node.

This makes the generator simpler.

---

# Type Support

Initial supported types may include:

- bool
- integer types
- floating-point types
- std::string
- nested supported structs
- std::vector<T>

Unsupported types should produce diagnostics that point to the user's source code.

---

# Canonical Names

C++ allows many ways to refer to the same type.

Semantic Analysis should normalize names whenever practical.

For example:

```cpp
using UserName = std::string;
```

The field type may be canonicalized as:

```text
std::string
```

while still preserving useful source information for diagnostics.

---

# Dependencies

Generated types may depend on each other, but v0.1 does not require semantic analysis to build a standalone dependency graph.

Example:

```cpp
struct Address {
    std::string city; // json:"city"
};

struct User {
    Address address; // json:"address"
};
```

For v0.1, semantic analysis may represent this relationship through the resolved field type:

```text
FieldModel.type = Address
```

Explicit dependency relationships may be introduced later when there is a concrete need, such as generation ordering, recursive type detection, cycle diagnostics, or multi-file generation planning.

---

# Diagnostics

Semantic Analysis should produce the majority of user-facing diagnostics.

Good diagnostics should include:

- original source file
- line number
- column number
- clear explanation
- suggested fix when possible

Example:

```text
user.hpp:12:18: unsupported CJM field type: std::map<std::string, int>
```

Diagnostics should point to the user's source, not primarily to generated files.

---

# Separation From Parser

The parser should expose source facts.

Semantic Analysis should interpret those facts.

The parser should not decide:

- whether metadata is valid
- whether a type is supported
- whether a declaration should generate code
- how dependency ordering works

Those decisions belong here.

---

# Separation From Generator

The generator should consume validated metadata.

It should not perform semantic validation.

The generator may assume that:

- types are supported
- metadata is valid
- names are canonicalized
- field type references are parser-independent

This keeps code generation simple and predictable.

---

# Failure Model

If Semantic Analysis fails, code generation should stop.

Partial generated output should be avoided unless explicitly requested for debugging.

The build should fail early with actionable diagnostics.

---

# Extensibility

Future versions of Semantic Analysis may support:

- optional fields
- custom field names
- default values
- nested containers
- enum handling
- template constraints
- user-defined adapters
- schema validation

Each new feature should be expressed through the Metadata Model rather than leaking parser-specific concepts into generators.

---

# Summary

Semantic Analysis is where CJM understands the user's program.

It converts parser output into a stable Metadata Model.

By keeping semantic logic separate from both parsing and generation, CJM remains maintainable, testable, and extensible.
