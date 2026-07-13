#include <set>

#include "semantic/analysis.hpp"

namespace cjm::semantic {

JsonFieldMetadataResult
parse_json_field_metadata(const std::string& comment,
                          const SourceLocation& location) {
    JsonFieldMetadataResult result;

    const std::string prefix = "json:";
    if (comment.rfind(prefix, 0) != 0) {
        result.found = false;
        return result;
    }

    result.found = true;
    const auto value = comment.substr(prefix.size());
    if (value.size() < 2 || value.front() != '"' || value.back() != '"') {
        result.success = false;
        result.diagnostic.location = location;
        result.diagnostic.message =
            R"(invalid CJM json metadata, expected json:"name")";
        return result;
    }

    result.json_name = value.substr(1, value.size() - 2);
    if (result.json_name.empty()) {
        result.success = false;
        result.diagnostic.location = location;
        result.diagnostic.message =
            "invalid CJM json metadata: empty JSON field name";
        return result;
    }
    result.success = true;
    return result;
}

SourceLocation to_semantic_location(const parser::SourceLocation& location) {
    SourceLocation result;
    result.file = location.file;
    result.line = location.line;
    result.column = location.column;
    return result;
}

AnalysisResult analyze_source_file(const parser::SourceFileSyntax& file) {
    AnalysisResult result;
    result.success = true;

    for (const auto& declaration : file.declarations) {
        metadata::TypeModel type;
        type.name = declaration.name;
        std::set<std::string> json_names;

        for (const auto& field_syntax : declaration.fields) {
            for (const auto& comment : field_syntax.comments) {
                const auto comment_location =
                    to_semantic_location(comment.location);
                const auto metadata_result =
                    parse_json_field_metadata(comment.text, comment_location);

                if (!metadata_result.found) {
                    continue;
                }

                if (!metadata_result.success) {
                    result.success = false;
                    result.diagnostics.push_back(metadata_result.diagnostic);
                    continue;
                }

                if (json_names.count(metadata_result.json_name) != 0) {
                    result.success = false;

                    Diagnostic diagnostic;
                    diagnostic.location = comment_location;
                    diagnostic.message = "duplicate JSON field name: " +
                                         metadata_result.json_name;
                    result.diagnostics.push_back(diagnostic);
                    continue;
                }

                json_names.insert(metadata_result.json_name);

                metadata::FieldModel field;
                field.name = field_syntax.name;
                field.type.spelling = field_syntax.type_spelling;
                field.json.name = metadata_result.json_name;

                type.fields.push_back(field);
                break;
            }
        }
        if (!type.fields.empty()) {
            result.project.types.push_back(type);
        }
    }
    return result;
}
} // namespace cjm::semantic
