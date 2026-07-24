# Tree-sitter Frontend Spike

This document describes the planned v0.3.5 research spike for evaluating
Tree-sitter as a future CJM frontend implementation.

The spike is not a production migration.

---

# Goal

CJM currently uses a lightweight handwritten C++ parser. That parser was enough
to bootstrap the first releases, but it becomes riskier as CJM supports more
realistic C++ headers.

Tree-sitter is the preferred candidate to evaluate because it can provide:

- concrete syntax trees
- source ranges
- comment nodes
- multiline declaration structure
- nested template structure
- lightweight C runtime integration
- a possible path toward future language frontends

The goal of v0.3.5 is to decide whether Tree-sitter should become CJM's future
C++ frontend foundation.

---

# Architectural Position

Tree-sitter is a frontend implementation detail.

It must not leak into:

- Semantic Analysis
- Metadata IR
- code generators
- public APIs
- generated C++

The intended pipeline is:

```text
C++ Source
    |
    v
tree-sitter-cpp
    |
    v
CJM C++ Tree-sitter Adapter
    |
    v
SourceFileSyntax
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

Only the source parsing implementation is under evaluation.

---

# Product Boundary

CJM is not a general-purpose C++ compiler.

Tree-sitter may parse far more C++ than CJM supports. CJM must still accept
only the documented source subset that the frontend adapter can convert into
CJM syntax structures unambiguously.

The frontend must fail closed:

- accepted syntax is converted exactly
- unsupported managed syntax produces diagnostics
- malformed managed syntax produces diagnostics
- partial syntax models must not escape after failure
- generation must not continue after frontend failure

Tree-sitter producing a syntax tree is not the same thing as CJM successfully
parsing supported input.

---

# Metadata Syntax

The Tree-sitter spike must not change CJM's public metadata syntax.

The current syntax remains:

```cpp
std::string name; // json:"name"
```

Attribute syntax such as this is not part of the spike:

```cpp
[[cjm::json("name")]]
std::string name;
```

Frontend migration and metadata syntax migration are separate decisions.

---

# Comment Binding Contract

Tree-sitter can identify comments, but CJM owns the semantics that attach a
metadata comment to a field.

For the initial adapter, a metadata comment attaches to a field only when:

- it is a line comment
- it appears on the same physical source line as the field terminator
- it begins after the field semicolon
- only whitespace appears between the semicolon and the comment
- the comment uses supported `json:"..."` syntax
- the field has no other CJM metadata comment

Supported:

```cpp
std::string name; // json:"name"
```

Initially unsupported:

```cpp
// json:"name"
std::string name;

std::string name;
/* json:"name" */

