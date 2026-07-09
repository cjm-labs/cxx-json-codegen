# Backend Strategy

This document describes CJM's JSON backend strategy.

CJM is a build-time code generation tool.

Backends determine what kind of serialization code CJM emits.

---

# Overview

The CJM pipeline is:

```text
C++ Source
    │
    ▼
Parser
    │
    ▼
Semantic Analysis
    │
    ▼
Metadata Model
    │
    ▼
Code Generator
    │
    ▼
Backend-Specific C++
```

The backend is responsible for deciding the shape of generated serialization code.

---

# Initial Backend

The first supported backend is:

```text
nlohmann/json
```

This choice is intentional.

`nlohmann/json` is widely used, easy to understand, and provides a convenient first target for validating the CJM pipeline.

The initial goal is not to compete with JSON libraries.

The initial goal is to prove that CJM can:

- parse C++ metadata
- build a Metadata Model
- generate correct C++ code
- integrate with CMake
- pass tests through generated output

---

# Why Not Start With a Native Backend?

A native backend is valuable.

However, implementing one from the beginning would significantly increase the MVP scope.

It would require CJM to solve two problems at once:

1. Build-time C++ metadata extraction and code generation.
2. JSON parsing and writing infrastructure.

These are both large projects.

The MVP should focus on the CJM pipeline first.

---

# Long-Term Native Backend

Long term, CJM should support a native backend.

A CJM-native backend would reduce dependency risk and improve product independence.

Potential benefits:

- fewer third-party dependencies
- stable generated API
- tighter integration with CJM metadata
- predictable performance characteristics
- better control over diagnostics
- better cross-platform packaging

---

# Backend Independence

The public CJM workflow should remain stable as backends evolve.

For example:

```cmake
cjm_generate(
    TARGET app
    HEADERS user.hpp
)
```

should remain valid even if the default backend changes in the future.

Backend choices should be expressed as options only when necessary.

Example future API:

```cmake
cjm_generate(
    TARGET app
    HEADERS user.hpp
    BACKEND nlohmann
)
```

or:

```cmake
cjm_generate(
    TARGET app
    HEADERS user.hpp
    BACKEND native
)
```

---

# Generated API Stability

Generated user-facing code should remain as stable as possible.

Backend internals may differ.

But user code should ideally remain simple:

```cpp
nlohmann::json j = value;
```

or, for a native backend:

```cpp
std::string json = cjm::to_json(value);
```

The exact API will be defined as the backend design matures.

---

# Backend Interface

Internally, a backend should consume the Metadata Model.

It should not depend on parser-specific AST nodes.

Conceptually:

```text
Metadata Model

    ↓

Backend

    ↓

Generated C++
```

This preserves the separation between frontend and backend.

---

# MVP Backend Scope

For v0.1, the backend scope is intentionally narrow.

Target:

- generate `to_json`
- generate `from_json`
- support basic scalar fields
- support `std::string`
- target nlohmann/json
- golden-file tests

Out of scope:

- native JSON writer
- custom allocators
- streaming JSON
- SIMD JSON processing
- schema validation
- advanced adapters

---

# Native Backend Direction

A future native backend may provide:

- JSON writer
- JSON reader
- generated serialization helpers
- minimal runtime support
- zero unnecessary dynamic reflection
- optional DOM-free mode

Possible future usage:

```cpp
std::string json = cjm::to_json(user);

User user = cjm::from_json<User>(json);
```

or:

```cpp
cjm::json_writer writer;
cjm::write_json(writer, user);
```

This should be designed only after the initial codegen pipeline is stable.

---

# Dependency Policy

Third-party dependencies are acceptable when they accelerate development.

However, core product value should not permanently depend on unnecessary external libraries.

CJM should avoid becoming a thin wrapper around any single JSON library.

The long-term product should be valuable on its own.

---

# Summary

The initial backend should optimize for progress.

The long-term backend strategy should optimize for independence.

CJM should first prove the build-time code generation model.

Then it can evolve toward a native backend with stronger product control and fewer external dependencies.
