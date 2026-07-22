# CJM

> Build-time Metadata Compiler for Modern C++

**CJM** is a build-time metadata compiler for Modern C++.

It extracts source-level metadata from ordinary C++ declarations, builds a
stable Metadata IR, and generates backend-specific C++ code during the build.

The first official backend generates `nlohmann/json` integration from Go-style
field metadata.

Instead of writing repetitive integration code or relying on macros and runtime
reflection, CJM generates ordinary C++ code while keeping your source files
valid, standard C++.

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

CJM aims to provide a compiler-style metadata pipeline while preserving a
Go-like developer experience for its first JSON backend:

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

CJM can be tried directly from the source tree.

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
Adoption roadmap rather than the current release surface.

---

## Use CJM Today

The easiest way to try CJM today is from this repository checkout.

### 1. Build the CLI

```sh
cmake -S . -B build
cmake --build build --target cjm
```

This produces the local CLI:

```text
build/cjm
```

### 2. Write an annotated C++ model

CJM reads ordinary C++ headers. Metadata is written as same-line field
comments:

```cpp
#pragma once

#include <string>

struct User {
    std::string name; // json:"name"
    int age = 0;      // json:"age"
};
```

### 3. Generate a nlohmann/json header

Run the CLI against one or more explicit input headers:

```sh
./build/cjm generate \
  --input examples/basic/user.hpp \
  --output /tmp/user.cjm.hpp
```

For multiple related headers, repeat `--input`:

```sh
./build/cjm generate \
  --input address.hpp \
  --input user.hpp \
  --output model.cjm.hpp
```

The generated file contains ordinary C++ `to_json` and `from_json` functions
for `nlohmann/json`.

### 4. Use the generated header

Include your model header first, then the generated CJM header:

```cpp
#include "user.hpp"
#include "user.cjm.hpp"

#include <nlohmann/json.hpp>

int main() {
    User user;
    user.name = "Ada";
    user.age = 42;

    nlohmann::json json = user;
    User round_trip = json.get<User>();
}
```

### 5. Try the included CMake example

The repository includes a working CMake integration example:

```sh
cmake -S . -B build
cmake --build build --target cjm_basic_example
./build/examples/basic/cjm_basic_example
```

The example uses:

```cmake
cjm_generate(
  TARGET cjm_basic_example
  HEADERS user.hpp
)
```

Current parser notes:

- write one supported field declaration per line
- put `json:"..."` metadata in a same-line `//` comment
- use qualified standard types such as `std::string`
- pass every related header explicitly with `--input`

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
 C++ Frontend
        │
        ▼
 Metadata IR
        │
        ▼
 nlohmann/json Backend
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

The Metadata IR is the stable boundary between source-language understanding
and backend-specific code generation.

Users only interact with standard C++ source code, CMake, and generated C++
files.

---

## Project Status

Current status:

- v0.3.0 Practical Type Coverage is the current release line
- CJM has a parser -> semantic analysis -> Metadata IR -> nlohmann backend
  pipeline
- First official backend: `nlohmann/json`
- The supported model surface is still a documented practical subset, not full
  C++ grammar support

---

## Current Mapping Surface

CJM currently supports a practical JSON mapping subset.

Supported:

- explicit CMake header registration with `cjm_generate`
- one or more explicit input headers
- public `struct` declarations in the supported parser subset
- fields with Go-style `json:"name"` comments
- generated `to_json` / `from_json` for `nlohmann/json`
- `bool`
- signed and unsigned integer types
- common fixed-width integer spellings from `<cstdint>`
- floating-point types
- `std::string`
- enums
- nested generated structs
- namespaces
- supported type aliases
- `std::vector<T>`
- `std::optional<T>`
- `std::map<std::string, T>`
- `std::unordered_map<std::string, T>`
- `json:"-"` ignored fields
- `omitempty` for supported optional fields

## Current Limitations

CJM intentionally remains a practical subset.

Not yet supported:

- full C++ parsing 
- automatic header discovery 
- arbitrary dynamic JSON values
- `std::variant`
- `std::any`
- pointer fields
- polymorphism
- custom converters
- enum string mapping policies
- time and datetime mappings
- multiline field declarations in the current handwritten parser
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
- [v0.3.0 Release Notes](docs/releases/v0.3.0.md)
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
