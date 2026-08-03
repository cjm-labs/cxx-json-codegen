#include "backends/contract/contract_generator.hpp"

#include <cstddef>
#include <sstream>
#include <string>
#include <vector>

namespace cjm::generator::contract {
// Return the generated C++ spelling for a bool literal.
std::string cpp_bool_literal(bool value) { return value ? "true" : "false"; }

// Generate one source_location initializer for public contract metadata.
void generate_contract_source_location(
    std::ostringstream& out, const metadata::SourceLocation& location) {
    out << "{\"" << location.file << "\", " << location.line << ", "
        << location.column << "}";
}

// Return a stable generated identifier for one field's type descriptor.
std::string
contract_field_type_descriptor_name(const metadata::FieldModel& field) {
    return field.name + "_type";
}

// Return the generated array name that stores one descriptor's type arguments.
std::string contract_type_arguments_name(const std::string& descriptor_name) {
    return descriptor_name + "_arguments";
}

// Return the generated descriptor name for one nested type argument.
std::string
contract_type_argument_descriptor_name(const std::string& descriptor_name,
                                       std::size_t index) {
    return descriptor_name + "_arg" + std::to_string(index);
}

// Return the generated namespace that owns one model's contract descriptors.
std::string contract_generated_namespace_name(const metadata::TypeModel& type) {
    std::string name = "cjm::contract::generated_";

    for (std::size_t i = 0; i < type.namespace_path.size(); ++i) {
        name += type.namespace_path[i];
        name += "_";
    }

    name += type.name;
    return name;
}

void generate_contract_type_descriptor(
    std::ostringstream& out, const std::string& descriptor_name,
    const metadata::FieldType& type,
    const std::vector<metadata::EnumModel>& enums);

// Return the generated array name that stores enum value descriptors.
//
// Example:
//      descriptor_name: status_type
//      Output:          status_type_enum_values
std::string contract_enum_values_name(const std::string& descriptor_name) {
    return descriptor_name + "_enum_values";
}

// Find the enum model that matches an enum field type.
//
// Example:
//  Field type qualified_name: company::model::Status
//  Project enum qualified_name: company::model::Status
//  Result: pointer to that EnumModel
const metadata::EnumModel*
find_enum_model(const std::vector<metadata::EnumModel>& enums,
                const metadata::FieldType& type) {
    if (type.kind != metadata::FieldTypeKind::Enum) {
        return nullptr;
    }

    for (const auto& enum_model : enums) {
        if (enum_model.qualified_name == type.qualified_name) {
            return &enum_model;
        }
    }
    return nullptr;
}

// Generate public enum value descriptors for one enum field type.
//
// Example output:
//  inline constexpr enum_value_descriptor status_type_enum_values[] = {
//      {"Active", "Active"},
//      {"Disabled", "Disabled"},
//  };
void generate_contract_enum_values(std::ostringstream& out,
                                   const std::string& descriptor_name,
                                   const metadata::EnumModel* enum_model) {
    if (enum_model == nullptr || enum_model->enumerators.empty()) {
        return;
    }
    out << "inline constexpr cjm::contract::enum_value_descriptor "
        << contract_enum_values_name(descriptor_name) << "[] = {\n";
    for (const auto& enumerator : enum_model->enumerators) {
        out << "    {\"" << enumerator << "\", \"" << enumerator << "\"},\n";
    }
    out << "};\n\n";
}

// Generate nested type descriptors used by vector/optional/map descriptors.
void generate_contract_type_arguments(
    std::ostringstream& out, const std::string& descriptor_name,
    const metadata::FieldType& type,
    const std::vector<metadata::EnumModel>& enums) {
    if (type.arguments.empty()) {
        return;
    }

    for (std::size_t i = 0; i < type.arguments.size(); ++i) {
        generate_contract_type_descriptor(
            out, contract_type_argument_descriptor_name(descriptor_name, i),
            type.arguments[i], enums);
        out << "\n";
    }

    out << "inline constexpr cjm::contract::type_descriptor "
        << contract_type_arguments_name(descriptor_name) << "[] = {\n";
    for (std::size_t i = 0; i < type.arguments.size(); ++i) {
        out << "    "
            << contract_type_argument_descriptor_name(descriptor_name, i)
            << ",\n";
    }
    out << "};\n\n";
}

// Return the C++ type spelling recorded in generated contract metadata.
std::string cpp_type_name(const metadata::FieldType& type) {
    switch (type.kind) {
    case metadata::FieldTypeKind::Bool:
    case metadata::FieldTypeKind::SignedInteger:
    case metadata::FieldTypeKind::UnsignedInteger:
    case metadata::FieldTypeKind::FloatingPoint:
    case metadata::FieldTypeKind::String:
    case metadata::FieldTypeKind::Enum:
    case metadata::FieldTypeKind::UserDefined:
        return type.qualified_name.empty() ? type.spelling
                                           : type.qualified_name;

    case metadata::FieldTypeKind::Array:
        return type.spelling;
    case metadata::FieldTypeKind::Vector:
        return "std::vector<" + cpp_type_name(type.arguments[0]) + ">";
    case metadata::FieldTypeKind::Optional:
        return "std::optional<" + cpp_type_name(type.arguments[0]) + ">";
    case metadata::FieldTypeKind::Map:
        return type.qualified_name + "<" + cpp_type_name(type.arguments[0]) +
               ", " + cpp_type_name(type.arguments[1]) + ">";
    }
    return type.spelling;
}

// Return the generated contract enum spelling for a validated Metadata IR type.
std::string contract_type_kind_name(metadata::FieldTypeKind kind) {
    switch (kind) {
    case metadata::FieldTypeKind::Bool:
        return "cjm::contract::type_kind::bool_";
    case metadata::FieldTypeKind::SignedInteger:
        return "cjm::contract::type_kind::signed_integer";
    case metadata::FieldTypeKind::UnsignedInteger:
        return "cjm::contract::type_kind::unsigned_integer";
    case metadata::FieldTypeKind::FloatingPoint:
        return "cjm::contract::type_kind::floating_point";
    case metadata::FieldTypeKind::String:
        return "cjm::contract::type_kind::string";
    case metadata::FieldTypeKind::Enum:
        return "cjm::contract::type_kind::enum_";
    case metadata::FieldTypeKind::Array:
        return "cjm::contract::type_kind::array";
    case metadata::FieldTypeKind::Vector:
        return "cjm::contract::type_kind::vector";
    case metadata::FieldTypeKind::Map:
        return "cjm::contract::type_kind::map";
    case metadata::FieldTypeKind::Optional:
        return "cjm::contract::type_kind::optional";
    case metadata::FieldTypeKind::UserDefined:
        return "cjm::contract::type_kind::object";
    }
    return "cjm::contract::type_kind::object";
}

// Generate one public contract type descriptor for a validated field type.
void generate_contract_type_descriptor(
    std::ostringstream& out, const std::string& descriptor_name,
    const metadata::FieldType& type,
    const std::vector<metadata::EnumModel>& enums) {
    generate_contract_type_arguments(out, descriptor_name, type, enums);
    const auto* enum_model = find_enum_model(enums, type);
    generate_contract_enum_values(out, descriptor_name, enum_model);
    // Write the descriptor declaration.
    out << "inline constexpr cjm::contract::type_descriptor " << descriptor_name
        << "{\n";

    // Record the stable public type category.
    out << "    " << contract_type_kind_name(type.kind) << ",\n";

    // Preserve the user-facing C++ spelling and resolved qualified_name.
    out << "    \"" << cpp_type_name(type) << "\",\n"
        << "    \"" << cpp_type_name(type) << "\",\n";

    // Nested type arguments are added in a later step.
    if (type.arguments.empty()) {
        out << "    nullptr,\n";
    } else {
        out << "    " << contract_type_arguments_name(descriptor_name) << ",\n";
    }
    out << "    " << type.arguments.size() << ",\n"
        << "    ";
    if (enum_model == nullptr || enum_model->enumerators.empty()) {
        out << "nullptr,\n";
    } else {
        out << contract_enum_values_name(descriptor_name) << ",\n";
    }
    out << "    "
        << (enum_model == nullptr ? 0 : enum_model->enumerators.size()) << ",\n"
        << "};\n";
}

// Generate one public contract field descriptor.
void generate_contract_field_descriptor(std::ostringstream& out,
                                        const metadata::FieldModel& field) {
    const bool ignored = field.json.ignored;

    out << "    {\n"
        << "        \"" << field.name << "\",\n"
        << "        \"" << field.json.name << "\",\n"
        << "        " << cpp_bool_literal(ignored) << ",\n"
        << "        " << cpp_bool_literal(field.json.omit_empty) << ",\n"
        << "        ";
    generate_contract_source_location(out, field.source_location);
    out << ",\n"
        << "        &" << contract_field_type_descriptor_name(field) << ",\n"
        << "    },\n";
}

// Generate public contract field descriptors for one model.
void generate_contract_field_descriptors(std::ostringstream& out,
                                         const metadata::TypeModel& type) {
    out << "inline constexpr cjm::contract::field_descriptor fields[] = {\n";
    for (const auto& field : type.fields) {
        generate_contract_field_descriptor(out, field);
    }
    out << "};\n";
}

// Generate all contract descriptors for one model namespace.
void generate_contract_model_namespace(
    std::ostringstream& out, const metadata::TypeModel& type,
    const std::vector<metadata::EnumModel>& enums) {
    out << "namespace " << contract_generated_namespace_name(type) << " {\n\n";

    for (const auto& field : type.fields) {
        generate_contract_type_descriptor(
            out, contract_field_type_descriptor_name(field), field.type, enums);
        out << "\n";
    }

    generate_contract_field_descriptors(out, type);
    out << "\n";

    out << "} // namespace " << contract_generated_namespace_name(type) << "\n";
}

// Generate the public model_traits<T> entry point for one model.
void generate_contract_model_traits(std::ostringstream& out,
                                    const metadata::TypeModel& type) {
    out << "template <>\n"
        << "struct cjm::contract::model_traits<" << type.qualified_name
        << "> {\n"
        << "    static constexpr cjm::contract::model_descriptor model{\n"
        << "        \"" << type.name << "\",\n"
        << "        \"" << type.qualified_name << "\",\n"
        << "        ";
    generate_contract_source_location(out, type.source_location);
    out << ",\n"
        << "        " << contract_generated_namespace_name(type)
        << "::fields,\n"
        << "        " << type.fields.size() << ",\n"
        << "    };\n"
        << "};\n";
}

std::string generate_header(const metadata::ProjectModel& project) {
    std::ostringstream out;

    out << "// This file was generated by CJM.\n"
        << "// Do not edit this file manually.\n"
        << "\n"
        << "#pragma once\n"
        << "\n"
        << "#include <cjm/model_contract.hpp>\n"
        << "\n";

    for (std::size_t i = 0; i < project.types.size(); ++i) {
        generate_contract_model_namespace(out, project.types[i], project.enums);
        out << "\n";
        generate_contract_model_traits(out, project.types[i]);
        if (i + 1 < project.types.size()) {
            out << "\n";
        }
    }

    return out.str();
}

} // namespace cjm::generator::contract
