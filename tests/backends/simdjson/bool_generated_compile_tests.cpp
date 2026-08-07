#include <cassert>

struct BoolValues {
    bool enabled = false;
};

#include "tests/golden/simdjson_bool.expected.cjm.hpp"

int main() {
    cjm::simdjson::DecodeError error;

    const auto result =
        cjm::simdjson::from_json<BoolValues>(R"({"enabled":true})", error);

    assert(result.has_value());
    assert(result->enabled);
    assert(error.code == cjm::simdjson::DecodeErrorCode::none);
    assert(error.path.empty());
    assert(error.runtime_error == simdjson::SUCCESS);

    cjm::simdjson::DecodeError missing_error;

    const auto missing_result =
        cjm::simdjson::from_json<BoolValues>("{}", missing_error);

    assert(!missing_result.has_value());
    assert(missing_error.code ==
           cjm::simdjson::DecodeErrorCode::missing_required_field);
    assert(missing_error.path.size() == 1);
    assert(missing_error.path[0].kind ==
           cjm::simdjson::DecodePathSegmentKind::field);
    assert(missing_error.path[0].field_name == "enabled");
    assert(missing_error.runtime_error == simdjson::SUCCESS);

    cjm::simdjson::DecodeError type_mismatch_error;

    const auto type_mismatch_result = cjm::simdjson::from_json<BoolValues>(
        R"({"enabled":123})", type_mismatch_error);

    assert(!type_mismatch_result.has_value());
    assert(type_mismatch_error.code ==
           cjm::simdjson::DecodeErrorCode::expected_bool);
    assert(type_mismatch_error.path.size() == 1);
    assert(type_mismatch_error.path[0].kind ==
           cjm::simdjson::DecodePathSegmentKind::field);
    assert(type_mismatch_error.path[0].field_name == "enabled");
    assert(type_mismatch_error.runtime_error == simdjson::INCORRECT_TYPE);

    cjm::simdjson::DecodeError trailing_error;

    const auto trailing_result = cjm::simdjson::from_json<BoolValues>(
        R"({"enabled":true}
{"enabled":false})",
        trailing_error);

    assert(!trailing_result.has_value());
    assert(trailing_error.code ==
           cjm::simdjson::DecodeErrorCode::trailing_content);
    assert(trailing_error.path.empty());
    assert(trailing_error.runtime_error == simdjson::TRAILING_CONTENT);
    return 0;
}
