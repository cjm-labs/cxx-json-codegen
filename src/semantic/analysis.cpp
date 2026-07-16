#include <set>
#include <cctype>
#include "semantic/analysis.hpp"

namespace cjm::semantic {

namespace {

std::string trim(const std::string& text) {
    std::size_t first = 0;
    while (first < text.size() &&
           std::isspace(static_cast<unsigned char>(text[first]))) {
        ++first;
    }

    std::size_t last = text.size();
    while (last > first &&
           std::isspace(static_cast<unsigned char>(text[last - 1]))) {
        --last;
    }

    return text.substr(first, last - first);
}

std::vector<std::string> split_options(const std::string& text) {
    std::vector<std::string> result;
    std::size_t start = 0;

    while (start <= text.size()) {
        const auto comma = text.find(",", start);
        if (comma == std::string::npos) {
            result.push_back(trim(text.substr(start)));
            break;
        }

        result.push_back(trim(text.substr(start, comma - start)));
        start = comma + 1;
    }
    return result;
}

} // namespace

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

    const auto body = value.substr(1, value.size() - 2);
    const auto options = split_options(body);

    if (options.empty() || options[0].empty()) {
        result.success = false;
        result.diagnostic.location = location;
        result.diagnostic.message =
            "invalid CJM json metadata: empty JSON field name";
        return result;
    }

    result.json_name = options[0];
    if (result.json_name == "-") {
        result.ignored = true;
    }

    for (std::size_t i = 1; i < options.size(); ++i) {
        if (options[i] == "omitempty") {
            result.omit_empty = true;
            continue;
        }

        if (options[i].empty()) {
            continue;
        }

        result.success = false;
        result.diagnostic.location = location;
        result.diagnostic.message =
            "unsupported CJM json metadata option: " + options[i];
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

                if (metadata_result.ignored) {
                    break;
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
                field.json.omit_empty = metadata_result.omit_empty;

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
