#include "backends/nlohmann/cpp_generator.hpp"

#include <cstddef>
#include <sstream>
#include <string>
#include <vector>

namespace cjm::generator {

// Return the generated C++ spelling needed by generated nlohmann code.
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

// Return the generated helper name used for JSON string -> enum conversion.
std::string
enum_from_string_function_name(const metadata::EnumModel& enum_model) {
    return "cjm_from_json_string_" + enum_model.name;
}

// Return the unqualified type name from a possibly qualified C++ name.
std::string unqualified_type_name(const std::string& cpp_name) {
    const auto position = cpp_name.rfind("::");
    if (position == std::string::npos) {
        return cpp_name;
    }
    return cpp_name.substr(position + 2);
}

// Return the namespace prefix from a possibly qualified C++ name.
std::string namespace_prefix(const std::string& cpp_name) {
    const auto position = cpp_name.rfind("::");
    if (position == std::string::npos) {
        return "";
    }
    return cpp_name.substr(0, position + 2);
}

// Return the helper call name for enum -> JSON string conversion.
std::string enum_to_string_call_name(const metadata::FieldType& type) {
    const auto cpp_name = cpp_type_name(type);
    return namespace_prefix(cpp_name) + "cjm_to_json_string";
}

// Return the helper call name for JSON string -> enum conversion.
std::string enum_from_string_call_name(const metadata::FieldType& type) {
    const auto cpp_name = cpp_type_name(type);
    return namespace_prefix(cpp_name) + "cjm_from_json_string_" +
           unqualified_type_name(cpp_name);
}

// Generate enum -> JSON string conversion for one enum model.
void generate_enum_to_json_string(std::ostringstream& out,
                                  const metadata::EnumModel& enum_model) {
    out << "inline const char* cjm_to_json_string(" << enum_model.name
        << " value) {\n"
        << "    switch (value) {\n";

    for (const auto& enumerator : enum_model.enumerators) {
        out << "    case " << enum_model.name << "::" << enumerator << ":\n"
            << "        return \"" << enumerator << "\";\n";
    }

    out << "    }\n"
        << "    throw std::invalid_argument(\"unknown enum value for "
        << enum_model.qualified_name << "\");\n"
        << "}\n";
}

// Generate JSON string -> enum conversion for one enum model.
void generate_enum_from_json_string(std::ostringstream& out,
                                    const metadata::EnumModel& enum_model) {
    out << "inline " << enum_model.name << " "
        << enum_from_string_function_name(enum_model)
        << "(std::string_view value) {\n";

    for (const auto& enumerator : enum_model.enumerators) {
        out << "    if (value == \"" << enumerator << "\") {\n"
            << "        return " << enum_model.name << "::" << enumerator
            << ";\n"
            << "    }\n";
    }

    out << "    throw std::invalid_argument(\"unknown enum string for "
        << enum_model.qualified_name << "\");\n"
        << "}\n";
}

// Emit both enum/string helpers for one enum before model serializers.
void generate_enum_string_helpers(std::ostringstream& out,
                                  const metadata::EnumModel& enum_model) {
    generate_enum_to_json_string(out, enum_model);
    out << "\n";
    generate_enum_from_json_string(out, enum_model);
}

// Generate one to_json assignment from a validated Metadata IR field.
void generate_to_json_field(std::ostringstream& out,
                            const metadata::FieldModel& field) {
    if (field.json.ignored) {
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
    if (field.type.kind == metadata::FieldTypeKind::Enum) {
        out << "    j[\"" << field.json.name
            << "\"] = " << enum_to_string_call_name(field.type) << "(value."
            << field.name << ");\n";
        return;
    }
    out << "    j[\"" << field.json.name << "\"] = value." << field.name
        << ";\n";
}

// Generate one from_json assignment from a validated Metadata IR field.
void generate_from_json_field(std::ostringstream& out,
                              const metadata::FieldModel& field) {
    if (field.json.ignored) {
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

    if (field.type.kind == metadata::FieldTypeKind::Enum) {
        out << "    value." << field.name << " = "
            << enum_from_string_call_name(field.type) << "(j.at(\""
            << field.json.name << "\").get<std::string>());\n";
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
        << "#include <nlohmann/json.hpp>\n";

    if (!project.enums.empty()) {
        out << "#include <stdexcept>\n"
            << "#include <string>\n"
            << "#include <string_view>\n";
    }

    out << "\n";

    for (const auto& enum_model : project.enums) {
        open_namespace(out, enum_model.namespace_path);
        generate_enum_string_helpers(out, enum_model);
        out << "\n";
        close_namespace(out, enum_model.namespace_path);
        out << "\n";
    }

    for (std::size_t i = 0; i < project.types.size(); ++i) {
        const auto& type = project.types[i];

        open_namespace(out, type.namespace_path);
        generate_to_json(out, type);
        out << "\n";
        generate_from_json(out, type);
        out << "\n";
        close_namespace(out, type.namespace_path);

        if (i + 1 < project.types.size()) {
            out << "\n";
        }
    }

    return out.str();
}

} // namespace cjm::generator
