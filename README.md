# CJM

> Go-style JSON tags for Modern C++

**CJM** is a build-time code generator that brings Go-style JSON tags to standard C++.

Instead of writing repetitive serialization code or relying on macros and runtime reflection, CJM generates ordinary C++ code during the build process while keeping your source files valid, standard C++.

CJM keeps your C++ models as the source of truth. It generates the JSON integration around them, not the models themselves.

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

## Try From Source

CJM v0.1 can be tried directly from the source tree.

Requirements:

- CMake
- a C++17 compiler
- `nlohmann/json`

Build and run the included example:

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

The basic example lives in:

```text
examples/basic/
```

It demonstrates:

- a standard C++ model in `user.hpp`
- field metadata written next to the fields
- generated `user.cjm.hpp`
- normal `nlohmann::json` conversion

Packaged installation through `find_package(CJM REQUIRED)` is part of the
Adoption roadmap rather than the current v0.1 release surface.

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

- v0.1.0 has been released 
- Minimal end-to-end JSON code generation pipeline is available 
- Initial backend: `nlohmann/json` 

---

## Current Limitations 

CJM v0.1 is intentionally minimal.

Supported:

- explicit CMake header registration with `cjm_generate`
- simple `struct` declarations 
- simple fields with `json:"name"` comments 
- generated `to_json` / `from_json` for `nlohmann/json` 
- basic scalar and `std::string` field spelling 

Not yet supported:

- full C++ parsing 
- automatic header discovery 
- nested structs 
- STL containers 
- templates 
- private fields 
- native JSON backend 
- install/package distribution

---

## Roadmap

See [ROADMAP.md](ROADMAP.md) for the current product roadmap.

---

## Documentation

- [Project Vision](docs/vision.md)
- [Architecture](ARCHITECTURE.md)
- [Philosophy](docs/philosophy.md)
- [Roadmap](ROADMAP.md)
- [JSON Mapping Scope](docs/design/json-mapping-scope.md)
- [Competitive Landscape](docs/design/competitive-landscape.md)
- [Design Notes](docs/design/)

---

## About

CJM is an open-source project developed by **CJM Labs**.

🌐 https://cjm-labs.org

GitHub Organization:

https://github.com/cjm-labs

---

## License

CJM is licensed under the MIT License. See [LICENSE](LICENSE).

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
