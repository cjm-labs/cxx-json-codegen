#include <catch2/catch_test_macros.hpp>

#include <cstddef>
#include <new>

namespace {
thread_local bool fail_builder_allocation = false;
thread_local std::size_t injected_failures = 0;

// Fail only nothrow array allocations during the generated encoder call.
struct FailBuilderAllocation {
    FailBuilderAllocation() { fail_builder_allocation = true; }
    ~FailBuilderAllocation() { fail_builder_allocation = false; }
};
} // namespace

// The pinned builder allocates its byte buffer with nothrow new[].
// Keep this replacement local to this dedicated test executable; delegate
// normal allocations to new[] so the normal delete[] remains compatible.
void* operator new[](std::size_t size, const std::nothrow_t&) noexcept {
    if (fail_builder_allocation) {
        ++injected_failures;
        return nullptr;
    }
    try {
        return ::operator new[](size);
    } catch (...) {
        return nullptr;
    }
}

struct BoolValues {
    bool enabled = false;
};

#include "tests/golden/simdjson_bool.expected.cjm.hpp"

TEST_CASE("to_json.bool_values", "[simdjson][encoder]") {
    for (const bool enabled : {false, true}) {
        DYNAMIC_SECTION("enabled = " << enabled) {
            const BoolValues value{enabled};
            cjm::simdjson::EncodeError error;

            const auto output = cjm::simdjson::to_json(value, error);

            REQUIRE(output.has_value());
            REQUIRE(*output ==
                    (enabled ? R"({"enabled":true})" : R"({"enabled":false})"));
            REQUIRE(error.code == cjm::simdjson::EncodeErrorCode::none);
            REQUIRE(error.path.empty());
            REQUIRE(error.runtime_error == ::simdjson::SUCCESS);
            REQUIRE(value.enabled == enabled);
        }
    }
}

TEST_CASE("to_json.bool_resets_previous_error", "[simdjson][encoder]") {
    cjm::simdjson::EncodeError error;
    error.code = cjm::simdjson::EncodeErrorCode::invalid_utf8_string;
    error.path.push_back(
        {cjm::simdjson::EncodePathSegmentKind::field, "old_field", 0});
    error.runtime_error = ::simdjson::UTF8_ERROR;

    const auto output = cjm::simdjson::to_json(BoolValues{true}, error);

    REQUIRE(output.has_value());
    REQUIRE(*output == R"({"enabled":true})");
    REQUIRE(error.code == cjm::simdjson::EncodeErrorCode::none);
    REQUIRE(error.path.empty());
    REQUIRE(error.runtime_error == ::simdjson::SUCCESS);
}

TEST_CASE("to_json.bool_reports_builder_failure_and_recovers",
          "[simdjson][encoder]") {
    const BoolValues value{true};
    cjm::simdjson::EncodeError error;
    error.code = cjm::simdjson::EncodeErrorCode::invalid_utf8_string;
    error.path.push_back(
        {cjm::simdjson::EncodePathSegmentKind::field, "old_field", 0});
    error.runtime_error = ::simdjson::UTF8_ERROR;

    std::optional<std::string> output;
    injected_failures = 0;
    {
        FailBuilderAllocation failure;
        output = cjm::simdjson::to_json(value, error);
    }

    REQUIRE(injected_failures > 0);
    REQUIRE_FALSE(output.has_value());
    REQUIRE(error.code == cjm::simdjson::EncodeErrorCode::output_failure);
    REQUIRE(error.path.empty());
    REQUIRE(error.runtime_error == ::simdjson::OUT_OF_CAPACITY);
    REQUIRE(value.enabled);

    output = cjm::simdjson::to_json(value, error);
    REQUIRE(output.has_value());
    REQUIRE(*output == R"({"enabled":true})");
    REQUIRE(error.code == cjm::simdjson::EncodeErrorCode::none);
    REQUIRE(error.path.empty());
    REQUIRE(error.runtime_error == ::simdjson::SUCCESS);
}
