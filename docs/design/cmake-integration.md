# CMake Integration

CMake is the primary build-system integration target for CJM.

The goal is to make CJM feel like a native part of an existing CMake-based C++ project.

Users should not need to manually invoke the `cjm` executable during normal development.

---

# Goals

The CMake integration should provide:

- simple user-facing API
- automatic invocation of the CJM generator
- generated include directory management
- dependency tracking
- incremental rebuild support
- clear diagnostics
- minimal user configuration

---

# Basic Usage

A typical project should look like this:

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

This should be enough for a simple project.

---

# User-Facing API

The primary CMake function is:

```cmake
cjm_generate(...)
```

Initial signature:

```cmake
cjm_generate(
    TARGET <target>
    HEADERS <header>...
)
```

Example:

```cmake
cjm_generate(
    TARGET app
    HEADERS
        include/user.hpp
        include/order.hpp
)
```

---

# Discovery Modes

CJM supports two conceptual discovery modes.

## Explicit Mode

Users list headers manually.

This is the default and most predictable mode.

```cmake
cjm_generate(
    TARGET app
    HEADERS
        include/user.hpp
        include/order.hpp
)
```

## Discovery Mode

Future versions may support automatic header discovery.

```cmake
cjm_generate(
    TARGET app
    DISCOVER
    ROOTS
        include
)
```

Discovery mode scans selected roots for CJM metadata.

It should never scan the entire project implicitly.

Explicit boundaries prevent accidental processing of third-party or generated headers.
---

# Generated Output

Generated files are placed under the build directory.

Example:

```text
<build-dir>/generated/cjm/user.cjm.hpp
<build-dir>/generated/cjm/order.cjm.hpp
```

The generated include directory is automatically added to the target.

Users should not need to manually include generated paths.

---

# Dependency Tracking

Each input header should map to one or more generated files.

CMake should regenerate output when:

- an input header changes
- CJM executable changes
- generator options change
- relevant dependencies change

The build should avoid unnecessary regeneration.

---

# Generated File Naming

Generated files use the convention:

```text
*.cjm.hpp
```

Example:

```text
user.hpp
    -> user.cjm.hpp
```

If multiple source files share the same base name, CJM should avoid collisions by preserving relative paths or using a deterministic output layout.

---

# Target Integration

`cjm_generate()` should attach generated files to the specified target.

Conceptually:

```cmake
target_include_directories(app PRIVATE <generated-dir>)
target_sources(app PRIVATE <generated-files>)
```

The exact implementation may evolve.

---

# Build-Time Execution

CJM runs before the target is compiled.

Simplified flow:

```text
header changed

    ↓

run cjm generate

    ↓

produce *.cjm.hpp

    ↓

compile target

    ↓

link target
```

---

# Compiler Independence

CJM may use a parser internally.

However, the user's target compiler remains independent.

The generated code should compile with any supported C++ compiler.

CMake integration should not require users to change their compiler.

---

# CLI Invocation

The CMake function should invoke the CLI internally.

Example conceptual command:

```bash
cjm generate \
    --input include/user.hpp \
    --output <build-dir>/generated/cjm/user.cjm.hpp \
    --target app
```

Users normally should not run this manually.

---

# Build Directory Only

Generated files should live in the build directory by default.

They should not be written into the source tree.

This keeps the source tree clean and prevents accidental manual edits.

---

# Debug Mode

Future versions may support a debug option:

```cmake
cjm_generate(
    TARGET app
    HEADERS user.hpp
    DEBUG
)
```

Debug mode may:

- print the generator command
- preserve intermediate metadata
- emit verbose diagnostics
- make generated files easier to inspect

---

# Native Backend Direction

The first implementation may generate code targeting an existing JSON library.

However, the long-term goal is to support a CJM-native JSON backend.

This would reduce third-party dependency risk and improve product stability.

Possible long-term generated APIs:

```cpp
cjm::json_writer writer;
cjm::to_json(writer, value);
```

or:

```cpp
std::string json = cjm::to_json(value);
```

The MVP should not block on this.

The CMake API should remain stable as backends evolve.

---

# Future Options

Potential future options:

```cmake
cjm_generate(
    TARGET app
    HEADERS user.hpp
    BACKEND nlohmann
    OUTPUT_DIR <dir>
    NAMESPACE my_project
    STRICT
)
```

Options should be added conservatively.

The default experience should remain simple.

---

# Error Reporting

Errors from CJM should appear as build errors.

Diagnostics should reference the original user source file.

Example:

```text
include/user.hpp:12:18: unsupported CJM field type: std::map<std::string, int>
```

Users should not need to inspect generated files to understand most errors.

---

# Summary

CMake integration is the primary user interface of CJM.

The user writes C++.

CMake invokes CJM.

CJM generates ordinary C++.

The user's compiler compiles the result.

The experience should feel native, predictable, and portable.
