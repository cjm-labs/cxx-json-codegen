# CJM Roadmap

This roadmap describes CJM as both a product strategy and a development plan.

CJM should become useful through small, testable milestones while preserving a
clear long-term identity.

---

# Vision

CJM is a metadata-driven source generator for Modern C++.

The goal is **not** to become another JSON library.

The goal is to let existing C++ models remain the single source of truth while
generating JSON integration automatically during the build.

> CJM keeps your C++ models as the source of truth. It generates the JSON
> integration around them, not the models themselves.

Design principles:

- C++ source is the source of truth.
- Metadata lives next to the field it describes.
- Generated code is ordinary C++.
- Generation happens at build time, not through runtime reflection.
- The architecture remains backend-independent.
- The workflow remains CMake-first.
- v1.0 supports a documented, tested JSON mapping matrix rather than an
  open-ended promise to support the entire C++ type system.

---

# Architecture Direction

The long-term architecture is:

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
Metadata IR
    |
    +----------------+
    |                |
    v                v
JSON Backend    Schema Backend
    |                |
    v                v
Generated C++    JSON Schema
```

The Metadata IR is the center of CJM.

All parser implementations produce it.
All backends consume it.

Future backends may include:

- nlohmann/json backend
- RapidJSON backend
- native CJM backend
- schema backend
- documentation backend
- OpenAPI backend

These are architectural directions, not v0.1 requirements.

---

# Philosophy

CJM prefers:

- explicit over implicit
- generated code over runtime magic
- metadata over external configuration
- build-time errors over runtime surprises
- ordinary C++ over proprietary DSLs
- readable generated output over opaque machinery

---

# Non-Goals

CJM is not intended to become:

- a JSON parser
- a serialization runtime
- a reflection framework
- a schema-first code generator
- a JSON editor
- a UI renderer
- a general-purpose compiler
- a replacement for CMake
- a replacement for the user's compiler

These are intentionally left to existing projects.

CJM should remain focused:

> Build-time C++ code generation from source-level metadata.

---

# Current Status

CJM is in early development.

The current focus is to preserve the architecture while making the first
end-to-end workflow practical:

- product identity
- architecture
- build pipeline
- metadata model
- initial CMake integration
- first working JSON code generation path

---

# Discovery and Indexing Strategy

CJM should distinguish indexing from SEO.

Indexing means:

> Search engines and developer platforms can discover that CJM exists and
> understand what it is.

SEO means:

> CJM is deliberately competing for search ranking.

Before the product is useful on practical projects, CJM should focus on
indexing and discovery rather than ranking optimization.

## Phase 0 - Indexing and Discovery

This phase may be completed once before or during v0.2.

Goal:

> Make CJM discoverable without distracting from core product development.

Scope:

- GitHub repository title and description
- GitHub About metadata
- GitHub topics
- website URL
- release description
- organization README
- basic website indexing metadata
- `robots.txt`
- `sitemap.xml`
- canonical URL metadata
- Google Search Console setup
- domain verification
- sitemap submission

Out of scope:

- SEO ranking optimization
- content marketing
- blog articles
- newsletter
- Discord or community operations
- documentation search
- API reference

Success criteria:

- Google can discover the CJM website
- GitHub clearly communicates what CJM is
- project metadata consistently describes CJM as a C++ JSON code generator
- indexing work can be left alone while product development continues

## Phase 1 - Adoption Documentation

This phase belongs with v0.3 Adoption.

Goal:

> Help developers understand and try CJM through documentation, not SEO tricks.

Scope:

- documentation site foundation
- getting started guide
- installation guide
- CMake integration guide
- JSON tags guide
- examples
- FAQ
- architecture overview for users

## Phase 2 - Content and Dogfooding

This phase should begin only after CJM has practical product value and real
dogfooding.

Dogfooding requirement:

- integrate CJM into `ull-md-engine` through the supported installation and
  build workflow
- use `ull-md-engine` to validate that CJM works in a real downstream project,
  not only in repository examples
- treat any integration friction as product feedback before v1.0

Potential content:

- Why build-time code generation?
- Go struct tags in C++
- Reflection vs code generation
- case study: `ull-md-engine` after the integration is real
- benchmark writeups

## Phase 3 - Brand and Community

This phase belongs near v1.0, once CJM has stable public workflows.

Potential work:

- release blog
- newsletter
- GitHub Discussions
- Discord or community space
- documentation search
- public roadmap
- API reference
- community contribution paths

---

# v0.1 - Minimal Working Generator

Goal:

> Generate working JSON mapping code for one simple C++ struct.

Scope:

- CMake-first integration
- explicit header registration
- parse one or more C++ headers
- extract basic JSON field tags from comments
- build the initial Metadata Model
- generate `to_json()`
- generate `from_json()`
- generate `*.cjm.hpp`
- support basic scalar fields
- golden-file tests
- example project

Supported initial field types:

- bool
- integer types
- floating-point types
- std::string

Out of scope:

- automatic header discovery
- nested structs
- containers
- custom backends
- native JSON backend
- inheritance
- templates
- polymorphism
- private fields

Success criteria:

```text
struct User {
    std::string name; // json:"name"
    int age;          // json:"age"
};
```

to:

```cmake
cjm_generate(
    TARGET app
    HEADERS
        user.hpp
)
```

to:

```text
user.cjm.hpp
```

to:

```cpp
nlohmann::json j = user;
User parsed = j.get<User>();
```

---

# v0.2 - Practical Models

Goal:

> Support realistic production models.

Add support for:

- nested supported structs
- std::vector<T>
- std::optional<T>
- enums
- namespaces
- type aliases
- `omitempty` metadata
- `ignore` metadata
- multiple input headers
- dependency ordering

Begin the production mapping matrix:

- JSON primitives through common C++ scalar and string types
- JSON objects through supported structs
- JSON arrays through `std::vector<T>`
- missing/null semantics through `std::optional<T>`
- field rename metadata
- `ignore` metadata
- `omitempty` metadata
- basic unsupported-type diagnostics

Improve:

- diagnostics
- generated code formatting
- golden tests
- examples
- integration with realistic downstream model projects

Success criteria:

- multiple related structs generate correctly
- nested JSON objects work
- optional and omitted fields behave predictably
- generated output remains deterministic
- CJM can be dogfooded against practical model headers

---

# v0.3 - Adoption

Goal:

> Make CJM easy for other C++ projects to try and adopt.

Add:

- FetchContent support
- `install()` rules
- CMake package config
- version command
- release artifacts
- cross-platform CI
- GitHub Action
- examples
- complete quick start
- troubleshooting documentation
- documentation site foundation
- getting started guide
- installation guide
- CMake integration guide
- JSON tags guide
- FAQ
- user-facing architecture overview

Mapping scope:

- no new core mapping requirements
- examples should cover the complete v0.2 supported mapping surface
- installation workflows should make generated headers usable without manual
  internal tool invocation

Target platforms:

- macOS arm64
- Linux x86_64
- Windows x86_64

Success criteria:

- users can consume CJM through normal CMake workflows
- users do not manually invoke internal tools during normal development
- examples demonstrate the supported v0.2 model surface

---

# v0.4 - Extensibility

Goal:

> Stabilize the extension points around the Metadata IR.

Add or design:

- backend abstraction
- adapter system
- custom converters
- metadata extensions
- stable IR boundaries
- backend-specific options
- explicit type mapping policies

Expand the mapping matrix:

- `std::array<T, N>`
- `std::map<std::string, T>`
- `std::unordered_map<std::string, T>`
- `enum` and `enum class` string mappings
- custom converter design for future non-core types

Initial backend:

- nlohmann/json

Potential later backends:

- native CJM backend
- RapidJSON backend
- schema backend
- documentation backend

Success criteria:

- generators consume the Metadata IR rather than parser-specific structures
- backend-specific code remains isolated
- public user workflows stay stable as internals evolve
- custom conversion points do not leak parser-specific details

---

# v0.5 - Schema

Goal:

> Generate JSON Schema from the Metadata IR.

Add:

- schema backend
- schema-oriented metadata mapping
- golden tests for schema output
- examples showing generated schemas
- documentation for schema limitations

Schema coverage should align with the supported JSON mapping matrix:

- primitive types
- objects
- arrays
- optional/null behavior
- string-keyed maps
- enum string mappings
- required fields when validation metadata has landed
- default values when default metadata has landed
- documented time string formats when time mappings have landed

Strategic value:

```text
C++
  |
  v
