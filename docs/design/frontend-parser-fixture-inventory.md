# Frontend Parser Fixture Inventory

This document inventories the current CJM C++ frontend parser surface before
the v0.3.5 Tree-sitter frontend research spike.

The goal is to define what a future frontend must represent, not to redesign
Semantic Analysis, Metadata IR, or backend generation.

---

# Purpose

The v0.3.5 parser research spike must answer whether Tree-sitter can become a
future C++ frontend foundation for CJM.

Before evaluating Tree-sitter, CJM needs a clear inventory of:

- the syntax facts the current parser produces;
- the source forms currently covered by parser fixtures;
- the frontend assumptions currently implied by semantic tests;
- unsupported or ambiguous C++ forms that must fail closed in a future frontend.

This document is focused on frontend syntax input. It does not change the
Metadata IR or generated output contract.

---

# Current Syntax Model Contract

The current parser boundary is `cjm::parser::SourceFileSyntax`.

A replacement frontend must be able to produce equivalent syntax facts for the
supported source subset.

## SourceFileSyntax

Represents one parsed C++ source file.

Required facts:

- source path;
- parsed struct declarations;
- parsed enum declarations;
- parsed type aliases;
- collected comments when supported by the frontend.

## DeclarationSyntax

Represents one supported `struct` declaration.

Required facts:

- struct name;
- namespace path;
- source location of the declaration;
- supported field declarations;
- declaration-level comments when supported by the frontend.

## FieldSyntax

Represents one supported non-static data field.

Required facts:

- field name;
- original type spelling;
- source location of the field;
- attached same-line comments.

The parser does not classify the field type. Type classification belongs to
Semantic Analysis.

## EnumSyntax

Represents one supported `enum` or `enum class` declaration.

Required facts:

- enum name;
- namespace path;
- source location.

The parser does not interpret enumerators.

## TypeAliasSyntax

Represents one supported `using` type alias.

Required facts:

- alias name;
- target type spelling;
- namespace path;
- source location.

Alias resolution belongs to Semantic Analysis.

---

# Current Parser-Covered Source Forms

The handwritten parser currently covers the following source shapes in parser
fixtures and integration fixtures.

## Namespaces

Supported:

```cpp
namespace company::model {
// declarations
} // namespace company::model
```

The parser records the namespace as:

```text
company, model
```

The parser also tracks namespace block closing braces while not inside a struct.

## Struct Declarations

Supported:

```cpp
struct User {
    std::string name; // json:"name"
};
```

The parser records the struct name, namespace path, source location, and
supported fields.

## Enum Declarations

Supported:

```cpp
enum Status {
    Active,
    Suspended,
};
```

## Enum Class Declarations

Supported:

```cpp
enum class Status {
    Ok,
    Failed,
};
```

The parser records the enum name and namespace path. Enumerator values are not
part of the current frontend contract.

## Type Aliases

Supported:

```cpp
using UserId = int;
using Sequence = std::uint64_t;
```

The parser records the alias name and raw target type spelling.

## Ordinary Data Fields

Supported:

```cpp
std::string name; // json:"name"
UserId id;        // json:"id"
Address address; // json:"address"
```

The parser records the field name and raw type spelling. It does not decide
whether the type is supported JSON metadata.

## Default Member Initializers

Supported:

```cpp
UserId id = 0; // json:"id"
```

The parser ignores the initializer and records:

```text
type_spelling = UserId
name = id
```

## Same-Line JSON Metadata Comments

Supported:

```cpp
std::string name; // json:"name"
std::optional<std::string> nickname; // json:"nickname,omitempty"
```

The parser records the comment text after `//`.

Metadata validation belongs to Semantic Analysis.

## Template Type Spellings

The parser currently records template types as raw spelling:

```cpp
std::vector<std::string> tags; // json:"tags"
std::optional<std::string> nickname; // json:"nickname,omitempty"
std::map<std::string, std::vector<std::uint64_t>> buckets; // json:"buckets"
std::unordered_map<std::string, std::string> aliases; // json:"aliases"
```

Nested template parsing belongs to Semantic Analysis today. A future frontend
may provide richer structure, but it must preserve the original spelling needed
by the current semantic layer unless that layer is intentionally changed in a
separate issue.

---

# Semantic-Implied Frontend Requirements

Some frontend requirements are more visible in semantic tests than in parser
fixture tests.

A future frontend should provide syntax facts sufficient for these semantic
behaviors.

## Generated Struct References

Semantic Analysis resolves fields that reference generated structs:

```cpp
Address address; // json:"address"
```

The frontend must preserve the field type spelling and namespace path so
Semantic Analysis can resolve the named type.

## Enum References

Semantic Analysis resolves enum field types:

```cpp
Status status; // json:"status"
```

