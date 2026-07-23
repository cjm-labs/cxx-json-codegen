# CJM Discussions

CJM is entering an early-adopter phase.

The project is looking for practical feedback from developers who use Modern
C++ and CMake on real model headers.

## Best Discussion Topics

Good topics include:

- whether CJM fits your current C++ model workflow;
- CMake integration friction;
- parser limitations you hit with ordinary headers;
- diagnostics that were hard to understand;
- practical type mappings needed before v1.0;
- how generated code should look and feel;
- downstream adoption stories.

## What To Share

The most helpful discussion includes:

- a small C++ model snippet;
- the JSON shape you want;
- whether you tried CLI or CMake integration;
- what worked;
- what felt awkward;
- what blocked you.

## Where To Post

Use [GitHub Discussions](https://github.com/cjm-labs/cxx-json-codegen/discussions)
for open-ended design, adoption, and usage feedback.

Use GitHub Issues for concrete bugs, reproducible failures, and scoped feature
requests.

If you are not sure which one to use, open a question issue. The maintainers can
move or split the topic later.

## Current Positioning

CJM is not a JSON library.

CJM is a build-time metadata compiler for Modern C++. JSON is the first
official backend.

The current release is intended for early adopters who want to try build-time
JSON integration and help shape the v1.0 path.
