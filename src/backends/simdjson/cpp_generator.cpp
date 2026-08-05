#include "backends/simdjson/cpp_generator.hpp"

#include <cstddef>
#include <sstream>
#include <string>
#include <vector>

namespace cjm::generator::simdjson {
namespace {

// Return whether a Metadata IR kind has a complete generated decoder.
bool is_supported_scalar_kind(metadata::FieldTypeKind kind) {
    switch (kind) {
    case metadata::FieldTypeKind::Bool:
        return true;
    case metadata::FieldTypeKind::SignedInteger:
    case metadata::FieldTypeKind::UnsignedInteger:
    case metadata::FieldTypeKind::FloatingPoint:
    case metadata::FieldTypeKind::String:
    case metadata::FieldTypeKind::Enum:
    case metadata::FieldTypeKind::Array:
    case metadata::FieldTypeKind::Vector:
    case metadata::FieldTypeKind::Map:
    case metadata::FieldTypeKind::Optional:
    case metadata::FieldTypeKind::UserDefined:
        return false;
    }
    return false;
}

// Open the namespace containing one generated model.
void open_namespace(std::ostringstream& out,
                    const std::vector<std::string>& namespace_path) {
    if (namespace_path.empty()) {
        return;
    }

    out << "namespace ";
    for (std::size_t index = 0; index < namespace_path.size(); ++index) {
        if (index > 0) {
            out << "::";
        }
        out << namespace_path[index];
    }
    out << " {\n\n";
}

// Close the namespace containing one generated model.
void close_namespace(std::ostringstream& out,
                     const std::vector<std::string>& namespace_path) {
    if (namespace_path.empty()) {
        return;
    }

    out << "} // namespace ";
    for (std::size_t index = 0; index < namespace_path.size(); ++index) {
        if (index > 0) {
            out << "::";
        }
        out << namespace_path[index];
    }
    out << "\n";
}

// Generate the experimental decode error and structured path types.
void generate_decode_error_model(std::ostringstream& out) {
    out << "#ifndef CJM_EXPERIMENTAL_SIMDJSON_DECODE_ERROR_DEFINED\n"
        << "#define CJM_EXPERIMENTAL_SIMDJSON_DECODE_ERROR_DEFINED\n"
        << "\n"
        << "namespace cjm::experimental::simdjson {\n"
        << "\n"
        << "enum class DecodeErrorCode {\n"
        << "    none,\n"
        << "    syntax_error,\n"
        << "    expected_object,\n"
        << "    expected_bool,\n"
        << "    missing_required_field\n"
        << "};\n"
        << "\n"
        << "enum class DecodePathSegmentKind {\n"
        << "    field,\n"
        << "    index\n"
        << "};\n"
        << "\n"
        << "struct DecodePathSegment {\n"
        << "    DecodePathSegmentKind kind = "
           "DecodePathSegmentKind::field;\n"
        << "    std::string field_name;\n"
        << "    std::size_t index = 0;\n"
        << "};\n"
        << "\n"
        << "struct DecodeError {\n"
        << "    DecodeErrorCode code = DecodeErrorCode::none;\n"
        << "    std::vector<DecodePathSegment> path;\n"
        << "    ::simdjson::error_code runtime_error = "
           "::simdjson::SUCCESS;\n"
        << "};\n"
        << "\n"
        << "} // namespace cjm::experimental::simdjson\n"
        << "\n"
        << "#endif\n";
}

// Generate the required bool decoder for one model.
void generate_bool_decode_function(std::ostringstream& out,
                                   const metadata::TypeModel& type) {
    open_namespace(out, type.namespace_path);

    out << "inline std::optional<" << type.name << "> cjm_decode_simdjson_"
        << type.name << "(\n"
        << "    std::string_view input,\n"
        << "    ::cjm::experimental::simdjson::DecodeError& error) {\n"
        << "    using DecodeErrorCode =\n"
        << "        ::cjm::experimental::simdjson::DecodeErrorCode;\n"
        << "    using DecodePathSegmentKind =\n"
        << "        ::cjm::experimental::simdjson::"
           "DecodePathSegmentKind;\n"
        << "\n"
        << "    // 1. Prepare the padded input owned for this decode.\n"
        << "    error = {};\n"
        << "    ::simdjson::padded_string padded_input(input);\n"
        << "    ::simdjson::ondemand::parser parser;\n"
        << "\n"
        << "    // 2. Start one On-Demand document and read its root object.\n"
        << "    ::simdjson::ondemand::document document;\n"
        << "    auto runtime_error = "
           "parser.iterate(padded_input).get(document);\n"
        << "    if (runtime_error) {\n"
        << "        error.code = DecodeErrorCode::syntax_error;\n"
        << "        error.runtime_error = runtime_error;\n"
        << "        return std::nullopt;\n"
        << "    }\n"
        << "\n"
        << "    ::simdjson::ondemand::object object;\n"
        << "    runtime_error = document.get_object().get(object);\n"
        << "    if (runtime_error) {\n"
        << "        error.code = runtime_error == ::simdjson::INCORRECT_TYPE\n"
        << "                         ? DecodeErrorCode::expected_object\n"
        << "                         : DecodeErrorCode::syntax_error;\n"
        << "        error.runtime_error = runtime_error;\n"
        << "        return std::nullopt;\n"
        << "    }\n"
        << "\n"
        << "    // 3. Build a new object and track its required fields.\n"
        << "    " << type.name << " value{};\n";

    for (const auto& field : type.fields) {
        if (!field.json.ignored) {
            out << "    bool has_" << field.name << " = false;\n";
        }
    }

    out << "\n"
        << "    // 4. Visit each JSON field once.\n"
        << "    for (auto field : object) {\n"
        << "        std::string_view key;\n"
        << "        runtime_error = field.unescaped_key().get(key);\n"
        << "        if (runtime_error) {\n"
        << "            error.code = DecodeErrorCode::syntax_error;\n"
        << "            error.runtime_error = runtime_error;\n"
        << "            return std::nullopt;\n"
        << "        }\n"
        << "\n";

    for (const auto& field : type.fields) {
        if (field.json.ignored) {
            continue;
        }

        out << "        if (key == \"" << field.json.name << "\") {\n"
            << "            runtime_error = "
               "field.value().get_bool().get(value."
            << field.name << ");\n"
            << "            if (runtime_error) {\n"
            << "                error.code = DecodeErrorCode::expected_bool;\n"
            << "                error.path.push_back(\n"
            << "                    {DecodePathSegmentKind::field, \""
            << field.json.name << "\", 0});\n"
            << "                error.runtime_error = runtime_error;\n"
            << "                return std::nullopt;\n"
            << "            }\n"
            << "            has_" << field.name << " = true;\n"
            << "            continue;\n"
            << "        }\n"
            << "\n";
    }

    out << "    }\n"
        << "\n"
        << "    // 5. Verify that every required field was present.\n";

    for (const auto& field : type.fields) {
        if (field.json.ignored) {
            continue;
        }

        out << "    if (!has_" << field.name << ") {\n"
            << "        error.code = "
               "DecodeErrorCode::missing_required_field;\n"
            << "        error.path.push_back(\n"
            << "            {DecodePathSegmentKind::field, \""
            << field.json.name << "\", 0});\n"
            << "        return std::nullopt;\n"
            << "    }\n";
    }

    out << "\n"
        << "    // 6. Return the completely decoded object.\n"
        << "    return value;\n"
        << "}\n";

    close_namespace(out, type.namespace_path);
}

// Return the first unsupported-field diagnostic, or an empty string.
std::string validate_project(const metadata::ProjectModel& project) {
    for (const auto& type : project.types) {
        for (const auto& field : type.fields) {
            if (field.json.ignored) {
                continue;
            }
            if (is_supported_scalar_kind(field.type.kind)) {
                continue;
            }

            const auto& type_name = field.type.spelling.empty()
                                        ? field.type.qualified_name
                                        : field.type.spelling;

            return "simdjson backend does not support field '" + field.name +
                   "' of type " + type_name;
        }
    }
    return {};
}

} // namespace

GenerationResult generate_header(const metadata::ProjectModel& project) {
    const auto error = validate_project(project);
    if (!error.empty()) {
        return GenerationResult{false, {}, error};
    }

    std::ostringstream header;
    header << "// This file was generated by CJM.\n"
           << "// Do not edit this file manually.\n"
           << "\n"
           << "#pragma once\n"
           << "\n"
           << "#include <simdjson.h>\n"
           << "\n"
           << "#include <cstddef>\n"
           << "#include <optional>\n"
           << "#include <string>\n"
           << "#include <string_view>\n"
           << "#include <vector>\n"
           << "\n";

    generate_decode_error_model(header);

    for (const auto& type : project.types) {
        header << "\n";
        generate_bool_decode_function(header, type);
    }

    return GenerationResult{true, header.str(), {}};
}

} // namespace cjm::generator::simdjson
