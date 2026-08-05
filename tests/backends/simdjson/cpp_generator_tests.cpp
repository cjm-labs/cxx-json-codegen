#include "backends/simdjson/cpp_generator.hpp"

#include <cassert>
#include <string>

namespace {

using cjm::metadata::FieldModel;
using cjm::metadata::FieldType;
using cjm::metadata::FieldTypeKind;
using cjm::metadata::JsonFieldMetadata;
using cjm::metadata::ProjectModel;
using cjm::metadata::SourceLocation;
using cjm::metadata::TypeModel;

// Build one required field whols JSON name matches its C++ member name.
FieldModel make_required_field(const std::string& name, FieldTypeKind kind,
                               const std::string& spelling) {
    return FieldModel{
        name,
        FieldType{kind, spelling, spelling},
        JsonFieldMetadata{name, false, false},
        SourceLocation{"tests/fixtures/scalar_values.hpp", 1, 1},
    };
}

// Build the required scalar model accepted by the initial simdjson spike.
ProjectModel make_scalar_project() {
    TypeModel type;
    type.name = "ScalarValues";
    type.qualified_name = "ScalarValues";
    type.source_location =
        SourceLocation{"tests/fixtures/scalar_values.hpp", 1, 1};
    type.fields = {
        make_required_field("enabled", FieldTypeKind::Bool, "bool"),
        make_required_field("count", FieldTypeKind::SignedInteger,
                            "std::int32_t"),
        make_required_field("limit", FieldTypeKind::UnsignedInteger,
                            "std::uint32_t"),
        make_required_field("ratio", FieldTypeKind::UnsignedInteger, "double"),
        make_required_field("name", FieldTypeKind::UnsignedInteger,
                            "std::string"),
    };

    ProjectModel project;
    project.types = {type};
    return project;
}

// Build a model containing one type unsupported by the initial spike.
ProjectModel make_vector_project() {
    auto vector_field = make_required_field("tags", FieldTypeKind::Vector,
                                            "std::vector<std::string>");
    vector_field.type.qualified_name = "std::vector";
    vector_field.type.arguments = {
        FieldType{
            FieldTypeKind::String,
            "std::string",
            "std::string",
        },
    };

    TypeModel type;
    type.name = "VectorValues";
    type.qualified_name = "VectorValues";
    type.fields = {vector_field};

    ProjectModel project;
    project.types = {type};
    return project;
}

} // namespace

int main() {
    const auto scalar_result =
        cjm::generator::simdjson::generate_header(make_scalar_project());

    assert(scalar_result.success);
    assert(scalar_result.error.empty());
    assert(!scalar_result.header.empty());
    assert(scalar_result.header.find("#include <simdjson.h>") !=
           std::string::npos);
    const auto vector_result =
        cjm::generator::simdjson::generate_header(make_vector_project());
    assert(!vector_result.success);
    assert(vector_result.header.empty());
    assert(vector_result.error.find("std::vector<std::string>") !=
           std::string::npos);
    return 0;
}
