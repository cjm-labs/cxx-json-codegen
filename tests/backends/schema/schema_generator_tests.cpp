#include "backends/schema/schema_generator.hpp"

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace cjm::metadata;

namespace {
const cjm::metadata::FieldType string_type = {
    cjm::metadata::FieldTypeKind::String,
    "std::string",
    "std::string",
};
const cjm::metadata::FieldType int_type = {
    cjm::metadata::FieldTypeKind::SignedInteger,
    "int",
    "int",
};
const cjm::metadata::FieldType bool_type = {
    cjm::metadata::FieldTypeKind::Bool,
    "bool",
    "bool",
};

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
            JsonFieldMetadata{"", false, true},
            SourceLocation{"include/user.hpp", 4, 14}},
    };

    ProjectModel project;
    project.types = {user};
    return project;
}

ProjectModel make_array_schema_project() {
    TypeModel array_values;
    array_values.name = "ArrayValues";
    array_values.qualified_name = "company::model::ArrayValues";

    cjm::metadata::FieldType int_type = {
        cjm::metadata::FieldTypeKind::SignedInteger,
        "int",
        "int",
    };
    cjm::metadata::FieldType string_type = {
        cjm::metadata::FieldTypeKind::String,
        "std::string",
        "std::string",
    };
    array_values.fields = {
        FieldModel{
            "tags",
            FieldType{FieldTypeKind::Vector,
                      "std::vector",
                      "std::vector",
                      {string_type}},
            JsonFieldMetadata{"tags", false},
            SourceLocation{"include/array_values.hpp", 2, 17},
        },
        FieldModel{
            "scores",
            FieldType{
                FieldTypeKind::Vector,
                "std::vector<int>",
                "std::vector",
                {int_type},
            },
            JsonFieldMetadata{"scores", false},
            SourceLocation{"include/array_values.hpp", 3, 18},
        },
        FieldModel{
            "samples",
            FieldType{FieldTypeKind::Array,
                      "std::array<int, 4>",
                      "std::array",
                      {int_type},
                      4},
            JsonFieldMetadata{"samples", false},
            SourceLocation{"include/array_values.hpp", 4, 20},
        },
    };
    ProjectModel project;
    project.types = {array_values};
    return project;
}

// Build a small project that exercises optional simple schema mappings.
ProjectModel make_optional_schema_project() {
    TypeModel optional_values;
    optional_values.name = "OptionalValues";
    optional_values.qualified_name = "company::model::OptionalValues";
    optional_values.fields = {
        FieldModel{
            "nickname",
            FieldType{FieldTypeKind::Optional,
                      "std::optional<std::string>",
                      "std::optional",
                      {string_type}},
            JsonFieldMetadata{"nickname", true},
        },
        FieldModel{
            "score",
            FieldType{FieldTypeKind::Optional,
                      "std::optional<int>",
                      "std::optional",
                      {int_type}},
            JsonFieldMetadata{"score", false},
        },
        FieldModel{"enabled",
                   FieldType{FieldTypeKind::Bool, "bool", "bool", {bool_type}},
                   JsonFieldMetadata{"enabled", false}},
    };

    ProjectModel project;
    project.types = {optional_values};
    return project;
}

// Build a small project that exercises string-keyed map schema mappings.
ProjectModel make_map_schema_project() {
    TypeModel map_values;
    map_values.name = "MapValues";
    map_values.fields = {
        FieldModel{
            "labels",
            FieldType{FieldTypeKind::Map,
                      "std::map<std::string, int>",
                      "std::map",
                      {string_type, int_type}},
            JsonFieldMetadata{"labels", false},
        },
        FieldModel{
            "aliases",
            FieldType{FieldTypeKind::Map,
                      "std::unordered_map<std::string, std::string>",
                      "std::unordered_map",
                      {string_type, string_type}},
            JsonFieldMetadata{"aliases", false},
        },
    };

    ProjectModel project;
    project.types = {map_values};
    return project;
}

// Build a small project that exercises enum string schema mappings.
ProjectModel make_enum_schema_project() {
    TypeModel enum_values;
    enum_values.name = "EnumValues";
    enum_values.fields = {
        FieldModel{
            "status",
            FieldType{
                FieldTypeKind::Enum,
                "Status",
                "company::model::Status",
            },
            JsonFieldMetadata{"status", false},
        },

    };

    EnumModel enum_status;
    enum_status.name = "Status";
    enum_status.qualified_name = "company::model::Status";
    enum_status.enumerators = {"Active", "Disabled"};
    ProjectModel project;
    project.types = {enum_values};
    project.enums = {enum_status};
    return project;
}

