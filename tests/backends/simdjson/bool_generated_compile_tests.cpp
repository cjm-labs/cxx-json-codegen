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

    return 0;
}
