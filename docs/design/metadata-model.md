# Metadata Model

The Metadata Model is the central internal representation used by CJM.

Every parser produces a Metadata Model.

Every code generator consumes a Metadata Model.

This separation allows parsing and code generation to evolve independently.

---

# Overview

```
Source Code

      │

      ▼

Metadata Extraction

      │

      ▼

Metadata Model

      │

      ▼

Code Generator
```

The Metadata Model is the only communication layer between parsing and generation.

Neither component should directly depend on the other.

---

# Why a Metadata Model?

Without an intermediate model:

```
Parser

↓

Generator
```

Both components become tightly coupled.

Any parser change immediately affects the generator.

Instead:

```
Parser
↓
Semantic analysis
↓
Metadata Model
↓
Generator
```

Each stage has a single responsibility.

---

# Design Goals

The Metadata Model should be:

- parser-independent
- generator-independent
- deterministic
- easy to extend
- easy to serialize
- easy to debug

It should represent semantic information rather than parser-specific AST nodes.

---

# What It Represents

The Metadata Model represents the user's program from CJM's perspective.

Examples include:

- namespaces
- classes
- structs
- enums
- fields
- methods (if needed)
- template information
- metadata annotations

It does **not** attempt to represent the complete C++ language.

Only information required for code generation should be stored.

---

# Logical Structure

```
Project
    │
    ├── Namespace
    │
    ├── Type
    │       │
    │       ├── Fields
    │       ├── Metadata
    │       └── Relationships
    │
    └── Dependencies
```

The hierarchy reflects logical relationships rather than parser implementation.

---

# Type Model

Each type contains information required for generation.

Example:

```
Type

    name

    namespace

    qualified_name

    template_parameters

    fields

    metadata
```

The model should not expose parser-specific objects.

---

# Field Model

Each field describes:

```
Field

    name

    type

    qualifiers

    metadata

    source_location
```

Metadata is attached directly to fields.

Future metadata types should not require redesigning the model.

---

# Metadata

Metadata is represented in a generic form.

Example:

```
Metadata

key

value

attributes
```

The Metadata Model should not hardcode JSON.

Future generators may interpret metadata differently.

---

# Source Information

Source information should be preserved whenever practical.

Examples:

- filename
- line
- column

This enables high-quality diagnostics and generated comments.

---

# Dependencies

The model should record dependencies between types.

Example:

```
User

↓

Address

↓

Country
```

Generators can use this information to determine generation order.

---

# Lifetime

The Metadata Model exists only during code generation.

It is an internal representation.

It is not part of the public API.

---

# Extensibility

Future versions may extend the Metadata Model with:

- concepts
- template specialization
- constexpr metadata
- inheritance
- custom generators

Existing generators should continue to work whenever possible.

---

# Stability

The Metadata Model is one of the most stable components of CJM.

Parser implementations may change.

Code generators may evolve.

Build systems may evolve.

The Metadata Model should remain as stable as possible.

---

# Summary

The Metadata Model is the semantic foundation of CJM.

Every parser produces it.

Every generator consumes it.

By separating parsing from generation, CJM remains modular, maintainable, and extensible.
