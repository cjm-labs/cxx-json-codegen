#include "backends/nlohmann/cpp_generator.hpp"

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace cjm::metadata;
using namespace cjm::generator;

namespace {

std::string read_file(const std::string& path) {
    std::fstream file(path);
    assert(file.is_open());

    std::ostringstream contents;
    contents << file.rdbuf();
    return contents.str();
}

void write_file(const std::string& path, const std::string& contents) {
    std::ofstream file(path);
    assert(file.is_open());
    file << contents;
}

ProjectModel make_basic_user_project() {
    TypeModel user;
    user.name = "User";
    user.namespace_path = {"company", "model"};
    user.qualified_name = "company::model::User";
    user.source_location = SourceLocation{"include/user.hpp", 1, 8};

    FieldType string_type{
        FieldTypeKind::String,
        "std::string",
        "std::string",
    };
    FieldType optional_string_type{
        FieldTypeKind::Optional,
        "std::optional<std::string>",
        "std::optional",
    };
    optional_string_type.arguments = {string_type};

    FieldType int_type{
        FieldTypeKind::SignedInteger,
        "int",
        "int",
    };
    FieldType optional_int_type{
        FieldTypeKind::Optional,
        "std::optional<int>",
        "std::optional",
    };
    optional_int_type.arguments = {int_type};

    FieldType vector_string_type{
        FieldTypeKind::Vector,
        "std::vector<std::string>",
        "std::vector",
    };
    vector_string_type.arguments = {string_type};

    FieldType address_type{
        FieldTypeKind::UserDefined,
        "Address",
        "company::model::Address",
    };

    FieldType status_type{
        FieldTypeKind::Enum,
        "Status",
        "company::model::Status",
    };

    user.fields = {
        FieldModel{
            "name",
            FieldType{
                FieldTypeKind::String,
                "std::string",
                "std::string",
            },
            JsonFieldMetadata{"name", false},
            SourceLocation{"include/user.hpp", 2, 27},
        },
        FieldModel{
            "age",
            FieldType{
                FieldTypeKind::SignedInteger,
                "int",
                "int",
            },
            JsonFieldMetadata{"age", false},
            SourceLocation{"include/user.hpp", 3, 9},
        },
        FieldModel{
            "tags",
            vector_string_type,
            JsonFieldMetadata{"tags", false},
            SourceLocation{"include/user.hpp", 6, 37},
        },
        FieldModel{
            "address",
            address_type,
            JsonFieldMetadata{"address", false},
            SourceLocation{"include/user.hpp", 7, 13},
        },
        FieldModel{
            "status",
            status_type,
            JsonFieldMetadata{"status", false},
            SourceLocation{"include/user.hpp", 8, 12},
        },
        FieldModel{
            "nickname",
            optional_string_type,
            JsonFieldMetadata{"nickname", true},
            SourceLocation{"include/user.hpp", 4, 39},
        },
        FieldModel{
            "score",
            optional_int_type,
            JsonFieldMetadata{"score", true},
            SourceLocation{"include/user.hpp", 5, 28},
        },
        FieldModel{
            "internal_id",
            FieldType{
                FieldTypeKind::SignedInteger,
                "int",
                "int",
            },
            JsonFieldMetadata{"", false},
            SourceLocation{"include/user.hpp", 9, 9},
        },
    };

    TypeModel address;
    address.name = "Address";
    address.namespace_path = {"company", "model"};
    address.qualified_name = "company::model::Address";
    address.source_location = SourceLocation{"include/address.hpp", 1, 8};
    address.fields = {
        FieldModel{
            "city",
            string_type,
            JsonFieldMetadata{"city", false},
            SourceLocation{"include/address.hpp", 2, 17},
        },
    };

    ProjectModel project;
    project.types = {address, user};
    return project;
}

// Compare generated output with a golden file.
void assert_generated_matches(const ProjectModel& project,
                              const std::string& expected_path,
                              const std::string& actual_path) {
    const std::string generated = generate_header(project);
    const std::string expected = read_file(expected_path);

    if (generated != expected) {
        write_file(actual_path, generated);
        std::cerr << "golden mismatch\n"
                  << "expected: " << expected << "\n"
                  << "actual: " << generated << "\n";
    }
    assert(generated == expected);
}

// Build a minimal project that exercises map backend generation.
ProjectModel make_map_user_project() {
    TypeModel user;
    user.name = "User";
    user.namespace_path = {"company", "model"};
    user.qualified_name = "company::model::User";
    user.source_location = SourceLocation{"include/map_user.hpp", 1, 8};

    // 1. Define scalar and map field types.
    FieldType string_type{FieldTypeKind::String, "std::string", "std::string"};
    FieldType int_type{FieldTypeKind::SignedInteger, "int", "int"};

    FieldType map_string_int_type{
        FieldTypeKind::Map,
        "std::map<std::string, int>",
        "std::map",
    };
    map_string_int_type.arguments = {string_type, int_type};

    FieldType unordered_map_string_string_type{
        FieldTypeKind::Map,
        "std::unordered_map<std::string, std::string>",
        "std::unordered_map",
    };
    unordered_map_string_string_type.arguments = {string_type, string_type};

    FieldType optional_map_string_int_type{
        FieldTypeKind::Optional,
        "std::optional<std::map<std::string, int>>",
        "std::optional",
    };
    optional_map_string_int_type.arguments = {map_string_int_type};

    FieldType optional_unordered_map_string_string_type{
        FieldTypeKind::Optional,
        "std::optional<std::unordered_map<std::string, std::string>>",
        "std::optional",
    };
    optional_unordered_map_string_string_type.arguments = {
        unordered_map_string_string_type};

    // 2. Add required and optional map fields.
    user.fields = {
        FieldModel{"labels", map_string_int_type,
                   JsonFieldMetadata{"labels", false},
                   SourceLocation{"include/map_user.hpp", 2, 37}},
        FieldModel{"aliases", unordered_map_string_string_type,
                   JsonFieldMetadata{"aliases", false},
                   SourceLocation{"include/map_user.hpp", 3, 58}},
        FieldModel{"optional_labels", optional_map_string_int_type,
                   JsonFieldMetadata{"optional_labels", true},
                   SourceLocation{"include/map_user.hpp", 4, 52}},
        FieldModel{"optional_aliases",
                   optional_unordered_map_string_string_type,
                   JsonFieldMetadata{"optional_aliases", true},
                   SourceLocation{"include/map_user.hpp", 5, 73}},
    };

    // 3. Return the one-type project.
    ProjectModel project;
    project.types = {user};
    return project;
}

} // namespace

int main() {
    assert_generated_matches(make_basic_user_project(),
                             "tests/golden/basic_user.expected.cjm.hpp",
                             "tests/golden/basic_user.actual.cjm.hpp");
    assert_generated_matches(make_map_user_project(),
                             "tests/golden/map_user.expected.cjm.hpp",
                             "tests/golden/map_user.actual.cjm.hpp");
    return 0;
}