std::string name; /* json:"name" */
```

Do not attach metadata by tree proximity alone.

---

# Correctness Contracts

The adapter must explicitly detect and reject malformed or unsupported syntax.

At minimum, the spike must evaluate:

- root `has_error`
- relevant parent node `has_error`
- `ERROR` nodes inside managed declarations
- `MISSING` nodes inside managed declarations
- unsupported declarator forms
- ambiguous field declarations
- function declarations inside structs
- static members
- malformed templates
- missing semicolons
- preprocessor-controlled fields

Unsupported cases should produce source-aware diagnostics where practical.

---

# Initial Source Subset

The spike should attempt to extract the source forms already used or planned by
CJM:

- namespaces
- nested namespaces
- struct declarations
- enum declarations
- enum class declarations
- using type aliases
- ordinary non-static data fields
- default member initializers
- nested template type spellings
- source locations
- comments
- `std::string`
- `std::vector`
- `std::optional`
- `std::map`
- `std::unordered_map`
- user-defined record types
- enums

The spike does not need to support:

- template class definitions
- function-local structs
- inheritance
- virtual members
- function declarations as serializable fields
- function pointers
- member pointers
- multiple declarators in one declaration
- macros generating fields
- arbitrary conditional compilation
- anonymous unions
- complex bit-fields
- full C++ semantic analysis
- template instantiation
- overload resolution
- compiler-specific extensions

---

# Integration Questions

The spike must answer:

- Can `tree-sitter-cpp` represent all current CJM source fixtures?
- Can it represent nested template types better than the handwritten parser?
- Are source ranges precise enough for diagnostics?
- Are comments reliable enough for deterministic CJM metadata attachment?
- How are malformed declarations represented?
- How are preprocessor constructs represented?
- Which node kinds must the adapter understand?
- Can Tree-sitter runtime and C++ grammar sources be compiled into CJM?
- Do users need Node.js, Rust, npm, Cargo, Python, or the Tree-sitter CLI?
- Should CJM use pinned FetchContent or vendored generated sources?
- What are the build-time, binary-size, and licensing impacts?

---

# Version Strategy

Production integration must not depend on unpinned moving branches.

The spike should evaluate a reproducible version strategy:

- pin a Tree-sitter runtime release or commit
- pin a `tree-sitter-cpp` release or commit
- treat them as a tested version pair
- run parser and generated-output tests before upgrades
- document required licenses and notices

Fetching "latest" during a user build is not acceptable.

---

# Integration Models

The spike should compare two practical options.

## Option A: Pinned FetchContent

Advantages:

- smaller CJM repository
- explicit upstream version control
- easier upstream updates

Risks:

- network required during initial configuration
- dependence on upstream repository layout
- possible upstream CMake instability

## Option B: Vendored Generated Sources

Possible vendored components:

- Tree-sitter runtime C sources
- Tree-sitter public headers
- `tree-sitter-cpp` generated parser
- external scanner if required
- `node-types.json` for development reference

Advantages:

- reproducible offline builds
- no external tool installation
- no dependence on upstream CMake layout

Risks:

- repository size
- manual upgrade process
- license and notice tracking responsibility

---

# Prototype Structure

The exact layout may evolve, but Tree-sitter code should remain isolated under
the frontend layer.

Possible structure:

```text
src/frontends/common/tree_sitter/
    parser_handle.hpp
    tree_handle.hpp
    node_utils.hpp
    source_text.hpp
    diagnostics.hpp

src/frontends/cxx/tree_sitter/
    adapter.hpp
    adapter.cpp
    comment_binding.hpp
    comment_binding.cpp
    node_kinds.hpp

src/frontends/cxx/parser/
    parser.hpp
    syntax.hpp
```

Do not build a general frontend plugin framework during the spike.

---

# Migration Plan

The migration, if approved, should be staged:

1. Research spike and fixture comparison
2. Tree-sitter adapter behind an internal build option
3. Run both parsers against the same corpus
4. Make Tree-sitter the default parser
5. Remove the handwritten parser after confidence is established

The current parser must not be removed during the spike.

---

# Deliverables

The v0.3.5 spike should produce:

- architecture note
- dependency assessment (`docs/design/tree-sitter-dependency-assessment.md`)
- adapter prototype
- correctness report
- staged migration plan
- recommendation

The recommendation must be one of:

- adopt Tree-sitter as the default C++ frontend
- keep Tree-sitter as an optional frontend
- reject Tree-sitter and retain the handwritten parser
- reject Tree-sitter and evaluate another frontend

The recommendation must be evidence-based.

---

# Non-Goals

Do not:

- implement CJM-C
- introduce attribute metadata syntax
- expand CJM to full C++
- add Clang/LLVM
- redesign Metadata IR
- redesign Semantic Analysis
- redesign backend APIs
- implement a frontend plugin framework
- remove the current parser before the spike succeeds
- accept syntax merely because Tree-sitter produced a node
- silently skip unsupported managed declarations

---

# Decision Criteria

Tree-sitter should be adopted only if the spike demonstrates that:

- current supported fixtures are represented correctly
- multiline and nested declarations are handled better than the current parser
- comment attachment can be deterministic
- syntax errors can be detected reliably
- unsupported syntax can fail closed
- Tree-sitter code remains isolated inside the frontend
- Semantic Analysis and Metadata IR remain unchanged
- users do not need external Tree-sitter tooling
- macOS and Linux builds remain practical
- version pinning is reproducible
- adapter complexity is lower than continued handwritten parser maintenance
- the design does not block a future C frontend
