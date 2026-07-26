#pragma once

#include <string>
#include <vector>

#include "frontends/cxx/parser/syntax.hpp"

namespace cjm::frontends::cxx::tree_sitter {

struct ParseDiagnostic {
    std::string message;
    parser::SourceLocation location;
};

struct TreeSitterParseResult {
    bool success = false;
    parser::SourceFileSyntax file;
    std::vector<ParseDiagnostic> diagnostics;
};

/**
 * Parse C++ source text through Tree-sitter and return a CJM syntax shell.
 *
 * This adapter owns Tree-sitter integration. It must not leak Tree-sitter
 * nodes, trees, or parser handles outside the C++ frontend layer.
 */
TreeSitterParseResult parse_source_text(const std::string& path,
                                        const std::string& source);
} // namespace cjm::frontends::cxx::tree_sitter
