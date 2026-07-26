#include <fstream>
#include <sstream>

#include "frontends/cxx/parser/parser.hpp"
#include "frontends/cxx/tree_sitter/adapter.hpp"

namespace cjm::parser {

namespace {
/**
 * Read a source file into memory before handing it to the active parser.
 *
 * File I/O belongs to parse_source_file. Syntax extraction belongs to the
 * frontend parser implementation.
 */
bool read_source_file(const std::string& path, std::string& source) {
    std::ifstream input(path);
    if (!input.is_open()) {
        return false;
    }

    std::ostringstream buffer;
    buffer << input.rdbuf();
    source = buffer.str();
    return true;
}

/**
 * Convert Tree-sitter adapter diagnostics into the public parser result shape.
 */
ParseResult convert_tree_sitter_result(
    const cjm::frontends::cxx::tree_sitter::TreeSitterParseResult& parsed) {

    ParseResult result;
    result.file = parsed.file;
    result.success = parsed.success;

    if (!parsed.diagnostics.empty()) {
        result.error.message = parsed.diagnostics[0].message;
        result.error.location = parsed.diagnostics[0].location;
    }
    return result;
}

} // namespace

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
    // 1. Read the user-provided source file.
    std::string source;
    if (!read_source_file(path, source)) {
        ParseResult result;
        result.success = false;
        result.error.message = "failed to open source file";
        result.error.location.file = path;
        return result;
    }

    // 2. Parse C++ syntax through the default Tree-sitter frontend.
    const auto parsed =
        cjm::frontends::cxx::tree_sitter::parse_source_text(path, source);

    // 3. Preserve the existing public parser result shape.
    return convert_tree_sitter_result(parsed);
}

} // namespace cjm::parser
