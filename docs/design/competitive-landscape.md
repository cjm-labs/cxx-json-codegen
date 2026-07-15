# CJM Competitive Landscape

This document describes CJM's competitive landscape by product model rather than
by listing JSON libraries.

CJM should be compared by:

- source of truth
- metadata location
- generation time
- output model
- backend strategy
- developer workflow

This helps keep CJM's product boundary clear.

---

# Product Position

CJM is a metadata-driven source generator for Modern C++.

Its core pipeline is:

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
    v
Backend Generator
    |
    v
Generated C++
```

CJM keeps user-owned C++ models as the source of truth.

It generates JSON integration around those models rather than generating the
models themselves.

---

# Product Categories

## Category 1: C++ Source to Generated Integration

These tools start from C++ source and generate integration code.

### CJM

Source of truth:

- C++ source

Metadata location:

- next to the field it describes

Generation model:

- build-time generation

Output:

- ordinary generated C++

Characteristics:

- C++ models remain user-owned
- metadata lives inline with source fields
- generated code is readable and inspectable
- CMake-native workflow
- backend-independent architecture
- Metadata IR separates parsing from generation

Goal:

> Keep existing C++ models as the source of truth and generate integration
> around them.

### reflective-rapidjson

Repository:

- https://github.com/Martchus/reflective-rapidjson

Characteristics:

- Clang-based frontend
- RapidJSON backend
- reflection-oriented generated API
- strong type support
- supports many STL containers

Strengths:

- mature parser foundation
- strong C++ type coverage
- RapidJSON performance profile

Tradeoffs:

- depends heavily on Clang
- exposes more reflection-oriented concepts
- higher integration complexity
- less aligned with CJM's product goal of hiding parser implementation details

### json-cpp-gen

Repository:

- https://github.com/Chlumsky/json-cpp-gen

Characteristics:

```text
C++ Header
    |
    v
Header Parser
    |
    v
Generator
    |
    v
Standalone Parser / Serializer
```

Strengths:

- standalone runtime
- high-performance direction
- no required nlohmann/json dependency
- mature parser/generator approach

Tradeoffs:

- metadata is external
- source and configuration are separated
- more setup/configuration required
- less focused on inline metadata next to C++ fields

---

## Category 2: Runtime Reflection Libraries

These libraries provide runtime or compile-time reflection-like APIs and use
that metadata for serialization.

### reflect-cpp

Website:

- https://rfl.getml.com

Characteristics:

```text
Reflection
    |
    v
Serialization
    |
    v
Many Formats
```

Supported formats include:

- JSON
- YAML
- XML
- CBOR
- MessagePack
- Avro
- BSON
- CSV
- Parquet

Strengths:

- modern C++20 design
- rich validation
- many serialization formats
- strong ecosystem direction

Tradeoffs:

- runtime/library-centered approach
- not primarily a build-time code generator
- broader abstraction than CJM
- user adopts a reflection framework rather than a small source generator

### StaticJSON

Repository:

- https://github.com/netheril96/StaticJSON

Characteristics:

```text
Struct
    |
    v
Registration API
    |
    v
RapidJSON Runtime
```

Strengths:

- strong validation
- mature runtime registration model
- JSON Schema support
- custom converter support

Tradeoffs:

- duplicate field registration
- runtime library dependency
- metadata is not colocated with fields
- not generated ordinary C++ integration code

---

## Category 3: Schema-First Code Generation

These tools start from JSON, JSON Schema, or another schema definition and
generate models.

### quicktype

Website:

- https://quicktype.io

Pipeline:

```text
JSON Sample / JSON Schema
    |
    v
Type Inference
    |
    v
Generated Models
```

Strengths:

- excellent onboarding from existing JSON
- multi-language output
- useful for API exploration and migration

Tradeoffs:

- JSON or schema owns the model
- generated model is not user-owned source
- opposite direction from CJM

### json-schema-codegen

Repository:

- https://github.com/pearmaster/json-schema-codegen

Pipeline:

```text
JSON Schema
    |
    v
