#include "core/ir/model.hpp"

#include <catch2/catch_test_macros.hpp>

using namespace cjm::metadata;

TEST_CASE("field_type.records_array_extent", "[metadata]") {
    // Bild a signed integer element FieldType.
    FieldType int_type{
        FieldTypeKind::SignedInteger,
        "int",
        "int",
    };

    FieldType array_int_type{
        FieldTypeKind::Array, "std::array<int, 4>", "std::array", {int_type}, 4,
    };

    REQUIRE(array_int_type.kind == FieldTypeKind::Array);
    REQUIRE(array_int_type.arguments.size() == 1);
    REQUIRE(array_int_type.arguments[0].kind == FieldTypeKind::SignedInteger);
    REQUIRE(array_int_type.array_extent == 4);
}
