# AGENTS.md

## Collaboration Goal

This repository is both a compiler-tooling project and a learning vehicle.

When assisting with this repository, optimize for:

1. Correct implementation
2. Engineering understanding
3. Compiler-tooling fundamentals
4. Incremental production-quality design
5. Long-term skill development

Do not optimize only for generating code quickly.

---

## Project Mission

`cxx-json-map` is a Clang-based C++ struct-to-JSON mapping code generator.

The initial product goal is narrow:

```text
annotated public C++ aggregate structs
-> generated JSON mapping code
-> first backend: nlohmann/json
```

The long-term engineering goal is broader:

```text
C++ source model discovery
-> stable intermediate metadata model
-> backend emitters
```

However, non-JSON formats are explicitly out of scope for the MVP.

---

## Learning Style

The user learns best through:

```text
problem
-> design
-> small implementation
-> test
-> observation
-> documentation
```

Prefer mentor-guided development over large unexplained code generation.

When proposing changes:

1. Explain the problem being solved.
2. Explain the design choice.
3. Mention important trade-offs.
4. Keep patches incremental.
5. Suggest validation commands.
6. Point out what should be understood from the change.

---

## Technical Direction

Preferred architecture:

```text
standalone Clang LibTooling code generator
```

Do not start with:

- LLVM IR pass
- Clang compiler plugin
- custom handwritten C++ parser
- generic serialization framework

Reasoning:

- JSON mapping needs source-level C++ metadata: field names, types,
  namespaces, attributes, and access information.
- Clang AST is the right first layer for this information.
- LLVM IR is too low-level and loses much of the C++ source model.
- A standalone code generator keeps integration closer to protobuf,
  flatbuffers, and capnproto.

---

## MVP Scope

The first version should support:

- opt-in annotated structs
- public aggregate fields
- `to_json` generation
- nlohmann/json backend
- primitive scalar fields
- `std::string`
- simple nested supported structs
- `std::vector<T>` where `T` is supported
- golden-file tests for generated output

Do not support initially:

- private fields
- inheritance
- polymorphism
- pointer graphs
- cyclic references
- arbitrary templates
- custom allocators
- `std::variant`
- full XML/YAML/TOML support
- standard reflection integration

---

## JSON vs Other Formats

The repository name and first product are JSON-specific.

The internal design should avoid unnecessary JSON-only assumptions when doing
so is cheap, but the project should not drift into a generic serialization
framework too early.

Good boundary:

```text
MVP: C++ struct -> nlohmann/json mapping
Architecture: metadata model that could support other emitters later
```

Non-JSON formats such as XML, YAML, TOML, or MessagePack can be considered only
after the JSON codegen path is useful and tested.

---

## Documentation Expectations

For each meaningful feature, include:

- What problem it solves
- What C++/Clang concept it exercises
- What limitation remains
- How to test it
- What the next logical step is

Prefer short design notes that explain trade-offs over vague roadmaps.

---

## Avoid

- Large unexplained code dumps
- Premature abstraction
- Building a generic serialization framework before JSON works
- Depending on future C++ standard reflection for the MVP
- Hiding Clang/AST concepts behind too much magic
- Adding XML/YAML/TOML before JSON is production-quality
