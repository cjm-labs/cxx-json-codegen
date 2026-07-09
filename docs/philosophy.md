# CJM Philosophy

This document describes the core design principles of CJM.

Every architectural decision should be consistent with these principles.

Implementation details may evolve over time.

The philosophy should remain stable.

---

# Standard C++ In. Standard C++ Out.

This is the single most important principle of CJM.

Users write ordinary C++.

CJM generates ordinary C++.

No proprietary language.

No custom syntax.

No special compiler.

No language extensions.

Generated code should be readable, debuggable, and compilable by any standard-conforming C++ compiler.

Everything else is an implementation detail.

---

# Build-Time, Not Runtime

Code generation happens during the build.

There should be **zero runtime overhead** introduced by CJM itself.

The generated code should behave exactly as if it had been handwritten.

The compiler should be able to fully optimize the generated code.

---

# User Experience Comes First

The purpose of CJM is not to demonstrate compiler technology.

The purpose is to improve the developer experience.

Users should spend their time writing application code instead of repetitive serialization logic.

Whenever implementation complexity conflicts with user simplicity, implementation complexity is preferred.

---

# Hide Complexity

Parsing.

AST construction.

Metadata analysis.

Dependency tracking.

Incremental generation.

Code generation.

These are implementation details.

Users should not need to understand how CJM works internally in order to use it effectively.

---

# CMake First

CJM should integrate naturally into existing CMake projects.

Users should not need to learn a new build system.

A typical project should require only a small amount of additional CMake configuration.

---

# No Macros

Macros modify the language before compilation.

They are difficult to debug.

They often produce poor diagnostics.

They hide generated behavior.

CJM avoids intrusive macro-based APIs whenever possible.

The preferred interface is ordinary C++ source code.

---

# No Compiler Plugins

Users should not be required to install compiler plugins or patched compilers.

Compiler-specific implementations are acceptable internally.

Compiler-specific workflows are not acceptable for users.

The user experience should remain portable across platforms.

---

# Readable Generated Code

Generated code is not an internal artifact.

It should be:

- readable
- deterministic
- debuggable
- easy to inspect

Generated files should help developers understand what CJM produces.

They should never feel like opaque binary output.

---

# Portability

Portability is a product requirement.

Users should not need to care about:

- operating systems
- compiler versions
- parser implementations

CJM should minimize external requirements whenever practical.

---

# Predictability

Code generation should be deterministic.

The same input should always produce the same output.

Generated files should remain stable unless the source code changes.

Stable output improves:

- code review
- debugging
- incremental builds
- reproducibility

---

# Simplicity Over Cleverness

CJM values maintainability over sophisticated implementation techniques.

Simple designs are easier to:

- debug
- document
- extend
- optimize

A small amount of implementation duplication is acceptable if it significantly improves readability.

---

# Technology Is Not the Product

CJM is a product.

Clang.

LibTooling.

ASTs.

Parsers.

Generators.

These are implementation choices.

They may change in future versions without affecting the public interface.

Users should think about CJM—not about how CJM is implemented.

---

# Long-Term Vision

CJM aims to become a production-quality developer tool that integrates naturally into modern C++ development workflows.

The project prioritizes:

- developer experience
- maintainability
- portability
- performance
- engineering quality

Every new feature should strengthen these principles rather than compromise them.
