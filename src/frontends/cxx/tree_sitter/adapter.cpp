#include "frontends/cxx/tree_sitter/adapter.hpp"

#include <cstdint>
#include <tree_sitter/api.h>
#include "tree-sitter-cpp.h"

namespace cjm::frontends::cxx::tree_sitter {
namespace {
/**
 * Convert a Tree-sitter point to CJM's source location shape.
 */
parser::SourceLocation to_source_location(const std::string& path,
                                          const TSPoint& point) {
    return parser::SourceLocation{
        path,
        static_cast<int>(point.row) + 1,
        static_cast<int>(point.column) + 1,
    };
}

} // namespace

TreeSitterParseResult parse_source_text(const std::string& path,
                                        const std::string& source) {
    TreeSitterParseResult result;
    result.file.path = path;

    // 1. Create a Tree-sitter parser for C++.
    auto* parser = ts_parser_new();
    if (parser == nullptr) {
        result.diagnostics.push_back(ParseDiagnostic{
            "failed to create tree-sitter parser",
            parser::SourceLocation{path, 1, 1},
        });
        return result;
    }

    if (!ts_parser_set_language(parser, tree_sitter_cpp())) {
        result.diagnostics.push_back(ParseDiagnostic{
            "failed to attach tree-sitter-cpp grammar",
            parser::SourceLocation{path, 1, 1},
        });
        ts_parser_delete(parser);
        return result;
    }

    // 2. Parse source text into a syntax tree
    auto* tree =
        ts_parser_parse_string(parser, nullptr, source.c_str(),
                               static_cast<std::uint32_t>(source.size()));

    if (tree == nullptr) {
        result.diagnostics.push_back(ParseDiagnostic{
            "failed to parse source with Tree-sitter",
            parser::SourceLocation{path, 1, 1},
        });
        ts_parser_delete(parser);
        return result;
    }

    // 3. Fail closed if the root tree contains syntax errors.
    const auto root = ts_tree_root_node(tree);
    if (ts_node_has_error(root)) {
        result.diagnostics.push_back(ParseDiagnostic{
            "Tree-sitter reported syntax errors",
            to_source_location(path, ts_node_start_point(root)),
        });
        ts_tree_delete(tree);
        ts_parser_delete(parser);
        return result;
    }

    // 4. Return a successful syntax shell. Extraction comes later.
    result.success = true;

    ts_tree_delete(tree);
    ts_parser_delete(parser);
    return result;
}

} // namespace cjm::frontends::cxx::tree_sitter
