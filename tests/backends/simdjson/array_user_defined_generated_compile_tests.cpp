#include <array>
#include <cassert>
#include <cstdint>

struct Item {
    std::int64_t id = 0;
};

struct FixedOrder {
    std::array<Item, 2> items;
};

#include "tests/golden/simdjson_array_user_defined.expected.cjm.hpp"

int main() {
    {
        cjm::simdjson::DecodeError error;
        const auto result = cjm::simdjson::from_json<FixedOrder>(
            R"({"items":[{"id":7},{"id":9}]})", error);

        assert(result.has_value());
        assert(result->items[0].id == 7);
        assert(result->items[1].id == 9);
        assert(error.code == cjm::simdjson::DecodeErrorCode::none);
        assert(error.path.empty());
    }
    {
        cjm::simdjson::DecodeError error;
        const auto result = cjm::simdjson::from_json<FixedOrder>(
            R"({"items":[{"id":7}]})", error);

        assert(!result.has_value());
        assert(error.code ==
               cjm::simdjson::DecodeErrorCode::fixed_array_extent_mismatch);
        assert(error.path.size() == 1);
        assert(error.path[0].field_name == "items");
    }
    {
        cjm::simdjson::DecodeError error;
        const auto result = cjm::simdjson::from_json<FixedOrder>(
            R"({"items":[{"id":"bad"},{"id":9}]})", error);

        assert(!result.has_value());
        assert(error.code == cjm::simdjson::DecodeErrorCode::expected_integer);
        assert(error.path.size() == 3);
        assert(error.path[0].field_name == "items");
        assert(error.path[1].kind ==
               cjm::simdjson::DecodePathSegmentKind::index);
        assert(error.path[1].index == 0);
        assert(error.path[2].field_name == "id");
    }
    return 0;
}
