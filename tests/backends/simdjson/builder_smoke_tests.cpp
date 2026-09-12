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

TEST_CASE("string_builder.accepts_invalid_utf8_until_validation",
          "[simdjson][builder]") {
    ::simdjson::builder::string_builder builder;
    const std::string_view invalid_utf8{"\xC3\x28", 2};

    builder.escape_and_append_with_quotes(invalid_utf8);

    std::string_view output;
    REQUIRE(builder.view().get(output) == ::simdjson::SUCCESS);
    REQUIRE_FALSE(builder.validate_unicode());
}

TEST_CASE("string_builder.appends_bool_value", "[simdjson][builder]") {
    ::simdjson::builder::string_builder builder;

    builder.start_object();
    builder.escape_and_append_with_quotes("enabled");
    builder.append_colon();
    builder.append(true);
    builder.end_object();

    std::string_view output;
    REQUIRE(builder.view().get(output) == ::simdjson::SUCCESS);
    REQUIRE(output == R"({"enabled":true})");
}
