#include "backends/schema/schema_generator.hpp"

#include <sstream>
#include <string>

namespace cjm::generator::schema {

std::string schema_type_name(metadata::FieldTypeKind kind) {
    switch (kind) {
    case metadata::FieldTypeKind::Bool:
        return "boolean";
    case metadata::FieldTypeKind::SignedInteger:
    case metadata::FieldTypeKind::UnsignedInteger:
        return "integer";
    case metadata::FieldTypeKind::FloatingPoint:
        return "number";
    case metadata::FieldTypeKind::String:
        return "string";
    default:
        return "object";
    }
}

bool is_supported_simple_schema_type(metadata::FieldTypeKind kind) {
    return kind == metadata::FieldTypeKind::Bool ||
           kind == metadata::FieldTypeKind::SignedInteger ||
           kind == metadata::FieldTypeKind::UnsignedInteger ||
           kind == metadata::FieldTypeKind::FloatingPoint ||
           kind == metadata::FieldTypeKind::String;
}

void generate_field_schema(std::ostringstream& out,
                           const metadata::FieldModel& field,
                           const std::string& indent) {
    out << indent << "\"" << field.json.name << "\": { ";
    out << "\"type\": \"" << schema_type_name(field.type.kind) << "\"";
    if (field.type.kind == metadata::FieldTypeKind::UnsignedInteger) {
        out << ", \"minimum\": 0";
    }
    out << " }";
}

void generate_type_schema(std::ostringstream& out,
                          const metadata::TypeModel& type) {
    out << "{\n"
        << "  \"$schema\": \"https://json-schema.org/draft/2020-12/schema\",\n"
        << "  \"title\": \"" << type.name << "\",\n"
        << "  \"type\": \"object\",\n"
        << "  \"properties\": {\n";

    bool wrote_property = false;
    for (const auto& field : type.fields) {
        if (field.json.name.empty()) {
            continue;
        }
        if (!is_supported_simple_schema_type(field.type.kind)) {
            continue;
        }
        if (wrote_property) {
            out << ",\n";
        }
        generate_field_schema(out, field, "    ");
        wrote_property = true;
    }

    out << "\n"
        << "  },\n"
        << "  \"required\": [";

    bool wrote_required = false;
    for (const auto& field : type.fields) {
        if (field.json.name.empty() ||
            !is_supported_simple_schema_type(field.type.kind)) {
            continue;
        }
        if (wrote_required) {
            out << ", ";
        }
        out << "\"" << field.json.name << "\"";
        wrote_required = true;
    }
    out << "]\n"
        << "}\n";
}

std::string generate_schema(const metadata::ProjectModel& project) {
    std::ostringstream out;
    if (!project.types.empty()) {
        generate_type_schema(out, project.types.front());
    }
    return out.str();
}

} // namespace cjm::generator::schema
