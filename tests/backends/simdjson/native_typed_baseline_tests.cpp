#include <catch2/catch_test_macros.hpp>
#include <simdjson.h>

#include <cstdint>
#include <string_view>

namespace {

struct NativeScalarValues {
    bool enabled = false;
    std::int64_t count = 0;
    std::uint64_t limit = 0;
};

} // namespace

// Decode one native scalar model from the current On-Demand document.
template <>
simdjson_inline simdjson::simdjson_result<NativeScalarValues>
simdjson::ondemand::document::get<NativeScalarValues>() & noexcept {
    simdjson::ondemand::object object;
    auto error = get_object().get(object);
    if (error) {
        return error;
    }

    NativeScalarValues values;
    if ((error = object["enabled"].get_bool().get(values.enabled))) {
        return error;
    }
    if ((error = object["count"].get_int64().get(values.count))) {
        return error;
    }
    if ((error = object["limit"].get_uint64().get(values.limit))) {
        return error;
    }
    return values;
}

TEST_CASE("document_get.required_scalars", "[simdjson][baseline]") {
    const simdjson::padded_string input(
        std::string_view{R"({"limit":199,"enabled":true,"count":-15})"});
    simdjson::ondemand::parser parser;
    simdjson::ondemand::document document;

    REQUIRE(parser.iterate(input).get(document) == simdjson::SUCCESS);

    NativeScalarValues values;
    REQUIRE(document.get<NativeScalarValues>().get(values) ==
            simdjson::SUCCESS);
    REQUIRE(values.enabled);
    REQUIRE(values.count == -15);
    REQUIRE(values.limit == 199);
}
