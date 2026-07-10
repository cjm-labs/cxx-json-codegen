#include "metadata/model.hpp"

#include <cassert>

using namespace cjm::metadata;

int main() {
    SourceLocation field_location{"include/user.hpp", 2, 17};

    JsonFieldMetadata json_name{
        "name",
        false,
    };

    FieldType string_type{
        FieldTypeKind::String,
        "std::string",
        "std::string",
    };

    FieldModel name_field{
        "name",
        string_type,
        json_name,
        field_location,
    };

    FieldModel age_field{
        "age",
        FieldType{
            FieldTypeKind::SignedInteger,
            "int",
            "int",
        },
        JsonFieldMetadata{"age", false},
        SourceLocation{"include/user.hpp", 3, 9},
    };

    TypeModel user_type;
    user_type.name = "User";
    user_type.namespace_path = {};
    user_type.qualified_name = "User";
    user_type.fields = {name_field, age_field};
    user_type.source_location = SourceLocation{"include/user.hpp", 1, 8};

    ProjectModel project;
    project.types = {user_type};

    assert(project.types.size() == 1);
    assert(project.types[0].qualified_name == "User");

    assert(project.types[0].fields.size() == 2);
    assert(project.types[0].fields[0].name == "name");
    assert(project.types[0].fields[0].json.name == "name");
    assert(project.types[0].fields[0].type.kind == FieldTypeKind::String);

    assert(project.types[0].fields[1].name == "age");
    assert(project.types[0].fields[1].json.name == "age");
    assert(project.types[0].fields[1].type.kind ==
           FieldTypeKind::SignedInteger);

    assert(project.types[0].fields[0].source_location.file ==
           "include/user.hpp");
    assert(project.types[0].fields[0].source_location.line == 2);
    assert(project.types[0].fields[0].source_location.column == 17);

    return 0;
}
