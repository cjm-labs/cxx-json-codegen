#include "frontends/cxx/tree_sitter/adapter.hpp"

#include <cstdint>
#include <cstring>
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

bool node_type_is(const TSNode& node, const char* type) {
    return std::strcmp(ts_node_type(node), type) == 0;
}

/**
 * Return the exact source text covered by a Tree-sitter node.
 *
 */
std::string node_text(const std::string& source, const TSNode& node) {
    const auto begin = ts_node_start_byte(node);
    const auto end = ts_node_end_byte(node);
    return source.substr(begin, end - begin);
}

/**
 * Collect namespace identifiers from a namespace name node.
 *
 * For example
 *  company::model
 *
 * becomes:
 *  ["company", "model"]
 */
void collect_namespace_parts(const TSNode& node, const std::string& source,
                             std::vector<std::string>& parts) {

    if (node_type_is(node, "namespace_identifier")) {
        parts.push_back(node_text(source, node));
        return;
    }

    const auto count = ts_node_named_child_count(node);
    for (uint32_t i = 0; i < count; ++i) {
        collect_namespace_parts(ts_node_named_child(node, i), source, parts);
    }
}

/**
 * Extract supported struct declarations into SourceFileSyntax.
 *
 * This only builds declarations shells. Field extraction is a later step.
 */
void extract_declarations(const std::string& path, const std::string& source,
                          const TSNode& node,
                          const std::vector<std::string>& namespace_path,
                          parser::SourceFileSyntax& file) {
    if (node_type_is(node, "namespace_definition")) {
        auto nested_namespace = namespace_path;

        const auto name_node = ts_node_child_by_field_name(node, "name", 4);
        if (!ts_node_is_null(name_node)) {
            collect_namespace_parts(name_node, source, nested_namespace);
        }

        const auto body_node = ts_node_child_by_field_name(node, "body", 4);
        if (!ts_node_is_null(body_node)) {
            extract_declarations(path, source, body_node, nested_namespace,
                                 file);
        }
        return;
    }

    if (node_type_is(node, "struct_specifier")) {
        const auto name_node = ts_node_child_by_field_name(node, "name", 4);
        if (!ts_node_is_null(name_node)) {
            parser::DeclarationSyntax declaration;
            declaration.name = node_text(source, name_node);
            declaration.namespace_path = namespace_path;
            declaration.location =
                to_source_location(path, ts_node_start_point(node));
            file.declarations.push_back(declaration);
        }
        return;
    }

    const auto count = ts_node_named_child_count(node);
    for (uint32_t i = 0; i < count; ++i) {
        extract_declarations(path, source, ts_node_named_child(node, i),
                             namespace_path, file);
    }
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

    // 4. Extract supported declaration shells.
    extract_declarations(path, source, root, {}, result.file);
    result.success = true;

    ts_tree_delete(tree);
    ts_parser_delete(parser);
    return result;
}

} // namespace cjm::frontends::cxx::tree_sitter
