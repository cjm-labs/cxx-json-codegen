# Contributing to CJM

Thanks for helping CJM become a better build-time metadata compiler for Modern
C++.

CJM is early-stage and actively looking for practical feedback from real C++
projects. The most useful contribution right now is a small, reproducible model
header or CMake integration case that shows what works, what feels awkward, or
what fails.

## Good First Feedback

Please open an issue if you have:

- a supported-looking C++ model that CJM cannot parse or analyze;
- generated code that does not compile;
- confusing diagnostics;
- CMake integration friction;
- a practical type mapping that seems important for v1.0;
- documentation that was unclear while trying CJM.

For now, prefer focused reports over broad wish lists.

## Bug Reports

A good bug report includes:

- CJM version or commit;
- operating system and compiler;
- CMake version;
- the smallest header that reproduces the problem;
- the `cjm generate` command or `cjm_generate(...)` snippet;
- the full error message or diagnostic;
- whether the issue happens in CJM itself or only from a downstream project.

Small reproducible examples are much more helpful than screenshots.

## Feature Requests

Feature requests should describe the real C++ model or workflow that needs the
feature.

Helpful context:

- the exact C++ type or declaration shape;
- the JSON shape you expect;
- whether the feature belongs in Parser, Semantic Analysis, Metadata IR,
  Generator, CMake integration, CLI, or docs;
- whether you are blocked or only asking for future planning.

CJM intentionally does not support the full C++ type system yet. Feature
requests are most useful when they explain a practical project need.

## Questions

If you are unsure whether your model is supported, open a question issue with a
small header snippet.

Please include what you expected CJM to generate and where the README or design
docs became unclear.

## Pull Requests

Pull requests are welcome, but please open an issue first for non-trivial
changes.

CJM is architecture-driven. Before changing behavior, read:

1. [AGENTS.md](AGENTS.md)
2. [ARCHITECTURE.md](ARCHITECTURE.md)
3. [docs/philosophy.md](docs/philosophy.md)
4. [docs/vision.md](docs/vision.md)
5. [ROADMAP.md](ROADMAP.md)

Please keep pull requests focused:

- one concept per PR when practical;
- generated output changes should include golden tests;
- parser changes should not leak into backends;
- generator changes should consume Metadata IR only;
- public workflow changes should update README or design docs.

## Current Early-Adopter Scope

CJM v0.3.0 is ready for early adopters, not production-stability claims.

Good trial projects:

- small model headers;
- JSON logging records;
- configuration snapshots;
- telemetry or benchmark-result structs;
- internal tools where generated code can be inspected.

Avoid expecting CJM v0.3.0 to handle:

- full C++ grammar;
- arbitrary templates;
- `std::variant`;
- `std::any`;
- pointer fields;
- polymorphic serialization;
- enum string mapping policies;
- automatic header discovery.

See the [current mapping scope](docs/design/json-mapping-scope.md) and the
[ull-md-engine dogfood report](docs/dogfood/ull-md-engine-v0.3.0.md).
