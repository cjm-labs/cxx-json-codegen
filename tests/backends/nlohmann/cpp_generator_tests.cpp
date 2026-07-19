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
    };

    ProjectModel project;
    project.types = {user};
    return project;
}

} // namespace

int main() {
    const ProjectModel project = make_basic_user_project();

    const std::string expected_path =
        "tests/golden/basic_user.expected.cjm.hpp";
    const std::string actual_path = "tests/golden/basic_user.actual.cjm.hpp";

    const std::string generated = generate_header(project);
    const std::string expected = read_file(expected_path);

    if (generated != expected) {
        write_file(actual_path, generated);
        std::cerr << "golden mismatch\n"
                  << "expected: " << expected << "\n"
                  << "actual: " << generated << "\n";
    }
    assert(generated == expected);
    return 0;
}
