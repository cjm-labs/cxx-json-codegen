#include "backends/contract/contract_generator.hpp"

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

void write_file(const std::string& path, const std::string& contents) {
    std::ofstream file(path);
    assert(file.is_open());
    file << contents;
}

// Build a small project that exercises contract metadata generation.
ProjectModel make_basic_user_project() {
    FieldType string_type{
        FieldTypeKind::String,
        "std::string",
        "std::string",
    };

    FieldType int_type{
        FieldTypeKind::SignedInteger,
        "int",
        "int",
    };

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

    FieldType optional_string_type{
        FieldTypeKind::Optional,
        "std::optional<std::string>",
        "std::optional",
    };
    optional_string_type.arguments = {string_type};

    FieldType optional_int_type{
        FieldTypeKind::Optional,
        "std::optional<int>",
        "std::optional",
    };
    optional_int_type.arguments = {int_type};
    FieldType map_string_int_type{
        FieldTypeKind::Map,
        "std::map<std::string, int>",
        "std::map",
    };
    map_string_int_type.arguments = {string_type, int_type};

    FieldType array_int_type{
        FieldTypeKind::Array,
        "std::array<int, 4>",
        "std::array",
    };
    array_int_type.arguments = {int_type};

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

    TypeModel user;
    user.name = "User";
    user.namespace_path = {"company", "model"};
    user.qualified_name = "company::model::User";
    user.source_location = SourceLocation{"include/user.hpp", 1, 8};
    user.fields = {
        FieldModel{
            "name",
            string_type,
            JsonFieldMetadata{"name", false},
            SourceLocation{"include/user.hpp", 2, 27},
        },
        FieldModel{
            "age",
            int_type,
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
            int_type,
            JsonFieldMetadata{"", false, true},
            SourceLocation{"include/user.hpp", 9, 9},
        },
        FieldModel{
            "attributes",
            map_string_int_type,
            JsonFieldMetadata{"attributes", false},
            SourceLocation{"include/user.hpp", 10, 42},
        },
        FieldModel{
            "samples",
            array_int_type,
            JsonFieldMetadata{"samples", false},
            SourceLocation{"include/user.hpp", 11, 35},
        },
    };

    EnumModel status;
    status.name = "Status";
    status.namespace_path = {"company", "model"};
    status.qualified_name = "company::model::Status";
    status.enumerators = {"Active", "Disabled"};
    status.source_location = SourceLocation{"include/user.hpp", 8, 1};

    ProjectModel project;
    project.types = {address, user};
    project.enums = {status};
    return project;
} // namespace

