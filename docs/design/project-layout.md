# Project Layout

This document defines the directory structure of the CJM repository.

The layout is designed for long-term maintainability rather than the minimum number of directories.

---

# Repository Structure

```
cxx-json-codegen/

├── CMakeLists.txt
├── README.md
├── LICENSE
├── ROADMAP.md
├── ARCHITECTURE.md
├── CONTRIBUTING.md
│
├── cmake/
│
├── docs/
│
├── include/
│   └── cjm/
│
├── src/
│
├── tools/
│
├── examples/
│
├── tests/
│
├── benchmark/
│
└── third_party/
```

---

# include/

Public API.

Everything installed by CJM should live here.

Example:

```
include/

    cjm/

        version.hpp

        api.hpp

        generated.hpp
```

Users include headers from this directory.

---

# src/

Implementation.

Example:

```
src/

    parser/

    semantic/

    metadata/

    generator/

    backend/

    cli/

    cmake/
```

Implementation details should never leak into public headers.

---

# tools/

Developer tools.

Example:

```
tools/

    cjm
```

Eventually this becomes the CLI executable.

---

# docs/

Project documentation.

Architecture.

Vision.

Design.

Branding.

Everything not directly related to compilation belongs here.

---

# examples/

Small example projects.

Examples should remain minimal.

Each example demonstrates one feature.

Example:

```
examples/

    basic/

    nested/

    vector/

    optional/
```

---

# tests/

Unit tests.

Golden tests.

Integration tests.

Suggested layout:

```
tests/

    parser/

    semantic/

    metadata/

    generator/

    integration/

    golden/
```

Each subsystem should be testable independently.

---

# benchmark/

Performance experiments.

Examples:

```
benchmark/

    generator/

    parser/

    serialization/
```

Benchmarks should never replace correctness tests.

---

# cmake/

Reusable CMake modules.

Example:

```
cmake/

    CJMConfig.cmake

    CJMGenerate.cmake
```

Users should never need to modify these files.

---

# third_party/

Optional third-party dependencies.

Examples:

```
third_party/

    nlohmann/

    Catch2/
```

Dependencies should remain isolated.

Core CJM logic should never be mixed with vendor code.

---

# Source Layering

```
CLI

↓

Parser

↓

Semantic Analysis

↓

Metadata Model

↓

Generator

↓

Backend
```

Dependencies should flow downward only.

Higher layers should not be referenced by lower layers.

---

# Design Principles

The repository structure should:

- separate public and private APIs
- isolate implementation details
- minimize coupling
- support unit testing
- support future backends
- support future parser implementations

The directory structure should evolve slowly.

Frequent restructuring should be avoided.

---

# Summary

The repository layout reflects the CJM architecture.

Each directory should have a clear responsibility.

As CJM grows, new functionality should fit naturally into the existing structure instead of introducing parallel hierarchies.