// Build a small project that exercises nested object schema mappings.
ProjectModel make_nested_object_schema_project() {
    TypeModel address;
    address.name = "Address";
    address.qualified_name = "company::model::Address";
    address.fields = {FieldModel{
        "city",
        string_type,
        JsonFieldMetadata{"city", false},
    }};

    TypeModel user;
    user.name = "User";
    user.fields = {
        FieldModel{
            "name",
            string_type,
            JsonFieldMetadata{"name", false},
        },
        FieldModel{
            "address",
            FieldType{
                FieldTypeKind::UserDefined,
                "Address",
                "company::model::Address",
            },
            JsonFieldMetadata{"address", false},
        },
    };
    ProjectModel project;
    project.types = {user, address};
    return project;
}

// Build a practical project that combines supported schema mappings.
ProjectModel make_practical_schema_project() {
    TypeModel address;
    address.name = "Address";
    address.qualified_name = "company::model::Address";
    address.fields = {FieldModel{
        "city",
        string_type,
        JsonFieldMetadata{"city", false},
    }};

    TypeModel user;
    user.name = "User";
    user.fields = {
        FieldModel{
            "name",
            string_type,
            JsonFieldMetadata{"name", false},
        },
        FieldModel{
            "tags",
            FieldType{FieldTypeKind::Vector,
                      "std::vector<std::string>",
                      "std::vector",
                      {string_type}},
            JsonFieldMetadata{"tags", false},
        },
        FieldModel{
            "samples",
            FieldType{FieldTypeKind::Array,
                      "std::array<int, 4>",
                      "std::array",
                      {int_type},
                      4},
            JsonFieldMetadata{"samples", false},
        },
        FieldModel{
            "nickname",
            FieldType{FieldTypeKind::Optional,
                      "std::optional<std::string>",
                      "std::optional",
                      {string_type}},
            JsonFieldMetadata{"nickname", true},
        },
        FieldModel{"labels",
                   FieldType{FieldTypeKind::Map,
                             "std::map<std::string, int>",
                             "std::map",
                             {string_type, int_type}},
                   JsonFieldMetadata{"labels", false}},
        FieldModel{
            "status",
            FieldType{
                FieldTypeKind::Enum,
                "Status",
                "company::model::Status",
            },
            JsonFieldMetadata{"status", false},
        },
        FieldModel{
            "address",
            FieldType{
                FieldTypeKind::UserDefined,
                "Address",
                "company::model::Address",
            },
            JsonFieldMetadata{"address", false},
        },
        FieldModel{"internal_id", int_type, JsonFieldMetadata{"", false, true}},
    };

    EnumModel enum_status;
    enum_status.name = "Status";
    enum_status.qualified_name = "company::model::Status";
    enum_status.enumerators = {"Active", "Disabled"};

    ProjectModel project;
    project.enums = {enum_status};
    project.types = {user, address};
    return project;
}

/**
 * Compare schema backend output with one golden schema file.
 *
 * The test prints generated schema text on mismatch so the failure is easy
 * to inspect without debugging the generator.
 */
void assert_schema_matches(const ProjectModel& project,
                           const std::string& expected_path) {
    const std::string generated =
        cjm::generator::schema::generate_schema(project);
    const std::string expected = read_file(expected_path);

    if (generated != expected) {
        std::cerr << generated;
    }

    assert(generated == expected);
}

} // namespace

int main() {

    assert_schema_matches(make_simple_schema_project(),
                          "tests/golden/basic_user.expected.schema.json");
    assert_schema_matches(make_array_schema_project(),
                          "tests/golden/array_values.expected.schema.json");
    assert_schema_matches(make_optional_schema_project(),
                          "tests/golden/optional_values.expected.schema.json");
    assert_schema_matches(make_map_schema_project(),
                          "tests/golden/map_values.expected.schema.json");
    assert_schema_matches(make_enum_schema_project(),
                          "tests/golden/enum_values.expected.schema.json");
    assert_schema_matches(make_nested_object_schema_project(),
                          "tests/golden/nested_object.expected.schema.json");
    assert_schema_matches(make_practical_schema_project(),
                          "tests/golden/practical_schema.expected.schema.json");

    return 0;
}