// Build a project that exercises recursive contract type descriptors.
ProjectModel make_recursive_contract_project() {
    FieldType string_type{
        FieldTypeKind::String,
        "std::string",
        "std::string",
    };

    FieldType int_type{
        FieldTypeKind::SignedInteger,
        "int",
        "int",
    };

    FieldType vector_int_type{
        FieldTypeKind::Vector,
        "std::vector<int>",
        "std::vector",
    };
    vector_int_type.arguments = {int_type};

    FieldType vector_vector_int_type{
        FieldTypeKind::Vector,
        "std::vector<std::vector<int>>",
        "std::vector",
    };
    vector_vector_int_type.arguments = {vector_int_type};

    FieldType vector_string_type{
        FieldTypeKind::Vector,
        "std::vector<std::string>",
        "std::vector",
    };
    vector_string_type.arguments = {string_type};

    FieldType optional_vector_string_type{
        FieldTypeKind::Optional,
        "std::optional<std::vector<std::string>>",
        "std::optional",
    };
    optional_vector_string_type.arguments = {vector_string_type};

    FieldType map_string_vector_int_type{
        FieldTypeKind::Map,
        "std::map<std::string, std::vector<int>>",
        "std::map",
    };
    map_string_vector_int_type.arguments = {string_type, vector_int_type};

    FieldType address_type{
        FieldTypeKind::UserDefined,
        "RecursiveAddress",
        "company::contract_test::RecursiveAddress",
    };

    FieldType vector_address_type{
        FieldTypeKind::Vector,
        "std::vector<RecursiveAddress>",
        "std::vector",
    };
    vector_address_type.arguments = {address_type};

    FieldType optional_address_type{
        FieldTypeKind::Optional,
        "std::optional<RecursiveAddress>",
        "std::optional",
    };
    optional_address_type.arguments = {address_type};

    FieldType map_string_address_type{
        FieldTypeKind::Map,
        "std::map<std::string, RecursiveAddress>",
        "std::map",
    };
    map_string_address_type.arguments = {string_type, address_type};

    FieldType vector_vector_address_type{
        FieldTypeKind::Vector,
        "std::vector<std::vector<RecursiveAddress>>",
        "std::vector",
    };
    vector_vector_address_type.arguments = {vector_address_type};

    TypeModel address;
    address.name = "RecursiveAddress";
    address.namespace_path = {"company", "contract_test"};
    address.qualified_name = "company::contract_test::RecursiveAddress";
    address.source_location = SourceLocation{"include/recursive_contract.hpp",
                                             10, 8};
    address.fields = {
        FieldModel{
            "city",
            string_type,
            JsonFieldMetadata{"city", false},
            SourceLocation{"include/recursive_contract.hpp", 11, 17},
        },
    };

    TypeModel recursive;
    recursive.name = "RecursiveContract";
    recursive.namespace_path = {"company", "contract_test"};
    recursive.qualified_name = "company::contract_test::RecursiveContract";
    recursive.source_location =
        SourceLocation{"include/recursive_contract.hpp", 14, 8};
    recursive.fields = {
        FieldModel{"matrix", vector_vector_int_type,
                   JsonFieldMetadata{"matrix", false},
                   SourceLocation{"include/recursive_contract.hpp", 15, 35}},
        FieldModel{"aliases", optional_vector_string_type,
                   JsonFieldMetadata{"aliases", true},
                   SourceLocation{"include/recursive_contract.hpp", 16, 45}},
        FieldModel{"buckets", map_string_vector_int_type,
                   JsonFieldMetadata{"buckets", false},
                   SourceLocation{"include/recursive_contract.hpp", 17, 51}},
        FieldModel{"addresses", vector_address_type,
                   JsonFieldMetadata{"addresses", false},
                   SourceLocation{"include/recursive_contract.hpp", 18, 43}},
        FieldModel{"maybe_address", optional_address_type,
                   JsonFieldMetadata{"maybe_address", true},
                   SourceLocation{"include/recursive_contract.hpp", 19, 49}},
        FieldModel{"address_by_id", map_string_address_type,
                   JsonFieldMetadata{"address_by_id", false},
                   SourceLocation{"include/recursive_contract.hpp", 20, 57}},
        FieldModel{"address_groups", vector_vector_address_type,
                   JsonFieldMetadata{"address_groups", false},
                   SourceLocation{"include/recursive_contract.hpp", 21, 56}},
    };

    ProjectModel project;
    project.types = {address, recursive};
    return project;
}

// Compare contract backend output with a golden file.
void assert_contract_matches(const ProjectModel& project,
                             const std::string& expected_path,
                             const std::string& actual_path) {
    const std::string generated =
        cjm::generator::contract::generate_header(project);
    const std::string expected = read_file(expected_path);

    if (generated != expected) {
        write_file(actual_path, generated);
        std::cerr << "contract golden mismatch\n"
                  << "expected: " << expected_path << "\n"
                  << "actual: " << actual_path << "\n";
    }

    assert(generated == expected);
}

} // namespace

int main() {
    assert_contract_matches(make_basic_user_project(),
                            "tests/golden/basic_user.expected.cjm.contract.hpp",
                            "tests/golden/basic_user.actual.cjm.contract.hpp");
    assert_contract_matches(
        make_recursive_contract_project(),
        "tests/golden/recursive_contract.expected.cjm.contract.hpp",
        "tests/golden/recursive_contract.actual.cjm.contract.hpp");

    return 0;
}
