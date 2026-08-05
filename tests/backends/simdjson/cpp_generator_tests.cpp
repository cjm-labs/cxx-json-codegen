#include "backends/simdjson/cpp_generator.hpp"

#include <cassert>
#include <cstdlib>
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

ProjectModel make_bool_project() {
    TypeModel type;
    type.name = "BoolValues";
    type.qualified_name = "BoolValues";
    type.source_location =
        SourceLocation{"tests/fixtures/bool_values.hpp", 1, 1};
    type.fields = {
        make_required_field("enabled", FieldTypeKind::Bool, "bool"),
    };

    ProjectModel project;
    project.types = {type};
    return project;
}

// Build one model containing a scalar kind not implemented yet.
ProjectModel make_signed_integer_project() {
    TypeModel type;
    type.name = "SignedValues";
    type.qualified_name = "SignedValues";
    type.fields = {
        make_required_field("count", FieldTypeKind::SignedInteger,
                            "std::int32_t"),
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
    const auto bool_result =
        cjm::generator::simdjson::generate_header(make_bool_project());

    assert(bool_result.success);
    assert(bool_result.error.empty());
    assert(!bool_result.header.empty());
    assert(bool_result.header.find("#include <simdjson.h>") !=
           std::string::npos);
    assert(bool_result.header.find(
               "std::optional<BoolValues> cjm_decode_simdjson_BoolValues(") !=
           std::string::npos);
    assert(bool_result.header.find("bool has_enabled = false;") !=
           std::string::npos);
    assert(bool_result.header.find("for (auto field : object)") !=
           std::string::npos);
    assert(bool_result.header.find("field.unescaped_key().get(key)") !=
           std::string::npos);
    assert(bool_result.header.find(
               "field.value().get_bool().get(value.enabled)") !=
           std::string::npos);

    const auto signed_result = cjm::generator::simdjson::generate_header(
        make_signed_integer_project());
    assert(!signed_result.success);
    assert(signed_result.header.empty());
    assert(signed_result.error.find("count") != std::string::npos);
    assert(signed_result.error.find("std::int32_t") != std::string::npos);

    const auto vector_result =
        cjm::generator::simdjson::generate_header(make_vector_project());
    assert(!vector_result.success);
    assert(vector_result.header.empty());
    assert(vector_result.error.find("tags") != std::string::npos);
    assert(vector_result.error.find("std::vector<std::string>") !=
           std::string::npos);
    return 0;
}
