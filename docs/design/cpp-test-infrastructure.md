# Named C++ Test Infrastructure

Status: Implemented for the #114 test-infrastructure slice.

This document defines CJM's C++ unit-test framework decision and the boundary
between framework-based tests, CTest, and production builds.

---

# Decision

CJM selects Catch2 v3.15.3 for new C++ unit tests.

The selected upstream release is
[Catch2 v3.15.3](https://github.com/catchorg/Catch2/releases/tag/v3.15.3).
The integration must pin that release and its archive hash. Moving branches or
unversioned latest-release URLs are not acceptable dependency inputs.

CTest remains CJM's repository-wide test runner. Catch2 owns test-case naming,
assertions, filtering, and failure reporting inside new C++ test executables.

---

# Problem

Existing standalone `main()` and `assert` tests are valid integration evidence,
but they make focused development increasingly expensive:

- one assertion abort can hide later cases
- CTest sees one executable instead of its individual cases
- exact function-and-scenario filtering is unavailable
- string equality failures do not identify the useful mismatch location

The goal is not to replace CTest or rewrite working tests. The goal is to give
new tests names, independent execution, and actionable diagnostics.

---

# Evaluation

The decision compared Catch2 v3.15.3 with
[doctest v2.5.3](https://github.com/doctest/doctest/releases/tag/v2.5.3).

Both frameworks satisfy the basic requirements:

- C++17 compatibility
- CMake and FetchContent integration
- direct test-case filtering
- one discovered CTest test per framework test case
- assertion context suitable for a custom golden-difference helper

Catch2 was selected because:

- `catch_discover_tests()` is a mature first-party CTest integration
- discovery supports prefixes, properties, reporters, and post-build or
  pre-test discovery modes
- assertions, matchers, scoped context, and diagnostics leave more room for
  complex parser, semantic, generator, and runtime tests
- Catch2 v3 provides separately compiled implementation targets and focused
  headers instead of requiring one monolithic header in every test source
- its extra compile cost can remain confined to CJM development and CI builds

doctest remains a capable alternative and has a lower compile-time footprint.
That advantage is not decisive for CJM because the framework must never enter a
downstream production build. Adopting both frameworks is not allowed.

Official integration references:

- [Catch2 CMake integration](https://catch2-temp.readthedocs.io/en/latest/cmake-integration.html)
- [doctest build-system integration](https://github.com/doctest/doctest/blob/v2.5.3/doc/markdown/build-systems.md)

---

# CMake Ownership

The test framework belongs entirely inside the `CJM_BUILD_TESTS` boundary.

Required behavior:

```text
CJM is the top-level project
    CJM_BUILD_TESTS defaults to ON

CJM is consumed as a subproject
    CJM_BUILD_TESTS defaults to OFF

CJM_BUILD_TESTS is OFF
    Catch2 is not found, downloaded, configured, built, linked, or exported
```

The top-level/subproject default must remain compatible with CJM's CMake 3.20
minimum. The implementation should use the existing source-directory comparison
pattern rather than requiring a newer CMake-only convenience variable.

Catch2 targets may be linked only by test executables. They must not appear in:

- production target link interfaces
- generated headers
- installed or exported CJM targets
- downstream compile definitions or include paths

The initial framework target should link `Catch2::Catch2WithMain`. New test
sources should include focused Catch2 headers such as
`<catch2/catch_test_macros.hpp>` rather than an all-features header.

CTest registration should use the official `Catch.cmake` module and
`catch_discover_tests()`. Native development builds should begin with the
default post-build discovery mode. A later cross-compilation requirement may
justify pre-test discovery without changing test source contracts.

The repository test declarations live under `tests/CMakeLists.txt`. The root
`CMakeLists.txt` owns only the `CJM_BUILD_TESTS` boundary, Catch2 dependency
setup, and `add_subdirectory(tests)`. This keeps production targets separate
from developer-only test registration.

---

# Naming Contract

Each Catch2 test case uses:

```text
function_name.scenario
```

Example:

```text
generate_header.required_integer
```

The name states the owning function or module-level operation first and the
observable scenario second. It does not encode issue numbers, implementation
incidents, or execution order.

CTest should add the owning target as a discovery prefix so cases from separate
executables cannot collide:

```text
Catch2 case
    generate_header.required_integer

CTest case
    cjm_simdjson_generator.generate_header.required_integer
```

This preserves concise direct filtering while keeping repository-wide CTest
names unique.

Focused CTest selection uses the target-prefixed name:

```sh
ctest --test-dir build/debug \
  -R '^cjm_metadata_named_tests\.field_type\.records_array_extent$' \
  --output-on-failure
```

Direct Catch2 execution uses the shorter case name:

```sh
./build/debug/tests/cjm_metadata_named_tests \
  "field_type.records_array_extent"
```

---

# Migration Policy

After the integration lands:

- new C++ unit tests use Catch2
- existing standalone `main()` and `assert` tests remain valid CTest tests
- legacy tests are not migrated during unrelated feature work
- migration occurs only in a dedicated stabilization or release-closing task
- CTest continues to run legacy and Catch2-based executables together

The first Catch2 target proves the infrastructure with new focused cases. It
must not disguise a legacy-suite migration.

---

# Golden Diagnostics Boundary

Catch2 supplies assertion context and failure reporting. CJM still owns the
semantic shape of golden-output diagnostics.

The reusable helper in `tests/support/golden_diff.hpp` reports a useful
expected/actual mismatch location, including missing or extra trailing content.
It remains under test ownership and is not exposed through production headers
solely to make it testable.

The helper reports:

- first mismatch line and column
- mismatch kind
- expected and actual line text
- missing trailing content
- extra trailing content

---

# Focused Workflow

Configure and build a Debug test tree before running focused cases:

```sh
cmake -S . -B build/debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build/debug
```

Run one exact discovered CTest case:

```sh
ctest --test-dir build/debug \
  -R '^cjm_test_support_named_tests\.golden_diff\.reports_first_changed_character$' \
  --output-on-failure
```

Run all repository tests:

```sh
ctest --test-dir build/debug --output-on-failure
```

Verify that a test-disabled build does not include the Catch2 boundary:

```sh
cmake -S . -B build/no-tests -DCJM_BUILD_TESTS=OFF
cmake --build build/no-tests --target cjm
```

Do not run two configurations that share the same `FETCHCONTENT_BASE_DIR` in
parallel. FetchContent population uses shared dependency directories and should
be configured serially.

---

# Completion Evidence

The #114 implementation was verified on
`feature/114-named-cpp-test-infrastructure` with:

```sh
cmake -S . -B build/debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build/debug
ctest --test-dir build/debug --output-on-failure
```

Result:

```text
100% tests passed, 0 tests failed out of 36
```

The test-disabled boundary was verified with:

```sh
cmake -S . -B build/no-tests -DCJM_BUILD_TESTS=OFF
cmake --build build/no-tests --target cjm
```

The `build/no-tests` tree did not contain Catch2 targets or named test targets.

---

# Non-Goals

This decision does not:

- replace CTest
- migrate existing tests
- change production behavior
- change public CLI or generated-code APIs
- add Catch2 to downstream builds
- select a benchmarking framework
- make test framework types part of CJM public interfaces

---

# Acceptance Criteria

This design decision is complete when:

- Catch2 v3.15.3 is the recorded framework and version
- doctest is recorded as the rejected alternative with its tradeoff
- CTest and Catch2 responsibilities are separate
- test-only dependency isolation is explicit
- direct and CTest naming contracts are explicit
- legacy-test coexistence and migration policy are explicit
- golden diagnostics remain CJM-owned test support
