# CJM

> Go-style JSON tags for Modern C++

**CJM** is a build-time code generator that brings Go-style JSON tags to standard C++.

Instead of writing repetitive serialization code or relying on macros and runtime reflection, CJM generates ordinary C++ code during the build process while keeping your source files valid, standard C++.

> **Standard C++ in. Standard C++ out.**

---

## Why CJM?

Modern C++ still lacks a simple and ergonomic way to associate metadata with user-defined types.

Other languages provide elegant solutions:

- **Go** → Struct Tags
- **Rust** → Derive Macros
- **C#** → Attributes
- **Java** → Annotations

C++ developers often have to choose between:

- intrusive macros
- handwritten serialization code
- runtime reflection libraries
- compiler-specific extensions

CJM aims to provide a Go-like developer experience while remaining:

- Build-time only
- Standard C++
- Compiler independent from the user's perspective
- Easy to integrate with existing CMake projects

---

## Example

The target user experience is intentionally simple.

```cpp
struct User {
    std::string name;   // json:"name"
    int age;            // json:"age"
};
```

Configure your project:

```cmake
find_package(CJM REQUIRED)

add_executable(app main.cpp)

cjm_generate(
    TARGET app
    HEADERS
        user.hpp
)
```

During the build, CJM generates:

```text
user.cjm.hpp
```

which contains ordinary C++ serialization code.

No macros.

No compiler plugins.

No runtime reflection.

---

## Design Philosophy

CJM follows a few core principles:

- Standard C++ in.
- Standard C++ out.
- Build-time code generation.
- CMake-first workflow.
- Keep implementation details hidden from users.
- Make generated code readable and debuggable.

---

## Architecture

```
User C++ Source
        │
        ▼
   CMake Build
        │
        ▼
      CJM
        │
        ▼
 Generated C++
        │
        ▼
   Normal Compiler
        │
        ▼
     Executable
```

The parser and code generator are implementation details.

Users only interact with standard C++ source code and generated C++ files.

---

## Project Status

Current status:

- 🚧 Early development
- Private development before the first public release
- Initial target: **v0.1**

---

## Roadmap

- v0.1 — Basic JSON code generation
- v0.2 — Nested types and STL containers
- v0.3 — Incremental generation
- v0.4 — Performance optimizations
- v1.0 — Production-ready release

See [ROADMAP.md](ROADMAP.md) for details.

---

## Documentation

- Project Vision
- Architecture
- Branding
- Philosophy
- Design Notes

(Documentation is under active development.)

---

## About

CJM is an open-source project developed by **CJM Labs**.

🌐 https://cjm-labs.org

GitHub Organization:

https://github.com/cjm-labs

---

## License

MIT License

## Golden Tests

CJM uses golden tests to protect generated C++ output.

Golden files live under:

```text
tests/golden/
```

Expected files are committed:

```text
*.expected.cjm.hpp
```

Actual files are local failure artifacts:

```text
*.actual.cjm.hpp
```

When a golden test fails, the test writes an actual file and prints both paths:

```text
golden mismatch
expected: tests/golden/example.expected.cjm.hpp
actual: tests/golden/example.actual.cjm.hpp
```

To inspect the change:

```bash
diff -u tests/golden/example.expected.cjm.hpp tests/golden/example.actual.cjm.hpp
```

If the generated output change is intentional, update the expected file after review:

```bash
cp tests/golden/example.actual.cjm.hpp tests/golden/example.expected.cjm.hpp
```

Do not commit `*.actual.*` files. They are ignored by git and exist only to help inspect local failures.
