# Early-Adopter Outreach

CJM is ready for a small early-adopter outreach round.

The goal is not broad marketing. The goal is to find a few experienced Modern
C++ developers who will try CJM on practical model headers and report real
friction.

## Target Audience

Look for developers who:

- use Modern C++;
- use CMake;
- write JSON serialization code by hand today;
- maintain model-heavy code;
- care about generated code being readable;
- are willing to report small reproducible cases.

The target is 5 to 10 serious early adopters, not a large star spike.

## Core Message

```text
CJM is a build-time metadata compiler for Modern C++.

It reads ordinary C++ model declarations with Go-style JSON field metadata,
builds a Metadata IR, and generates ordinary nlohmann/json integration code
during the build.

No macros. No runtime reflection. Standard C++ in, standard C++ out.
```

## What To Ask For

Ask early adopters for:

- practical model headers;
- CMake integration feedback;
- parser failure cases;
- confusing diagnostics;
- generated-code readability feedback;
- missing type mappings needed before v1.0.

## What Not To Claim

Do not claim:

- production stability;
- full C++ grammar support;
- that CJM is a JSON library;
- automatic header discovery;
- broad backend support;
- benchmark wins;
- that CJM replaces existing JSON libraries.

## Short Announcement

```md
CJM v0.3.0 is ready for early adopters.

CJM is a build-time metadata compiler for Modern C++. It reads ordinary C++
models annotated with Go-style JSON field metadata and generates ordinary
nlohmann/json integration code during the build.

The v0.3.0 release supports practical model composition: optionals, vectors,
ordered and unordered string-keyed maps, nested generated structs, enums,
ignored fields, omitempty, and fixed-width integers.

It has been dogfooded in a real downstream CMake project using the public
FetchContent + cjm_generate workflow.

I am looking for a few Modern C++ developers to try it on practical model
headers and report parser, CMake, diagnostics, or generated-code friction.

GitHub: https://github.com/cjm-labs/cxx-json-codegen
Release: https://github.com/cjm-labs/cxx-json-codegen/releases/tag/v0.3.0
Dogfood report: https://github.com/cjm-labs/cxx-json-codegen/blob/main/docs/dogfood/ull-md-engine-v0.3.0.md
```

## Reddit r/cpp Draft

Title:

```text
CJM v0.3.0: a compiler-style C++ JSON code generator looking for early adopters
```

Post:

````md
Hi r/cpp,

I have been building CJM, a build-time metadata compiler for Modern C++.

The idea is not to create another JSON library. CJM reads ordinary C++ model
headers with Go-style JSON field metadata, builds a Metadata IR, and generates
ordinary nlohmann/json integration code during the build.

Example:

```cpp
struct User {
    std::string name; // json:"name"
    std::uint64_t id; // json:"id"
};
```

The generated output is normal C++ `to_json` / `from_json` code. There are no
macros, compiler plugins, or runtime reflection systems.

v0.3.0 supports a practical subset:

- scalar fields
- fixed-width integers
- enums
- nested generated structs
- `std::vector<T>`
- `std::optional<T>`
- `std::map<std::string, T>`
- `std::unordered_map<std::string, T>`
- `json:"-"` ignored fields
- `omitempty`

It has now been dogfooded in a real downstream CMake project through the public
`FetchContent + cjm_generate` workflow.

I am looking for early adopters with practical model headers. The most valuable
feedback right now is not stars, but real failure cases:

- parser limitations
- CMake integration friction
- confusing diagnostics
- generated-code readability issues
- missing type mappings needed before v1.0

Repo:
https://github.com/cjm-labs/cxx-json-codegen

Dogfood report:
https://github.com/cjm-labs/cxx-json-codegen/blob/main/docs/dogfood/ull-md-engine-v0.3.0.md

I would appreciate feedback from people who maintain C++ projects with real JSON
model code.
````

## Hacker News Draft For Later

Do not post this until CJM has stronger installation and documentation support.

Title:

```text
Show HN: CJM, a build-time metadata compiler for C++ JSON code generation
```

Short body:

```md
CJM is a build-time metadata compiler for Modern C++.

It reads ordinary C++ model declarations with Go-style JSON metadata, builds a
Metadata IR, and generates ordinary C++ integration code for nlohmann/json.

The goal is compiler-style code generation without macros, runtime reflection,
or schema-first model duplication.

v0.3.0 has been dogfooded in a real downstream CMake project, but it remains an
early-adopter project with a documented practical subset rather than full C++
grammar support.
```

## Response Checklist

When someone gives feedback:

1. Thank them.
2. Ask for the smallest model header if missing.
3. Ask whether they used CLI or CMake integration.
4. Classify the feedback as parser, semantic, generator, CMake, diagnostics, or
   docs.
5. Create a focused issue if it is actionable.
6. Avoid promising broad features before they fit the roadmap.
