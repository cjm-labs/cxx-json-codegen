#include "backends/nlohmann/cpp_generator.hpp"

#include <cstddef>
#include <sstream>
#include <vector>

namespace cjm::generator {

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

// Return the generated C++ spelling for a validated Metadata IR type.
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
void generate_contract_type_descriptor(std::ostringstream& out,
                                       const std::string& descriptor_name,
                                       const metadata::FieldType& type) {
    // Write the descriptor declaration.
    out << "inline constexpr cjm::contract::type_descriptor " << descriptor_name
        << "{\n";

    // Record the stable public type category.
    out << "    " << contract_type_kind_name(type.kind) << ",\n";

    // Preserve the user-facing C++ spelling and resolved qualified_name.
    out << "    \"" << cpp_type_name(type) << "\",\n"
        << "    \"" << cpp_type_name(type) << "\",\n";

    // Nested type arguments are added in a later step.
    out << "    nullptr,\n"
        << "    0,\n"
        << "};\n";
}

// Generate one public contract field descriptor.
void generate_contract_field_descriptor(std::ostringstream& out,
                                        const metadata::FieldModel& field) {
    const bool ignored = field.json.name.empty();

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

// Generate initial contract type descriptors for one model.
void generate_contract_type_descriptors(std::ostringstream& out,
                                        const metadata::TypeModel& type) {
    out << "namespace cjm::contract::generated_";
    for (std::size_t i = 0; i < type.namespace_path.size(); ++i) {
        out << type.namespace_path[i] << "_";
    }

    out << type.name << " {\n\n";

    for (const auto& field : type.fields) {
        generate_contract_type_descriptor(
            out, contract_field_type_descriptor_name(field), field.type);
        out << "\n";
    }

    generate_contract_field_descriptors(out, type);
    out << "\n";
    out << "} // namespace cjm::contract::generated_";

    for (std::size_t i = 0; i < type.namespace_path.size(); ++i) {
        out << type.namespace_path[i] << "_";
    }
    out << type.name << "\n";
}

void open_namespace(std::ostringstream& out,
                    const std::vector<std::string>& namespace_path) {

    if (namespace_path.empty()) {
        return;
    }

    out << "namespace ";
    for (std::size_t i = 0; i < namespace_path.size(); ++i) {
        if (i > 0) {
            out << "::";
        }
        out << namespace_path[i];
    }
    out << " {\n\n";
}

void close_namespace(std::ostringstream& out,
                     const std::vector<std::string>& namespace_path) {
    if (namespace_path.empty()) {
        return;
    }

    out << "} // namespace ";
    for (std::size_t i = 0; i < namespace_path.size(); ++i) {
        if (i > 0) {
            out << "::";
        }
        out << namespace_path[i];
    }
    out << "\n";
}

// Generate one to_json assignment from a validated Metadata IR field.
void generate_to_json_field(std::ostringstream& out,
                            const metadata::FieldModel& field) {
    if (field.json.name.empty()) {
        return;
    }
    if (field.type.kind == metadata::FieldTypeKind::Optional &&
        field.json.omit_empty) {
        out << "    if (value." << field.name << ".has_value()) {\n"
            << "        j[\"" << field.json.name << "\"] = *value."
            << field.name << ";\n"
            << "    }\n";

        return;
    }
    out << "    j[\"" << field.json.name << "\"] = value." << field.name
        << ";\n";
}

// Generate one from_json assignment from a validated Metadata IR field.
void generate_from_json_field(std::ostringstream& out,
                              const metadata::FieldModel& field) {
    if (field.json.name.empty()) {
        return;
    }
    if (field.type.kind == metadata::FieldTypeKind::Optional &&
        field.json.omit_empty) {
        out << "    if (j.contains(\"" << field.json.name << "\")) {\n"
            << "        value." << field.name << " = j.at(\"" << field.json.name
            << "\").get<" << cpp_type_name(field.type.arguments[0]) << ">();\n"
            << "    }\n";
        return;
    }

    out << "    j.at(\"" << field.json.name << "\").get_to(value." << field.name
        << ");\n";
}

void generate_to_json(std::ostringstream& out,
                      const metadata::TypeModel& type) {
    out << "inline void to_json(nlohmann::json& j, const " << type.name
        << "& value) {\n";
    for (const auto& field : type.fields) {
        generate_to_json_field(out, field);
    }
    out << "}\n";
}

void generate_from_json(std::ostringstream& out,
                        const metadata::TypeModel& type) {

    out << "inline void from_json(const nlohmann::json& j, " << type.name
        << "& value) {\n";

    for (const auto& field : type.fields) {
        generate_from_json_field(out, field);
    }
    out << "}\n";
}

std::string generate_header(const metadata::ProjectModel& project) {

    std::ostringstream out;

    out << "// This file was generated by CJM.\n"
        << "// Do not edit this file manually.\n"
        << "\n"
        << "#pragma once\n"
        << "\n"
        << "#include <nlohmann/json.hpp>\n"
        << "#include <cjm/model_contract.hpp>\n";

    out << "\n";

    for (std::size_t i = 0; i < project.types.size(); ++i) {
        const auto& type = project.types[i];

        open_namespace(out, type.namespace_path);
        generate_to_json(out, type);
        out << "\n";
        generate_from_json(out, type);
        out << "\n";
        close_namespace(out, type.namespace_path);
        if (!type.namespace_path.empty()) {
            out << "\n";
        }
        generate_contract_type_descriptors(out, type);

        if (i + 1 < project.types.size()) {
            out << "\n";
        }
    }

    return out.str();
}

} // namespace cjm::generator
