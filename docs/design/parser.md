# Parser

The parser is responsible for transforming user C++ source code into an abstract syntax representation that can be analyzed by CJM.

Parsing is an implementation detail.

Users should never need to understand which parser CJM uses internally.

---

# Responsibilities

The parser is responsible for:

- parsing C++ source files
- discovering declarations
- preserving source locations
- exposing parser-independent source facts suitable for semantic analysis

The parser is **not** responsible for code generation.

---

# Input

The parser receives ordinary C++ source files.

Example:

```cpp
struct User {
    std::string name;   // json:"name"
    int age;            // json:"age"
};
```

The source code should remain valid standard C++.

---

# Output

The parser does **not** generate C++.

Instead it produces an intermediate representation suitable for semantic analysis.

```
Source

↓

Parser

↓

AST
```

The AST is an internal implementation detail.

---

# Parser Independence

CJM does not expose parser-specific APIs.

Possible parser implementations include:

- The current handwritten bootstrap parser
- Tree-sitter-based frontend adapters
- Clang
- Future parser implementations

The public API should remain unchanged regardless of parser implementation.

Parser implementation details must not leak into Semantic Analysis, Metadata
IR, backends, generated code, or public user workflows.

---

# Source Locations

The parser should preserve:

- filename
- line number
- column

These locations enable:

- diagnostics
- generated comments
- debugging

---

# Templates

The parser may identify template declarations when they are relevant to later stages.

Arbitrary template modeling is outside the v0.1 scope.
Future template support should be handled after semantic analysis has a concrete need for it.

The parser should avoid making policy decisions.

---

# Namespaces

Namespaces should be preserved exactly as written.

Fully qualified names should remain available throughout the pipeline.

---

# Comments

User metadata may appear inside comments.

The parser should expose comment information to later stages without interpreting it.

Example:

```cpp
std::string name; // json:"name"
```

Whether a comment represents metadata is determined by semantic analysis.

When a parser implementation provides concrete comment nodes, CJM still owns
comment-to-field attachment rules. A parser must not attach metadata by tree
proximity alone.

---

# Error Handling

Parser errors should stop the generation process.

Diagnostics should reference the user's original source code whenever possible.

Parser implementations must fail closed. Producing a syntax tree is not enough
to continue generation. If a parser recovers from malformed input, CJM must
still detect unsupported or ambiguous managed declarations and reject them
before Semantic Analysis receives partial source facts.

For Tree-sitter specifically, a future adapter must inspect error recovery
signals such as `has_error`, `ERROR` nodes, and `MISSING` nodes for managed
declarations.

---

# What the Parser Does NOT Do

The parser should not:

- validate metadata
- build dependency graphs
- generate code
- perform serialization logic
- determine generation order

Those responsibilities belong to later stages.

---

# Design Principle

The parser should remain as simple as possible.

Its only responsibility is to understand C++ source code and expose enough source facts for later stages.

Business logic should never live inside the parser.
