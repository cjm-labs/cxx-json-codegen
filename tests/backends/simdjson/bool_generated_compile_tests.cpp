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

    return 0;
}