CJM
  |
  v
JSON Schema
  |
  v
quicktype or other schema tooling
  |
  v
Rust / Go / TypeScript / other languages
```

CJM should not try to generate every language itself.
Schema export gives CJM a clean integration point with existing ecosystems.

---

# v0.6 - Performance

Goal:

> Make CJM fast enough for real projects.

Improve:

- incremental generation
- file hashing
- metadata cache
- dependency tracking
- unnecessary rewrite avoidance
- parallel generation
- benchmarks

Mapping scope:

- no major new mapping requirements
- performance work should cover realistic model sets using mappings introduced
  through v0.5
- generated output for supported mappings should remain deterministic and avoid
  unnecessary rewrites

Investigate:

- RapidJSON backend
- streaming writer
- native backend research

Generated files should not be rewritten if contents do not change.

---

# v0.7 - Reliability

Goal:

> Make CJM predictable and trustworthy on larger real-world inputs.

Improve:

- source-location aware diagnostics
- unsupported-type diagnostics
- duplicate tag detection
- invalid tag syntax diagnostics
- dependency cycle detection
- generated include stability
- deterministic dependency ordering
- clearer CMake failure messages
- regression tests for failure cases

Complete semantic mapping features:

- required metadata
- optional metadata
- default value metadata
- documented time string mappings
- missing required diagnostics
- type mismatch diagnostics
- invalid enum string diagnostics

Success criteria:

- users can understand why generation failed
- diagnostics point to the user's source whenever possible
- unsupported input fails clearly instead of producing confusing generated code
- supported input produces stable output across repeated builds

---

# v0.8 - Documentation and Ecosystem

Goal:

> Make CJM understandable as a Modern C++ developer tool, not only as a JSON helper.

Add:

- complete quick start
- tutorial
- examples for each supported model pattern
- generated code explanation
- troubleshooting guide
- JSON mapping scope documentation
- backend strategy documentation
- competitive landscape documentation
- contribution guide
- release notes process

Mapping scope:

- complete supported type matrix documentation
- examples for every supported v1.0 mapping
- troubleshooting entries for common mapping failures
- generated code explanation for supported mappings

Example projects:

- simple struct
- nested structs
- vector fields
- optional fields
- enum fields
- namespace usage
- schema generation
- larger CMake project

Success criteria:

- a new user can understand what CJM is and what it is not
- documentation explains the source-of-truth model
- examples match supported behavior instead of future aspirations
- contributors can understand CJM's architecture before changing code

---

# v0.9 - Release Hardening

Goal:

> Prepare CJM for a stable v1.0 release.

Harden:

- public CMake API
- CLI behavior
- generated file conventions
- Metadata IR compatibility expectations
- backend option behavior
- versioning policy
- release artifact process
- cross-platform CI
- installation validation
- dogfood coverage
- final v1.0 supported mapping matrix review

Mapping scope:

- freeze the v1.0 mapping matrix
- complete golden tests for supported mappings
- complete schema backend tests for supported mappings
- move unfinished mapping work to Future Ideas or mark it experimental

Success criteria:

- all v1.0 public interfaces are either stable or explicitly marked experimental
- release artifacts can be produced repeatably
- supported platforms are tested continuously
- downstream projects can adopt CJM without relying on repository internals
- `ull-md-engine` consumes CJM through the supported installation workflow
- all supported mappings have tests, examples, and diagnostics

---

# v1.0 - Production Ready

Goal:

> Make CJM stable enough for external production use.

Definition of done:

- production-ready core workflow
- documented public APIs
- stable CMake API
- stable generated file conventions
- versioned releases
- installable packages
- release artifacts
- reliable diagnostics
- tested supported type matrix
- cross-platform CI
- examples
- troubleshooting documentation
- dogfooded on practical models, including `ull-md-engine`
- at least several external or downstream projects using CJM successfully

Required JSON mapping surface:

- `bool`
- signed and unsigned integer types
- floating-point types
- `std::string` as UTF-8 JSON strings
- nested supported structs
- namespaces
- multiple input headers
- `std::vector<T>`
- `std::array<T, N>`
- `std::optional<T>`
- `std::map<std::string, T>`
- `std::unordered_map<std::string, T>`
- `enum` and `enum class` string mappings
- field rename metadata
- ignore metadata
- `omitempty` metadata
- required and optional metadata
- default value metadata
- documented time string mappings
- schema output for supported mappings
- clear diagnostics for unsupported types and invalid metadata

Out of scope for v1.0 by default:

- arbitrary C++ templates
- arbitrary custom containers
- arbitrary map key conversion
- pointer ownership semantics
- inheritance and polymorphic serialization
- native UTF-16 or UTF-32 string conversion
- Unicode normalization
- automatic timezone inference
- complete JSON Schema validation runtime

v1.0 should represent a tool that external C++ developers can reasonably try in
real projects, not just a demo pipeline.

---

# Future Ideas

These are not commitments.

They may be explored only after the core workflow is stable.

Possible future work:

- C++26 reflection integration
- JSON Schema export improvements
- RapidJSON backend
- YAML backend
- OpenAPI integration
- reflection backend
- validation metadata
- incremental generation improvements
- IDE integration
- VS Code extension
- compile database integration
- package manager integration
- richer metadata syntax
- custom field adapters

---

# Roadmap Principle

Every milestone should produce something testable.

Every feature should preserve the core philosophy:

```text
Standard C++ in.

Standard C++ out.
```
