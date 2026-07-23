# ull-md-engine Dogfood Report for CJM v0.3.0

This report records CJM v0.3.0 dogfooding in a real downstream CMake project:

- [lmingzhi618/ull-md-engine](https://github.com/lmingzhi618/ull-md-engine)

The goal was to validate CJM through its public workflow, not through private
integration shortcuts.

```text
FetchContent
-> FetchContent_MakeAvailable(cxx_json_codegen)
-> cjm_generate(...)
-> generated *.cjm.hpp
-> normal C++ test target
```

## Pass 1: JSON Logging Smoke Test

Model:

```text
ull::bench::BenchmarkLogEvent
```

Defined in:

```text
include/ull/bench/benchmark_log_event.h
```

Covered field types:

| Field | Type | Result |
| --- | --- | --- |
| `benchmark_name` | `std::string` | passed |
| `phase` | `std::string` | passed |
| `timestamp_ns` | `std::uint64_t` | passed |
| `elapsed_ns` | `std::uint64_t` | passed |
| `success` | `bool` | passed |
| `note` | `std::optional<std::string>` | passed |
| `tags` | `std::vector<std::string>` | passed |
| `counters` | `std::map<std::string, std::uint64_t>` | passed |

Validation:

```bash
cmake -S . -B build/dev -DCMAKE_BUILD_TYPE=Debug
cmake --build build/dev --target test_benchmark_log_event_json
./build/dev/test_benchmark_log_event_json
```

Result:

```text
test_benchmark_log_event_json PASS
```

## Pass 2: Practical Mapping Surface Test

Model:

```text
ull::bench::CjmMappingSurfaceEvent
```

Defined in:

```text
include/ull/bench/cjm_mapping_surface_event.h
```

Covered mapping features:

| Feature | Field / Type | Result |
| --- | --- | --- |
| enum class | `CjmEventLevel` | passed |
| nested generated struct | `CjmMappingDetail` | passed |
| signed fixed-width integer | `std::int64_t` | passed |
| floating point | `double` | passed |
| optional + omitempty | `std::optional<std::string>` | passed |
| vector | `std::vector<std::int64_t>` | passed |
| unordered string map | `std::unordered_map<std::string, std::uint64_t>` | passed |
| ignored field | `json:"-"` | passed |
| from_json round-trip | full model | passed |

Validation:

```bash
cmake -S . -B build/dev -DCMAKE_BUILD_TYPE=Debug
cmake --build build/dev --target test_cjm_mapping_surface_json
ctest --test-dir build/dev -R "test_benchmark_log_event_json|test_cjm_mapping_surface_json" --output-on-failure
```

Result:

```text
100% tests passed, 0 tests failed out of 2
```

## Observations

CJM generated downstream headers such as:

```text
build/dev/generated/cjm/benchmark_log_event.cjm.hpp
build/dev/generated/cjm/cjm_mapping_surface_event.cjm.hpp
```

The generated integration supported the normal `nlohmann/json` workflow:

```cpp
nlohmann::json j = event;
auto decoded = j.get<ull::bench::CjmMappingSurfaceEvent>();
```

The tests verified:

- optional fields with `omitempty`
- ignored fields with `json:"-"`
- nested generated structs
- ordered and unordered string-keyed maps
- enum round-trip behavior
- generated `to_json` / `from_json` round trips

## Friction

The dogfood run exposed one CJM warning when consumed as a downstream
dependency:

```text
warning: comparison of integers of different signs: 'int' and 'std::size_t'
_deps/cxx_json_codegen-src/src/frontends/cxx/semantic/analysis.cpp:82
```

This did not block the downstream build.

One usability note: CJM generates header-only `*.cjm.hpp` files. It is easy to
accidentally include `*.cjm.cpp` on the first attempt.

## Conclusion

CJM v0.3.0 works as a downstream CMake dependency in `ull-md-engine` for a
small JSON logging model and for a broader practical mapping-surface model.

This validates the public v0.3.0 workflow without modifying CJM internals or
introducing a broad logging framework in the downstream project.

Future dogfooding should be driven by real benchmark-result or observability
needs, not by adding mapping tests indefinitely.