The frontend must expose enum declarations and preserve enum field type
spelling.

## Parent Namespace Lookup

Semantic Analysis can resolve names through current and parent namespaces.

The frontend must preserve accurate namespace paths for declarations, enums,
aliases, and fields.

## Multiple Files

`analyze_source_files()` merges multiple `SourceFileSyntax` values before
semantic analysis.

The frontend contract remains file-local:

- parse one source file;
- produce one `SourceFileSyntax`;
- let orchestration merge files later.

## Source Locations

Semantic diagnostics depend on parser source locations.

The frontend must provide at least line and column locations for:

- declarations;
- fields;
- enums;
- type aliases;
- metadata comments where available.

Tree-sitter may provide byte ranges as well, but adding range-aware diagnostics
is separate from this inventory.

---

# Unsupported Or Ambiguous Forms

The current handwritten parser is intentionally small and line-oriented.

The Tree-sitter spike should evaluate whether each unsupported form can either
be supported safely or rejected with a source-aware diagnostic.

The future frontend must fail closed for managed syntax. It must not silently
produce a partial or misleading `SourceFileSyntax`.

## Multiline Field Declarations

Currently unsupported:

```cpp
std::map<std::string,
         std::vector<std::uint64_t>>
    buckets; // json:"buckets"
```

Tree-sitter should be evaluated for this form because it is common in real C++.

## Preceding Metadata Comments

Currently unsupported:

```cpp
// json:"name"
std::string name;
```

The v0.3.5 spike must not change metadata syntax rules by accident.

## Block Comment Metadata

Currently unsupported:

```cpp
std::string name; /* json:"name" */
```

Same-line `//` metadata remains the only accepted form unless a future design
explicitly changes it.

## Multiple Declarators

Must not be accepted silently:

```cpp
int x, y; // json:"x"
```

The adapter should reject this as ambiguous for CJM metadata.

## Function Declarations Inside Structs

Must not become fields:

```cpp
void reset();
```

The adapter should ignore or reject according to the final frontend policy, but
it must not produce a serializable field.

## Static Members

Must not become serializable instance fields:

```cpp
static int count; // json:"count"
```

## Function Pointers And Member Pointers

Unsupported:

```cpp
void (*callback)(); // json:"callback"
```

## Macro-Generated Fields

Unsupported:

```cpp
CJM_FIELD(std::string, name)
```

## Preprocessor-Controlled Fields

Ambiguous and currently unsupported:

```cpp
#if ENABLE_NAME
std::string name; // json:"name"
#endif
```

The spike should document how Tree-sitter represents these forms and whether
CJM can safely reject or ignore them.

## Bare Standard Types From Using Declarations

Semantic Analysis intentionally rejects bare `string` today unless parser syntax
records enough using-declaration information.

Unsupported today:

```cpp
using std::string;

struct User {
    string name; // json:"name"
};
```

Tree-sitter may make this easier to represent, but supporting it requires a
separate parser and semantic design decision.

---

# Tree-sitter Spike Questions

The v0.3.5 spike should answer these questions with evidence.

## Representation

- Can `tree-sitter-cpp` represent each current parser fixture?
- Which node kinds are needed for namespaces, structs, fields, enums, aliases,
  comments, and template types?
- Can the adapter recover the same `type_spelling` strings currently consumed
  by Semantic Analysis?

## Comment Binding

- Are same-line `// json:"..."` comments represented consistently?
- Can the adapter prove that the comment appears after the field semicolon on
  the same physical source line?
- Can preceding comments and block comments remain unsupported without
  accidental attachment?

## Fail-Closed Behavior

- How does Tree-sitter expose root errors?
- How does it expose `ERROR` nodes?
- How does it expose missing tokens?
- Can the adapter reject malformed managed declarations before semantic
  analysis?
- Can unsupported declarator forms be rejected with source-aware diagnostics?

## Build And Dependency Model

- Can CJM consume pinned Tree-sitter runtime and grammar sources without
  requiring users to install Node.js, Rust, npm, Cargo, Python, or the
  Tree-sitter CLI?
- Should the spike prefer pinned `FetchContent` or vendored generated sources?
- What license notices are required?

## Architecture

- Can all Tree-sitter-specific types stay inside `frontends/cxx/parser`?
- Can the adapter output `SourceFileSyntax` without changing Metadata IR?
- Can existing Semantic Analysis and backends remain unchanged?

---

# Acceptance Criteria For v0.3.5.1

This inventory step is complete when:

- the current syntax model contract is documented;
- current parser-covered source forms are listed;
- semantic-implied frontend requirements are listed;
- unsupported and ambiguous forms are identified;
- Tree-sitter spike questions are explicit;
- no production parser behavior changes have been made.
