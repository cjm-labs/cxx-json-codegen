# Early-Adopter Launch Posts

These drafts are for CJM's first early-adopter outreach round.

The goal is to recruit practical feedback, not to run a broad marketing launch.
Keep the tone technical, honest, and specific.

## Positioning

Use this core positioning everywhere:

```text
CJM is a build-time metadata compiler for Modern C++.
JSON is the first official backend.
```

Avoid:

- "production ready";
- "new JSON library";
- "full C++ support";
- "best/fastest";
- benchmark claims;
- broad backend promises.

Ask for:

- practical C++ model headers;
- CMake integration feedback;
- parser failure cases;
- confusing diagnostics;
- generated-code readability feedback.

## Links

- Repo: https://github.com/cjm-labs/cxx-json-codegen
- Release: https://github.com/cjm-labs/cxx-json-codegen/releases/tag/v0.3.0
- Dogfood report: https://github.com/cjm-labs/cxx-json-codegen/blob/main/docs/dogfood/ull-md-engine-v0.3.0.md
- Discussions: https://github.com/cjm-labs/cxx-json-codegen/discussions
- Issues: https://github.com/cjm-labs/cxx-json-codegen/issues
- Website: https://cjm-labs.org

## Reddit r/cpp

Title:

```text
CJM v0.3.0: a compiler-style C++ JSON code generator looking for early adopters
```

Post:

````md
Hi r/cpp,

I have been building CJM, a build-time metadata compiler for Modern C++.

The goal is not to create another JSON library. CJM reads ordinary C++ model
headers with Go-style JSON field metadata, builds a Metadata IR, and generates
ordinary `nlohmann/json` integration code during the build.

Example:

```cpp
struct User {
    std::string name;       // json:"name"
    std::uint64_t id = 0;   // json:"id"
};
```

CJM generates normal C++ `to_json` / `from_json` functions. There are no macros,
compiler plugins, or runtime reflection systems involved in user code.

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

It has also been dogfooded in a real downstream CMake project through the public
`FetchContent + cjm_generate` workflow.

I am looking for early adopters with practical model headers. The most useful
feedback right now would be:

- parser limitations on ordinary headers
- CMake integration friction
- confusing diagnostics
- generated-code readability issues
- type mappings needed before v1.0

Repo:
https://github.com/cjm-labs/cxx-json-codegen

Dogfood report:
https://github.com/cjm-labs/cxx-json-codegen/blob/main/docs/dogfood/ull-md-engine-v0.3.0.md

If you maintain C++ code with model structs and JSON integration, I would love
to hear where this approach works or fails for you.
````

## Reddit r/cpp Shorter Variant

Title:

```text
Looking for feedback on CJM, a build-time C++ JSON code generator
```

Post:

````md
I am looking for early feedback on CJM:

https://github.com/cjm-labs/cxx-json-codegen

CJM is a build-time metadata compiler for Modern C++. It reads ordinary C++
model headers with Go-style JSON field metadata and generates ordinary
`nlohmann/json` `to_json` / `from_json` code.

Example:

```cpp
struct Event {
    std::string name; // json:"name"
    std::optional<std::string> note; // json:"note,omitempty"
    std::map<std::string, std::uint64_t> counters; // json:"counters"
};
```

The current v0.3.0 release supports optionals, vectors, string-keyed maps,
nested generated structs, enums, ignored fields, `omitempty`, and fixed-width
integers.

It has been dogfooded in a real downstream CMake project using
`FetchContent + cjm_generate`.

This is still an early-adopter project, not a production-stability claim. I am
especially interested in practical model headers that break the current parser,
confusing diagnostics, and CMake integration friction.
````

## LinkedIn Post

```md
I have started the early-adopter phase for CJM v0.3.0.

CJM is a build-time metadata compiler for Modern C++. It reads ordinary C++
model declarations with Go-style JSON field metadata, builds a Metadata IR, and
generates ordinary C++ integration code during the build.

The first official backend targets nlohmann/json.

The design goal is simple:

Standard C++ in. Standard C++ out.

No macros in user models.
No runtime reflection.
No schema-first model duplication.

v0.3.0 supports a practical mapping surface:

- scalar fields
- fixed-width integers
- enums
- nested generated structs
- std::vector<T>
- std::optional<T>
- std::map<std::string, T>
- std::unordered_map<std::string, T>
- ignored fields
- omitempty

It has also been dogfooded in a real downstream CMake project using the public
FetchContent + cjm_generate workflow.

I am looking for a small number of Modern C++ developers to try CJM on practical
model headers and tell me what breaks:

- parser limitations
- CMake integration friction
- confusing diagnostics
- generated-code readability
- missing type mappings before v1.0

Repo:
https://github.com/cjm-labs/cxx-json-codegen

Dogfood report:
https://github.com/cjm-labs/cxx-json-codegen/blob/main/docs/dogfood/ull-md-engine-v0.3.0.md

If you work with C++ model structs and JSON integration, I would appreciate your
feedback.
```

