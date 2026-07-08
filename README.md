# cxx-json-map

A Clang-based C++ struct-to-JSON mapping code generator.

## Goal

`cxx-json-map` generates explicit JSON mapping code from annotated C++ structs.

Initial target:

```text
public aggregate struct
-> generated to_json/from_json code
-> first backend: nlohmann/json
```

The project starts with JSON only. The internal design should avoid needless
JSON-only assumptions where practical, but non-JSON formats are out of scope
for the MVP.

## Why

C++ does not currently have a standard Go-style struct tag mechanism for JSON
mapping.

This project explores a compiler-tooling approach:

```text
C++ source
-> Clang AST discovery
-> metadata model
-> generated JSON mapping code
```

The generated code should be readable, reviewable, and built by a normal C++
compiler.

## Initial Direction

Preferred implementation strategy:

```text
standalone Clang LibTooling code generator
```

Not the first approach:

- LLVM IR pass
- Clang compiler plugin
- handwritten C++ parser
- generic serialization framework

## MVP Scope

Support first:

- opt-in annotated structs
- public aggregate fields
- `to_json` generation
- nlohmann/json backend
- primitive scalar fields
- `std::string`
- simple nested supported structs
- `std::vector<T>` where `T` is supported
- golden-file tests

Avoid initially:

- private fields
- inheritance
- polymorphism
- pointer graphs
- cyclic references
- XML/YAML/TOML support
- future standard reflection dependency

## Example

Input:

```cpp
#include <cstdint>

#include "cxx_json_map/annotations.hpp"

struct [[cjm::json]] ConsumerLatency {
  std::uint64_t consumer_id;
  std::uint64_t count;
  std::uint64_t p50_ns;
};
```

Generated output:

```cpp
inline void to_json(nlohmann::json &j, const ConsumerLatency &v) {
  j = nlohmann::json{
      {"consumer_id", v.consumer_id},
      {"count", v.count},
      {"p50_ns", v.p50_ns},
  };
}
```

## Planned Layout

```text
cxx-json-map/
├── README.md
├── CMakeLists.txt
├── AGENTS.md
├── docs/
│   ├── design.md
│   └── roadmap.md
├── include/
│   └── cxx_json_map/
│       └── annotations.hpp
├── tools/
│   └── cxx-json-map/
│       └── main.cpp
├── examples/
│   └── basic/
│       ├── bench_result.hpp
│       └── main.cpp
└── tests/
    └── golden/
        ├── basic.input.hpp
        └── basic.expected.hpp
```

## Documentation

Start here:

- [Design](docs/design.md)
- [Roadmap](docs/roadmap.md)
