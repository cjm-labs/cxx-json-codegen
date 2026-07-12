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

    while (std::getline(input, line)) {
        ++line_number;
        const std::string stripped = trim(line);

        if (starts_with(stripped, "struct ")) {
            const auto name_start = std::string("struct ").size();
            const auto name_end = stripped.find_first_of(" {", name_start);

            if (name_end != std::string::npos) {
                DeclarationSyntax declaration;
                declaration.name =
                    stripped.substr(name_start, name_end - name_start);
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