## LinkedIn Short Variant

```md
CJM v0.3.0 is entering its early-adopter phase.

CJM is a build-time metadata compiler for Modern C++. It generates ordinary
nlohmann/json integration from ordinary C++ model declarations with Go-style
JSON field metadata.

The goal is not to build another JSON library. The goal is a compiler-style
pipeline:

C++ source -> Metadata IR -> generated C++

v0.3.0 has been dogfooded in a real downstream CMake project and supports
practical type composition such as optionals, vectors, string-keyed maps,
nested generated structs, enums, ignored fields, omitempty, and fixed-width
integers.

I am looking for a few C++ developers to try it on practical model headers and
share feedback before v1.0.

Repo:
https://github.com/cjm-labs/cxx-json-codegen
```

## Chinese Short Post

```md
CJM v0.3.0 现在进入 early-adopter 阶段。

CJM 是一个面向 Modern C++ 的 build-time metadata compiler。它从普通 C++ model
声明中读取 Go-style JSON metadata，构建 Metadata IR，然后在构建期生成普通
C++ 的 nlohmann/json 集成代码。

它不是新的 JSON library，也不是 runtime reflection。

目标是：

Standard C++ in. Standard C++ out.

v0.3.0 已经在真实下游 CMake 项目中通过 FetchContent + cjm_generate 的公开流程
完成 dogfood，覆盖 optional、vector、string-keyed map、nested struct、enum、
ignore、omitempty、fixed-width integer 等 practical mapping surface。

现在想找少量真正写 Modern C++ / CMake 的开发者试用，重点不是 Star，而是真实反馈：

- parser 哪里不够用；
- CMake 接入哪里别扭；
- diagnostics 哪里看不懂；
- generated code 是否可读；
- v1.0 前还缺哪些 practical mapping。

Repo:
https://github.com/cjm-labs/cxx-json-codegen

Dogfood report:
https://github.com/cjm-labs/cxx-json-codegen/blob/main/docs/dogfood/ull-md-engine-v0.3.0.md
```

## GitHub Discussions First Post

Title:

```text
CJM v0.3.0 early-adopter feedback thread
```

Body:

```md
CJM v0.3.0 is ready for early-adopter feedback.

CJM is a build-time metadata compiler for Modern C++. It reads ordinary C++
model declarations with Go-style JSON field metadata and generates ordinary
`nlohmann/json` integration code during the build.

This thread is for open-ended usage and adoption feedback:

- Would CJM fit any model-heavy C++ code you maintain?
- Does the `FetchContent + cjm_generate` workflow feel reasonable?
- What parser limitations would block your real headers?
- Are the current diagnostics understandable?
- What type mappings are missing before v1.0?
- Does the generated code look readable enough to trust?

Useful links:

- Repo: https://github.com/cjm-labs/cxx-json-codegen
- v0.3.0 release: https://github.com/cjm-labs/cxx-json-codegen/releases/tag/v0.3.0
- Dogfood report: https://github.com/cjm-labs/cxx-json-codegen/blob/main/docs/dogfood/ull-md-engine-v0.3.0.md

If you try CJM, please share the smallest practical model that worked or failed.
```

## Short Direct Message

```md
Hi, I am looking for early feedback on CJM, a build-time metadata compiler for
Modern C++ JSON code generation.

It reads ordinary C++ model headers with Go-style JSON metadata and generates
ordinary nlohmann/json integration during the build.

v0.3.0 has been dogfooded in a real downstream CMake project, and I am looking
for a few C++ developers to try it on practical model headers.

Repo: https://github.com/cjm-labs/cxx-json-codegen
Dogfood report: https://github.com/cjm-labs/cxx-json-codegen/blob/main/docs/dogfood/ull-md-engine-v0.3.0.md

If you have a small model header that would be a good test case, feedback would
be very helpful.
```

## Posting Order

Suggested order:

1. Post the GitHub Discussions thread first.
2. Share the LinkedIn post.
3. Post to Reddit only after the repo homepage and Discussions are ready.
4. Do not post to Hacker News yet.

## Follow-Up Rule

When someone responds, optimize for learning:

1. Ask for the smallest model header.
2. Ask how they tried to integrate CJM.
3. Move concrete failures into GitHub Issues.
4. Keep broad design discussion in GitHub Discussions.
5. Do not promise features outside the roadmap.
