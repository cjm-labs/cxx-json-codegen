# CJM Roadmap

This roadmap describes the planned development path for CJM.

The roadmap is intentionally incremental.

CJM should become useful through small, testable milestones rather than large rewrites.

---

# Current Status

CJM is in early development.

The current focus is to establish:

- product identity
- architecture
- build pipeline
- metadata model
- initial CMake integration
- first working JSON code generation path

---

# v0.1 — Minimal Working Codegen

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

↓

```cmake
cjm_generate(
    TARGET app
    HEADERS
        user.hpp
)
```

↓

```text
user.cjm.hpp
```

↓

```cpp
nlohmann::json j = user;
User parsed = j.get<User>();
```

---

# v0.2 — Practical Type Support

Goal:

> Support realistic application structs.

Add support for:

- nested supported structs
- std::vector<T>
- std::optional<T>
- enums
- namespace-aware generated code
- multiple input headers
- dependency ordering

Improve:

- diagnostics
- generated code formatting
- golden tests
- examples

Success criteria:

- multiple related structs generate correctly
- nested JSON objects work
- generated output remains deterministic

---

# v0.3 — Discovery and Build UX

Goal:

> Improve user experience in larger projects.

Add:

- header discovery mode
- root-based scanning
- generated include aggregation
- better incremental rebuild behavior
- debug mode
- `cjm doctor`
- better CMake diagnostics

Possible CMake API:

```cmake
cjm_generate(
    TARGET app
    DISCOVER
    ROOTS
        include
)
```

Explicit mode remains supported.

Discovery mode should never scan the entire project implicitly.

---

# v0.4 — Backend Strategy

Goal:

> Stabilize backend abstraction.

Initial backend:

- nlohmann/json

Investigate:

- backend interface
- native CJM JSON writer
- reduced dependency mode
- generated-only serialization path
- backend-specific options

Long-term direction:

> CJM should eventually provide a native backend so users are not forced to depend on third-party JSON libraries.

The MVP should not block on this.

---

# v0.5 — Production Diagnostics

Goal:

> Make CJM pleasant to use when things go wrong.

Add:

- source-location aware errors
- clear unsupported-type diagnostics
- duplicate tag detection
- invalid tag syntax diagnostics
- dependency cycle detection
- actionable suggestions

Example diagnostic:

```text
include/user.hpp:12:18: unsupported CJM field type: std::map<std::string, int>
```

---

# v0.6 — Packaging

Goal:

> Make CJM easy to install.

Add:

- prebuilt binaries
- CMake package config
- install rules
- version command
- release artifacts
- basic platform coverage

Target platforms:

- macOS arm64
- Linux x86_64
- Windows x86_64

The user should not need to manually configure parser dependencies.

---

# v0.7 — Performance and Incrementality

Goal:

> Make CJM fast enough for real projects.

Improve:

- incremental generation
- file hashing
- metadata cache
- dependency tracking
- unnecessary rewrite avoidance
- parallel generation

Generated files should not be rewritten if contents do not change.

---

# v0.8 — Documentation and Examples

Goal:

> Make CJM understandable and adoptable.

Add:

- complete quick start
- tutorial
- examples
- FAQ
- generated code explanation
- design notes
- troubleshooting guide

Example projects:

- simple struct
- nested structs
- vector fields
- optional fields
- enum fields
- larger CMake project

---

# v1.0 — Production-Ready Release

Goal:

> Make CJM stable enough for external users.

Requirements:

- stable public CMake API
- stable generated file conventions
- clear versioning
- reliable diagnostics
- tested supported type matrix
- CI
- documentation
- examples
- release artifacts
- public repository
- license
- contribution guide

v1.0 should represent a tool that external C++ developers can reasonably try in real projects.

---

# Long-Term Ideas

These are not commitments.

They may be explored only after the core workflow is stable.

Possible future work:

- native JSON backend
- additional JSON backends
- schema export
- formatting/linting for tags
- IDE integration
- VS Code extension
- compile database integration
- package manager integration
- richer metadata syntax
- custom field adapters
- reflection-like metadata queries

---

# Non-Goals

CJM should not become:

- a general C++ reflection framework
- a full serialization framework for every format
- a JSON DOM library first
- a compiler plugin requirement
- a macro-heavy registration system
- a replacement for CMake
- a replacement for the user's compiler

CJM should remain focused:

> Build-time C++ code generation from source-level metadata.

---

# Roadmap Principle

Every milestone should produce something testable.

Every feature should preserve the core philosophy:

```text
Standard C++ in.

Standard C++ out.
```
