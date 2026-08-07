#include <cassert>
#include <cstdint>

struct IntegerValues {
    std::int32_t count = 0;
    std::uint32_t limit = 0;
    std::int8_t narrow = 0;
};

#include "tests/golden/simdjson_integer.expected.cjm.hpp"

int main() {
    cjm::simdjson::DecodeError error;

    const auto result = cjm::simdjson::from_json<IntegerValues>(
        R"({"limit": 199, "narrow": -8, "count": -15})", error);

    assert(result.has_value());
    assert(result->count == -15);
    assert(result->limit == 199);
    assert(result->narrow == -8);
    assert(error.code == cjm::simdjson::DecodeErrorCode::none);
    assert(error.path.empty());
    assert(error.runtime_error == simdjson::SUCCESS);

    cjm::simdjson::DecodeError overflow_error;
    const auto overflow_result = cjm::simdjson::from_json<IntegerValues>(
        R"({"count": 0, "limit": 0, "narrow": 128})", overflow_error);

    assert(!overflow_result.has_value());
    assert(overflow_error.code ==
           cjm::simdjson::DecodeErrorCode::integer_overflow);
    assert(overflow_error.path.size() == 1);
    assert(overflow_error.path[0].kind ==
           cjm::simdjson::DecodePathSegmentKind::field);
    assert(overflow_error.path[0].field_name == "narrow");
    assert(overflow_error.runtime_error == simdjson::SUCCESS);

    return 0;
}
