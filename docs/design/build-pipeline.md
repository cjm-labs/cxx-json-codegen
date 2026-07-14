# Build Pipeline

This document describes how CJM integrates into a normal CMake build.

The build pipeline is one of the core design goals of CJM.

Users should not need to learn a new build system.

Instead, CJM becomes another step in an existing CMake workflow.

---

# Overview

A simplified build pipeline looks like this:

```
        Configure
            │
            ▼
     CMake Generation
            │
            ▼
      CJM Code Generator
            │
            ▼
 Generate *.cjm.hpp Files
            │
            ▼
     Normal C++ Compiler
            │
            ▼
          Linker
            │
            ▼
        Final Binary
```

The generated files become ordinary C++ source that is compiled together with the rest of the project.

---

# Step 1 — Configure

The user writes normal C++.

Example:

```cpp
struct User {
    std::string name;   // json:"name"
    int age;            // json:"age"
};
```

CMake configuration:

```cmake
find_package(CJM REQUIRED)

add_executable(app
    main.cpp
)

cjm_generate(
    TARGET app
    HEADERS
        user.hpp
)
```

Nothing special happens during compilation.

The generation process is explicitly managed by CMake.

---

# Step 2 — Metadata Extraction

Before compilation begins, CJM scans the specified header files.

Responsibilities include:

- parsing C++ source
- discovering annotated declarations
- extracting metadata
- validating metadata

The result is an internal Metadata Model.

Parser implementation details are intentionally hidden.

---

# Step 3 — Code Generation

The Metadata Model is transformed into ordinary C++.

Generated files follow the naming convention:

```
user.cjm.hpp
```

Generated code may include:

- to_json()
- from_json()
- helper functions
- metadata utilities

The generated files are deterministic.

The same input always produces the same output.

---

# Step 4 — Compilation

Generated files participate in the normal compilation process.

```
main.cpp

user.hpp

user.cjm.hpp
```

↓

```
Compiler
```

↓

```
Object Files
```

No custom compiler is required.

No compiler plugins are required.

---

# Step 5 — Linking

Linking is completely unchanged.

The linker is unaware that CJM participated in the build.

The final executable contains only ordinary compiled C++.

---

# Generated Files

Generated files are considered build artifacts.

They should:

- remain readable
- be deterministic
- be easy to debug
- integrate naturally with IDEs

Generated code should never appear "magical."

Developers should always be able to inspect the generated implementation.

---

# Incremental Builds

Only modified source files should trigger regeneration.

Typical workflow:

```
user.hpp changed

↓

Regenerate user.cjm.hpp

↓

Recompile affected translation units

↓

Link
```

Future versions will optimize this process further.

---

# Error Reporting

Errors should be reported as early as possible.

Whenever practical:

- point to the original source file
- preserve original line numbers
- provide actionable diagnostics

Generated files should not become the primary source of user-visible errors.

---

# Design Principles

The build pipeline follows several principles:

- Build-time execution
- Standard C++ output
- Deterministic generation
- CMake-first integration
- Compiler independence
- Minimal user configuration

---

# Current v0.1 End-to-End Example 

The current end-to-end example demonstrates the complete CJM build pipeline:

```text 
C++ header 
↓
Parser 
↓
Semantic Analysis 
↓
Metadata Model
↓
Code Generator 
↓
Generated *.cjm.hpp
↓
Normal C++ compilation 
↓
Executable test
```
The example uses `examples/basic/user.hpp` as input and generates `user.cjm.hpp` under the build directory.
The generated header targets the initial `nlohmann/json` backend. The example links `nlohmann_json::nlohmann_json` so the generated code can be included, compiled, and executed.

Current limitations:
- the parser supports only a minimal v0.1 struct shape 
- semantic analysis supports only simple `json:"name" field metadata 
- generated code currently targets `nlohmann/json` 
- native JSON backend supports is not part of the current example 
- advanced C++ constructs, templates, methods, and complex type resolution are out of scope


---

# Summary

The user experience should feel simple:

```
Write C++

↓

Run CMake

↓

Build

↓

Done
```

The entire generation pipeline should disappear into the build process.

Developers should think about writing C++, not about running a code generator.
