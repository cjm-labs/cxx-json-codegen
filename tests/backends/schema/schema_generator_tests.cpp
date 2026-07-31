#include "backends/schema/schema_generator.hpp"

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace cjm::metadata;

namespace {

std::string read_file(const std::string& path) {
    std::fstream file(path);
    assert(file.is_open());
    std::ostringstream contents;
    contents << file.rdbuf();
    return contents.str();
}

ProjectModel make_simple_schema_project() {
    TypeModel user;
    user.name = "User";
    user.qualified_name = "company::model::User";

    user.fields = {
        FieldModel{
            "name",
            FieldType{FieldTypeKind::String, "std::string", "std::string"},
            JsonFieldMetadata{"name", false},
            SourceLocation{"include/user.hpp", 2, 17}},
        FieldModel{"age", FieldType{FieldTypeKind::SignedInteger, "int", "int"},
                   JsonFieldMetadata{"age", false},
                   SourceLocation{"include/user.hpp", 3, 9}},
        FieldModel{
            "internal_id",
            FieldType{FieldTypeKind::UnsignedInteger, "unsigned", "unsigned"},
            JsonFieldMetadata{"", false},
            SourceLocation{"include/user.hpp", 4, 14}},
    };

    ProjectModel project;
    project.types = {user};
    return project;
}

} // namespace

int main() {
    const std::string generated =
        cjm::generator::schema::generate_schema(make_simple_schema_project());
    const std::string expected =
        read_file("tests/golden/basic_user.expected.schema.json");

    if (generated != expected) {
        std::cerr << generated;
    }

    assert(generated == expected);
    return 0;
}