Generated Models
```

Strengths:

- strong contract-first workflow
- validation-oriented
- useful when schema is the source of truth

Tradeoffs:

- schema owns the model
- not source-first
- not aligned with CJM's C++ source ownership model

### JSON Structure

Website:

- https://json-structure.org

Characteristics:

- schema-centered workflow
- multi-schema and multi-language direction
- closer to OpenAPI, Avro, or Protobuf style tooling than to a C++ JSON
  integration generator

Tradeoffs:

- not C++ source-first
- not focused on build-time integration for existing C++ models

---

## Category 4: JSON Developer Workspaces

These tools improve productivity around JSON data itself.

### JsonStudio

Website:

- https://jsonstudio.js.org

Characteristics:

- formatter
- editor
- validator
- diff tool
- generator
- playground

Goal:

- developer productivity around JSON documents

Tradeoffs:

- not a C++ serialization or source generation framework

### json-render

Website:

- https://json-render.dev

Characteristics:

- JSON-driven UI rendering
- turns JSON-like structures into user interfaces

Tradeoffs:

- unrelated to C++ serialization
- not a source generation tool

---

# Common Design Dimensions

Every related product can be analyzed through these dimensions.

## Source of Truth

Possible sources of truth:

- C++ source
- runtime reflection
- JSON sample
- JSON Schema
- external configuration
- registration API
- proprietary schema language

CJM chooses:

- C++ source

## Metadata Location

Possible metadata locations:

- inline with fields
- external JSON/YAML configuration
- runtime registration calls
- reflection declarations
- schema files
- generated model annotations

CJM chooses:

- metadata next to the field it describes

## Parser Strategy

Possible parser strategies:

- Clang
- self-written parser
- Tree-sitter
- reflection system
- no parser

CJM chooses:

- parser implementation as an internal detail
- parser-independent Metadata IR

## Semantic Model

Possible semantic models:

- explicit IR
- reflection graph
- runtime handler table
- schema tree
- generated model AST

CJM chooses:

- explicit Metadata IR

## Backend Model

Possible backends:

- nlohmann/json
- RapidJSON
- native runtime
- schema output
- documentation output
- multiple serialization formats

CJM starts with:

- nlohmann/json

CJM may later support:

- RapidJSON backend
- native backend
- JSON Schema backend
- documentation backend

## Generation Time

Possible generation times:

- build time
- compile time
- runtime
- offline/manual generation

CJM chooses:

- build time

## Output

Possible outputs:

- generated C++
- runtime serialization behavior
- generated models
- schema
- documentation
- UI

CJM primarily outputs:

- generated ordinary C++

Future CJM backends may output:

- JSON Schema
- documentation

## Dependency Model

Possible dependency models:

- header-only library
- runtime library
- Clang toolchain
- reflection framework
- third-party JSON library
- standalone CLI

CJM should avoid making any single implementation dependency part of the product
identity.

---

# CJM Differentiation

CJM intentionally chooses:

- C++ source as the source of truth
- inline metadata
- build-time generation
- ordinary generated C++
- backend-independent architecture
- CMake-native workflow
- parser independence through Metadata IR

CJM intentionally avoids:

- runtime reflection
- runtime registration
- schema-first model generation
- external generator configuration as the primary workflow
- proprietary DSLs
- exposing parser implementation details as product identity

---

# Long-Term Backend Evolution

The long-term backend direction is:

```text
Metadata IR
    |
    +-- nlohmann/json backend
    |
    +-- RapidJSON backend
    |
    +-- Native backend
    |
    +-- JSON Schema backend
    |
    +-- Documentation backend
    |
    +-- OpenAPI backend
    |
    +-- Reflection backend
```

These are possible directions, not immediate commitments.

The important architectural rule is:

> Backends consume the Metadata IR. They do not depend on parser-specific AST
> nodes.

---

# Projects to Monitor

## Reflection and C++ Metadata

- reflect-cpp
- Boost.PFR
- Boost.Hana
- magic_enum
- Glaze
- visit_struct

## JSON Libraries

- nlohmann/json
- RapidJSON
- simdjson
- yyjson
- DAW JSON Link

## Code Generation

- quicktype
- json-cpp-gen
- reflective-rapidjson
- Protocol Buffers
- FlatBuffers
- Cap'n Proto
- Bond
- Apache Thrift

## Compiler Infrastructure

- Clang LibTooling
- Clang AST Matchers
- Tree-sitter C++
- cppast
- CppHeaderParser

## Serialization

- cereal
- Boost.Serialization
- bitsery
- Protocol Buffers
- FlatBuffers

---

# Broader Developer Tooling References

CJM should not learn only from JSON libraries.

It should also study mature developer tools and code generators:

- Protocol Buffers
- FlatBuffers
- Cap'n Proto
- Rust Serde
- sqlc
- ent
- oapi-codegen
- Wire
- gqlgen

Useful lessons include:

- IR design
- generator architecture
- backend boundaries
- plugin architecture
- CLI design
- build integration
- versioning
- diagnostics
- developer experience
- release process

CJM is best understood as a Modern C++ developer tool, not merely a JSON
library wrapper.

---

# Strategic Conclusion

The competitive landscape supports CJM's current direction.

CJM should not pivot into:

- a JSON library
- a runtime reflection framework
- a schema-first generator
- a JSON editor
- a general serialization runtime

Instead, CJM should deepen its existing direction:

> C++ source as the source of truth, Metadata IR as the architectural center,
> and generated ordinary C++ as the output.

The roadmap should evolve by improving practicality, adoption, extensibility,
schema output, performance, reliability, documentation, ecosystem fit, and
production readiness.
