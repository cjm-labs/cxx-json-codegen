#include <catch2/catch_test_macros.hpp>
#include <simdjson.h>

#include <string_view>

TEST_CASE("string_builder.writes_object_in_cpp17", "[simdjson][builder]") {
    ::simdjson::builder::string_builder builder;

    builder.start_object();
    builder.escape_and_append_with_quotes("name");
    builder.append_colon();
    builder.escape_and_append_with_quotes("A\nB");
    builder.end_object();

    std::string_view output;
    REQUIRE(builder.view().get(output) == ::simdjson::SUCCESS);
    REQUIRE(output == R"({"name":"A\nB"})");
}
