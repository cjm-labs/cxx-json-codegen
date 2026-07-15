#include <cctype>
#include <fstream>

#include "parser/parser.hpp"

namespace cjm::parser {

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

bool starts_with(const std::string& text, const std::string& prefix) {
    return text.rfind(prefix, 0) == 0;
}

std::vector<std::string> split_namespace_path(const std::string& text) {
    std::vector<std::string> result;
    std::size_t start = 0;

    while (start < text.size()) {
        const auto end = text.find("::", start);
        if (end == std::string::npos) {
            result.push_back(text.substr(start));
            break;
        }

        result.push_back(text.substr(start, end - start));
        start = end + 2;
    }

    return result;
}

ParseResult parse_source_file(const std::string& path) {
    std::ifstream input(path);

    if (!input.is_open()) {
        ParseResult result;
        result.success = false;
        result.error.message = "failed to open source file";
        result.error.location.file = path;
        return result;
    }
    ParseResult result;
    result.success = true;
    result.file.path = path;

    std::string line;
    int line_number = 0;
    DeclarationSyntax* current_declaration = nullptr;
    std::vector<std::string> namespace_path;
    std::vector<std::size_t> namespace_block_sizes;

    while (std::getline(input, line)) {
        ++line_number;
        const std::string stripped = trim(line);

        if (current_declaration == nullptr &&
            starts_with(stripped, "namespace ")) {
            const auto name_start = std::string("namespace ").size();
            const auto brace = stripped.find("{", name_start);
            if (brace != std::string::npos) {
                const auto namespace_name =
                    trim(stripped.substr(name_start, brace - name_start));
                const auto parts = split_namespace_path(namespace_name);

                namespace_path.insert(namespace_path.end(), parts.begin(),
                                      parts.end());
                namespace_block_sizes.push_back(parts.size());
            }
            continue;
        }

        // ?
        if (current_declaration == nullptr && starts_with(stripped, "}") &&
            stripped != "};") {
            if (!namespace_block_sizes.empty()) {
                const auto block_size = namespace_block_sizes.back();
                namespace_block_sizes.pop_back();

                for (std::size_t i = 0;
                     i < block_size && !namespace_path.empty(); ++i) {
                    namespace_path.pop_back();
                }
            }
            continue;
        }

        if (current_declaration == nullptr && starts_with(stripped, "using ")) {
            const auto name_start = std::string("using ").size();
            const auto equals = stripped.find("=", name_start);
            const auto semicolon = stripped.find(";", name_start);

            if (equals != std::string::npos && semicolon != std::string::npos &&
                equals < semicolon) {
                TypeAliasSyntax alias;
                alias.name =
                    trim(stripped.substr(name_start, equals - name_start));
                alias.target_type_spelling =
                    trim(stripped.substr(equals + 1, semicolon - equals - 1));
                alias.namespace_path = namespace_path;
                alias.location.file = path;
                alias.location.line = line_number;
                alias.location.column =
                    static_cast<int>(line.find("using")) + 1;
                result.file.type_aliases.push_back(alias);
            }
            continue;
        }

        if (current_declaration == nullptr &&
            (starts_with(stripped, "enum ") ||
             starts_with(stripped, "enum class "))) {
            const std::string prefix =
                starts_with(stripped, "enum class ") ? "enum class " : "enum ";
            const auto name_start = prefix.size();
            const auto name_end = stripped.find_first_of(" {:", name_start);

            if (name_end != std::string::npos) {
                EnumSyntax enum_syntax;
                enum_syntax.name =
                    stripped.substr(name_start, name_end - name_start);
                enum_syntax.namespace_path = namespace_path;
                enum_syntax.location.file = path;
                enum_syntax.location.line = line_number;
                enum_syntax.location.column =
                    static_cast<int>(line.find("enum")) + 1;
                result.file.enums.push_back(enum_syntax);
            }
            continue;
        }

        if (starts_with(stripped, "struct ")) {
            const auto name_start = std::string("struct ").size();
            const auto name_end = stripped.find_first_of(" {", name_start);

            if (name_end != std::string::npos) {
                DeclarationSyntax declaration;
                declaration.name =
                    stripped.substr(name_start, name_end - name_start);
                declaration.namespace_path = namespace_path;
                declaration.location.file = path;
                declaration.location.line = line_number;
                declaration.location.column =
                    static_cast<int>(line.find("struct")) + 1;

                result.file.declarations.push_back(declaration);
                current_declaration = &result.file.declarations.back();
            }
            continue;
        }

        if (stripped == "};") {
            current_declaration = nullptr;
            continue;
        }
        if (current_declaration == nullptr) {
            continue;
        }

        // parse comments
        std::string code_text = stripped;
        std::string comment_text;
        const auto comment_start = stripped.find("//");
        if (comment_start != std::string::npos) {
            code_text = trim(stripped.substr(0, comment_start));
            comment_text = trim(stripped.substr(comment_start + 2));
        }

        const auto semicolon = code_text.find(";");
        if (semicolon == std::string::npos) {
            continue;
        }

        std::string field_text = code_text.substr(0, semicolon);
        const auto initializer = field_text.find('=');
        if (initializer != std::string::npos) {
            field_text = trim(field_text.substr(0, initializer));
        }

        const auto split = field_text.find_last_of(" \t");
        if (split == std::string::npos) {
            continue;
        }

        FieldSyntax field;
        field.type_spelling = trim(field_text.substr(0, split));
        field.name = trim(field_text.substr(split + 1));
        field.location.file = path;
        field.location.line = line_number;
        field.location.column = static_cast<int>(line.find(field.name)) + 1;

        if (!comment_text.empty()) {
            CommentSyntax comment;
            comment.text = comment_text;
            comment.location.file = path;
            comment.location.line = line_number;
            comment.location.column = static_cast<int>(line.find("//")) + 1;
            field.comments.push_back(comment);
        }
        current_declaration->fields.push_back(field);
    }

    return result;
}

} // namespace cjm::parser
