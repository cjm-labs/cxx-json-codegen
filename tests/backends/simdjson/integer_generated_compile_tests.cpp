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

    return 0;
}
