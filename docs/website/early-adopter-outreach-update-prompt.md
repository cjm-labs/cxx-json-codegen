# Website Update Prompt for CJM Early-Adopter Outreach

Use this prompt for the AI assistant working on `cjm-labs.org`.

````md
# Task: Update cjm-labs.org for CJM early-adopter outreach

## Context

CJM is a build-time metadata compiler for Modern C++.

The current public release is CJM v0.3.0, the Practical Type Coverage release.
JSON is the first official backend, implemented through generated
`nlohmann/json` integration code.

CJM v0.3.0 has been dogfooded in a real downstream CMake project:

- [lmingzhi618/ull-md-engine](https://github.com/lmingzhi618/ull-md-engine)

The repo now has early-adopter feedback channels:

- GitHub Discussions: https://github.com/cjm-labs/cxx-json-codegen/discussions
- Early-adopter feedback thread: https://github.com/cjm-labs/cxx-json-codegen/discussions/129
- Issues: https://github.com/cjm-labs/cxx-json-codegen/issues
- Contributing guide: https://github.com/cjm-labs/cxx-json-codegen/blob/main/CONTRIBUTING.md

## Goal

Update the website so it can receive early adopters from LinkedIn, Reddit, and
GitHub without overselling CJM.

The site should make three things clear:

1. CJM is ready for early adopters.
2. CJM is still a documented practical subset, not production-stable v1.0.
3. The project is actively looking for real C++ model headers, CMake feedback,
   parser failure cases, diagnostics feedback, and generated-code readability
   feedback.

## Positioning

Use this exact positioning:

```text
CJM is a build-time metadata compiler for Modern C++.
JSON is the first official backend.
```

Do not describe CJM as:

- a JSON library;
- a runtime reflection framework;
- a serialization runtime;
- production-stable;
- full C++ grammar compatible;
- a replacement for nlohmann/json.

## Homepage Changes

Add a visible early-adopter section near the top of the homepage, after the
main product positioning and before deeper architecture details.

Suggested heading:

```text
Early Adopters Welcome
```

Suggested copy:

```md
CJM v0.3.0 is ready for early adopters who want to try build-time JSON
integration for ordinary Modern C++ models.

It has been dogfooded in a real downstream CMake project using the public
FetchContent + cjm_generate workflow, covering optionals, vectors, ordered and
unordered string-keyed maps, nested generated structs, enums, ignored fields,
omitempty, fixed-width integers, and generated to_json / from_json round trips.

Try it on a small model. If something fails or feels awkward, share the smallest
header and build command.
```

## Calls To Action

Primary CTA:

```text
Try CJM v0.3.0
```

Target:

```text
https://github.com/cjm-labs/cxx-json-codegen#quickstart
```

Secondary CTA:

```text
Join the feedback thread
```

Target:

```text
https://github.com/cjm-labs/cxx-json-codegen/discussions/129
```

Additional links:

- Report an issue: https://github.com/cjm-labs/cxx-json-codegen/issues/new/choose
- View dogfood report: https://github.com/cjm-labs/cxx-json-codegen/blob/main/docs/dogfood/ull-md-engine-v0.3.0.md
- View release: https://github.com/cjm-labs/cxx-json-codegen/releases/tag/v0.3.0

## Supported Scope Summary

Include a compact supported-scope list:

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

Also keep visible limitations:

- not full C++ grammar support
- no automatic header discovery yet
- no `std::variant` / `std::any` yet
- no pointer or polymorphic serialization
- no enum string mapping policies yet
- packaged installation is still future work

## Quickstart Reminder

Make sure the website clearly shows the current downstream workflow:

```cmake
include(FetchContent)

FetchContent_Declare(
  cxx_json_codegen
  GIT_REPOSITORY https://github.com/cjm-labs/cxx-json-codegen.git
  GIT_TAG v0.3.0
)

FetchContent_MakeAvailable(cxx_json_codegen)

cjm_generate(
  TARGET app
  HEADERS user.hpp
)
```

Also show that users include the model first, then the generated header:

```cpp
#include "user.hpp"
#include "user.cjm.hpp"
```

## Social Proof

Mention dogfooding carefully:

```md
CJM v0.3.0 was validated in
[ull-md-engine](https://github.com/lmingzhi618/ull-md-engine), a real downstream
CMake project, through the public FetchContent + cjm_generate workflow.
```

Do not turn this into a case study or benchmark claim yet.

## Metadata / SEO

Update page title and description if needed:

Title:

```text
CJM — Build-time Metadata Compiler for Modern C++
```

Description:

```text
CJM generates ordinary C++ JSON integration code from source-level metadata at
build time. Early adopters welcome for v0.3.0.
```

Keep canonical URL and existing indexing metadata intact.

## Out of Scope

Do not add:

- broad SEO rewrites;
- blog/case-study pages;
- benchmark claims;
- Discord/newsletter/community operations;
- Tree-sitter implementation details;
- production-stability claims;
- fake testimonials.

## Validation

After editing the website:

- run the site's normal formatter/linter/build if available;
- verify the homepage renders on desktop and mobile;
- verify all GitHub links work;
- verify the CTAs point to Quickstart and Discussion #129;
- keep the tone technical, honest, and early-adopter oriented.
````
