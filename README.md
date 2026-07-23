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

## Early Adopters Welcome

CJM v0.3.0 is ready for early adopters who want to try build-time JSON
integration for ordinary Modern C++ models.

The current release has been dogfooded through the public
`FetchContent` + `cjm_generate` workflow in
[ull-md-engine](https://github.com/lmingzhi618/ull-md-engine), covering
optionals, vectors, ordered and unordered string-keyed maps, nested generated
structs, enums, ignored fields, `omitempty`, and fixed-width integers.

If CJM fails on a practical model you expected to work, or if the CMake,
diagnostics, or documentation feel confusing, please open an issue with the
smallest reproducible header and build command.

For open-ended usage or design feedback, use
[GitHub Discussions](https://github.com/cjm-labs/cxx-json-codegen/discussions).

See [CONTRIBUTING.md](CONTRIBUTING.md) for bug reports, questions, feature
requests, and pull request guidance.

See the [ull-md-engine dogfood report](docs/dogfood/ull-md-engine-v0.3.0.md)
for the current downstream validation record.

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

## Usage Example

Write ordinary C++ models and put JSON metadata next to the fields:

```cpp
#pragma once

#include <string>

struct User {
    std::string name; // json:"name"
    int age = 0;      // json:"age"
};
```

After building the CLI, run CJM:

```sh
./build/cjm generate \
  --input user.hpp \
  --output user.cjm.hpp
```

CJM generates ordinary C++ integration code:

```cpp
inline void to_json(nlohmann::json& j, const User& value) {
    j["name"] = value.name;
    j["age"] = value.age;
}

inline void from_json(const nlohmann::json& j, User& value) {
    j.at("name").get_to(value.name);
    j.at("age").get_to(value.age);
}
```

Use it like normal `nlohmann/json` code:

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

No macros. No compiler plugins. No runtime reflection.

---

## Quickstart

The fastest way to try CJM today is from this repository checkout.

Requirements:

- CMake
- a C++17 compiler
- `nlohmann/json` available locally or downloadable by CMake

### Run the Example

```sh
cmake -S . -B build
cmake --build build --target cjm_basic_example
./build/examples/basic/cjm_basic_example
```

That builds the CJM CLI, generates `user.cjm.hpp`, compiles the example, and
runs a tiny JSON round trip.

The example model is ordinary C++:

```cpp
#pragma once

#include <string>

struct User {
    std::string name; // json:"name"
    int age = 0;      // json:"age"
};
```

### Generate Code Manually

You can also run the local CLI directly:

```sh
cmake --build build --target cjm
./build/cjm generate \
  --input examples/basic/user.hpp \
  --output /tmp/user.cjm.hpp
```

The generated file contains ordinary C++ `to_json` and `from_json` functions
for `nlohmann/json`.

For multiple related headers, repeat `--input` in the same command:

```sh
./build/cjm generate \
  --input address.hpp \
  --input user.hpp \
  --output model.cjm.hpp
```

CJM does not automatically discover `#include` dependencies yet, so pass every
related model header explicitly.

A less repetitive file-list or manifest-based input workflow is planned for a
future adoption milestone.

### Use the Generated Header

Include your model first, then the generated CJM header:

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

### Use CJM From CMake

Inside this source tree, the example uses `cjm_generate`:

```cmake
add_executable(app main.cpp)

target_link_libraries(app PRIVATE nlohmann_json::nlohmann_json)

cjm_generate(
  TARGET app
  HEADERS user.hpp
)
```

In this snippet:

- `app` is your normal C++ executable.
- `nlohmann_json::nlohmann_json` provides the JSON library used by the
  generated backend code.
- `cjm_generate` wires CJM into the build: it reads `user.hpp`, generates
  `user.cjm.hpp`, and makes the generated header available to `app`.

During the build, CJM generates:

```text
generated/cjm/user.cjm.hpp
```

and adds the generated directory to the target include path.

### Use CJM From A Downstream Project

Early adopters can consume CJM by pinning a release tag with CMake
`FetchContent`:

```cmake
include(FetchContent)

FetchContent_Declare(
  cxx_json_codegen
  GIT_REPOSITORY https://github.com/cjm-labs/cxx-json-codegen.git
  GIT_TAG v0.3.0
)

FetchContent_MakeAvailable(cxx_json_codegen)

add_executable(app main.cpp)
target_link_libraries(app PRIVATE nlohmann_json::nlohmann_json)

cjm_generate(
  TARGET app
  HEADERS user.hpp
)
```

This is the workflow used by the downstream `ull-md-engine` dogfood run.
Packaged installation through `find_package(CJM REQUIRED)` is planned later.

### Try the Full Test Suite

```sh
ctest --test-dir build --output-on-failure
```

### Keep the First Try Smooth

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
- CJM v0.3.0 has been dogfooded in a real downstream CMake project:
  [ull-md-engine](https://github.com/lmingzhi618/ull-md-engine)
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
- [Contributing](CONTRIBUTING.md)
- [Discussions](DISCUSSIONS.md)
- [Roadmap](ROADMAP.md)
- [JSON Mapping Scope](docs/design/json-mapping-scope.md)
- [ull-md-engine Dogfood Report](docs/dogfood/ull-md-engine-v0.3.0.md)
- [Early-Adopter Outreach](docs/community/early-adopter-outreach.md)
- [v0.3.0 Release Notes](docs/releases/v0.3.0.md)
- [Competitive Landscape](docs/design/competitive-landscape.md)
- [Third-Party Notices](THIRD_PARTY_NOTICES.md)
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
