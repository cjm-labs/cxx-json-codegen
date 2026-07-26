#include "frontends/cxx/tree_sitter/adapter.hpp"

#include <cctype>
#include <cstdint>
#include <cstring>
#include <tree_sitter/api.h>
#include "tree-sitter-cpp.h"

namespace cjm::frontends::cxx::tree_sitter {
namespace {

/**
 * Trim whitespace around a source spelling slice.
 */
std::string trim_copy(std::string text) {
    auto begin = text.begin();
    while (begin != text.end() &&
           std::isspace(static_cast<unsigned char>(*begin))) {
        ++begin;
    }

    auto end = text.end();
    while (end != begin &&
           std::isspace(static_cast<unsigned char>(*(end - 1)))) {
        --end;
    }

    return std::string(begin, end);
}

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
 * Count direct named children with the requested Tree-sitter node type.
 *
 * This is used by fail-closed checks where accepting only the first matching
 * child would silently drop unsupported source syntax.
 */
uint32_t count_direct_named_children_of_type(const TSNode& node,
                                             const char* type) {
    uint32_t count = 0;
    const auto child_count = ts_node_named_child_count(node);
    for (uint32_t i = 0; i < child_count; ++i) {
        const auto child = ts_node_named_child(node, i);
        if (node_type_is(child, type)) {
            ++count;
        }
    }
    return count;
}

/**
 * Find the first named child with the requested Tree-sitter node type.
 */
TSNode find_first_named_child_of_type(const TSNode& node, const char* type) {
    const auto count = ts_node_named_child_count(node);
    for (uint32_t i = 0; i < count; ++i) {
        const auto child = ts_node_named_child(node, i);
        if (node_type_is(child, type)) {
            return child;
        }
    }
    return TSNode{};
}

/**
 * Return true when a node has a direct named child of the requested type.
 *
 * This is used for small fail-closed checks on managed syntax nodes.
 */
bool has_direct_named_child_of_type(const TSNode& node, const char* type) {
    const auto& typed_node = find_first_named_child_of_type(node, type);
    return !ts_node_is_null(typed_node);
}

TSNode next_named_sibling(const TSNode& node) {
    auto sibling = ts_node_next_sibling(node);
    while (!ts_node_is_null(sibling) && !ts_node_is_named(sibling)) {
        sibling = ts_node_next_sibling(sibling);
    }
    return sibling;
}

/**
 * Return true for function or function-pointer field declaration shapes.
 *
 * CJM does not support these as JSON fields, so the parser rejects them before
 * extracting a partial FieldSyntax.
 */
bool has_function_declarator(const TSNode& field_node) {
    return has_direct_named_child_of_type(field_node, "function_declarator");
}

/**
 * Return true when an unsupported declaration has a same-line json comment.
 *
 * This marks declarations managed by CJM metadata but not represented as
 * ordinary field_declaration nodes.
 */
bool has_same_line_json_comment(const TSNode& node, const std::string& source) {
    const auto sibling = next_named_sibling(node);
    if (ts_node_is_null(sibling) || !node_type_is(sibling, "comment")) {
        return false;
    }
    return ts_node_end_point(node).row == ts_node_start_point(sibling).row &&
           node_text(source, sibling).find("json:") != std::string::npos;
}

/**
 * Convert one supported Tree-sitter field_declaration into FieldSyntax.
 */
bool extract_field(const std::string& path, const std::string& source,
                   const TSNode& field_node, parser::FieldSyntax& field,
                   std::vector<ParseDiagnostic>& diagnostics) {
    if (has_direct_named_child_of_type(field_node, "storage_class_specifier")) {
        ParseDiagnostic diagnostic;
        diagnostic.message = "unsupported field declaration: static data "
                             "members are not supported";
        diagnostic.location =
            to_source_location(path, ts_node_start_point(field_node));
        diagnostics.push_back(diagnostic);
        return false;
    }

    if (has_function_declarator(field_node)) {
        ParseDiagnostic diagnostic;
        diagnostic.message = "unsupported field declaration: function "
                             "declarators are not supported";
        diagnostic.location =
            to_source_location(path, ts_node_start_point(field_node));
        diagnostics.push_back(diagnostic);
        return false;
    }

    const auto field_name_count =
        count_direct_named_children_of_type(field_node, "field_identifier");
    if (field_name_count > 1) {
        ParseDiagnostic diagnostic;
        diagnostic.message = "unsupported field declaration: multiple field "
                             "declarators are not supported";
        diagnostic.location =
            to_source_location(path, ts_node_start_point(field_node));
        diagnostics.push_back(diagnostic);
        return false;
    }

    const auto name_node =
        find_first_named_child_of_type(field_node, "field_identifier");
    if (ts_node_is_null(name_node)) {
        return false;
    }

    const auto type_begin = ts_node_start_byte(field_node);
    const auto type_end = ts_node_start_byte(name_node);
    field.type_spelling =
        trim_copy(source.substr(type_begin, type_end - type_begin));
    field.name = node_text(source, name_node);
    field.location = to_source_location(path, ts_node_start_point(field_node));
    return true;
}

/**
 * Convert one Tree-sitter comment node into CJM CommentSyntax.
 *
 * This function preserves the original comment text. Metadata validation
 * belongs to Semantic Analysis, not the frontend adapter.
 */
parser::CommentSyntax extract_comment(const std::string& path,
                                      const std::string& source,
                                      const TSNode& comment_node) {
    parser::CommentSyntax comment;
    comment.text = node_text(source, comment_node);
    comment.location =
        to_source_location(path, ts_node_start_point(comment_node));
    return comment;
}

/**
 * Return true when `comment_node` is a trailing same-line comment for
 * `field_node`.
 *
 * This function only checks source positions. It does not parse the comment
 * text and does not decide whether the comment contains CJM metadata.
 */
bool is_same_line_trailing_comment(const TSNode& field_node,
                                   const TSNode& comment_node) {
    const auto field_end = ts_node_end_point(field_node);
    const auto comment_start = ts_node_start_point(comment_node);

    return field_end.row == comment_start.row &&
           comment_start.column > field_end.column;
}

/**
 * Attach the next same-line comment to a field when the Tree-sitter sibling
 * sequence is:
 *
 *     field_declaration
 *     comment
 *
 * This keeps comment binding local to the struct body traversal.
 */
void bind_trailing_comment_if_present(const std::string& path,
                                      const std::string& source,
                                      const TSNode& field_node,
                                      const TSNode& maybe_comment_node,
                                      parser::FieldSyntax& field) {
    if (ts_node_is_null(maybe_comment_node)) {
        return;
    }

    if (!node_type_is(maybe_comment_node, "comment")) {
        return;
    }
    if (!is_same_line_trailing_comment(field_node, maybe_comment_node)) {
        return;
    }
    field.comments.push_back(extract_comment(path, source, maybe_comment_node));
}

/**
 * Extract supported ordinary fields from a struct body.
 */
void extract_fields(const std::string& path, const std::string& source,
                    const TSNode& struct_node,
                    parser::DeclarationSyntax& declaration,
                    std::vector<ParseDiagnostic>& diagnostics) {
    const auto body_node = ts_node_child_by_field_name(struct_node, "body", 4);
    if (ts_node_is_null(body_node)) {
        return;
    }

    const auto count = ts_node_named_child_count(body_node);
    for (uint32_t i = 0; i < count; ++i) {
        const auto child = ts_node_named_child(body_node, i);
        if (node_type_is(child, "declaration") &&
            has_same_line_json_comment(child, source)) {
            ParseDiagnostic diagnostic;
            diagnostic.message =
                "unsupported field declaration: managed declarations "
                "must use ordinary field syntax.";
            diagnostic.location =
                to_source_location(path, ts_node_start_point(child));
            diagnostics.push_back(diagnostic);
            continue;
        }

        if (!node_type_is(child, "field_declaration")) {
            continue;
        }

        parser::FieldSyntax field;
        if (!extract_field(path, source, child, field, diagnostics)) {
            continue;
        }
        if ((i + 1) < count) {
            const auto next_child = ts_node_named_child(body_node, i + 1);
            bind_trailing_comment_if_present(path, source, child, next_child,
                                             field);
        }
        declaration.fields.push_back(field);
    }
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
 * Convert one supported Tree-sitter enum_specifier into EnumSyntax.
 *
 * This records only the enum name, namespace, and source location.
 * Enumerator values are not part of the current parser syntax contract.
 */
bool extract_enum(const std::string& path, const std::string& source,
                  const TSNode& enum_node,
                  const std::vector<std::string>& namespace_path,
                  parser::EnumSyntax& out) {
    const auto name_node = ts_node_child_by_field_name(enum_node, "name", 4);
    if (ts_node_is_null(name_node)) {
        return false;
    }

    out.name = node_text(source, name_node);
    out.namespace_path = namespace_path;
    out.location = to_source_location(path, ts_node_start_point(enum_node));
    return true;
}

/**
 * Convert one supported Tree-sitter alias_declaration into TypeAliasSyntax.
 *
 * This preserves the target type spelling exactly as source text.
 * Alias resolution belongs to Semantic Analysis.
 */
bool extract_alias(const std::string& path, const std::string& source,
                   const TSNode& alias_node,
                   const std::vector<std::string>& namespace_path,
                   parser::TypeAliasSyntax& out) {
    const auto name_node = ts_node_child_by_field_name(alias_node, "name", 4);
    const auto type_node = ts_node_child_by_field_name(alias_node, "type", 4);
    if (ts_node_is_null(name_node) || ts_node_is_null(type_node)) {
        return false;
    }

    out.name = node_text(source, name_node);
    out.target_type_spelling = trim_copy(node_text(source, type_node));
    out.namespace_path = namespace_path;
    out.location = to_source_location(path, ts_node_start_point(alias_node));
    return true;
}

/**
 * Extract supported struct declarations into SourceFileSyntax.
 *
 * This only builds declarations shells. Field extraction is a later step.
 */
void extract_declarations(const std::string& path, const std::string& source,
                          const TSNode& node,
                          const std::vector<std::string>& namespace_path,
                          parser::SourceFileSyntax& file,
                          std::vector<ParseDiagnostic>& diagnostics) {
    if (node_type_is(node, "namespace_definition")) {
        auto nested_namespace = namespace_path;

        const auto name_node = ts_node_child_by_field_name(node, "name", 4);
        if (!ts_node_is_null(name_node)) {
            collect_namespace_parts(name_node, source, nested_namespace);
        }

        const auto body_node = ts_node_child_by_field_name(node, "body", 4);
        if (!ts_node_is_null(body_node)) {
            extract_declarations(path, source, body_node, nested_namespace,
                                 file, diagnostics);
        }
        return;
    }

    if (node_type_is(node, "enum_specifier")) {
        parser::EnumSyntax enum_syntax;
        if (extract_enum(path, source, node, namespace_path, enum_syntax)) {
            file.enums.push_back(enum_syntax);
        }
        return;
    }

    if (node_type_is(node, "alias_declaration")) {
        parser::TypeAliasSyntax alias_syntax;
        if (extract_alias(path, source, node, namespace_path, alias_syntax)) {
            file.type_aliases.push_back(alias_syntax);
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
            extract_fields(path, source, node, declaration, diagnostics);
            file.declarations.push_back(declaration);
        }
        return;
    }

    const auto count = ts_node_named_child_count(node);
    for (uint32_t i = 0; i < count; ++i) {
        extract_declarations(path, source, ts_node_named_child(node, i),
                             namespace_path, file, diagnostics);
    }
}

/**
 * Find the first concrete Tree-sitter syntax error marker under `node`.
 *
 * `ts_node_has_error(node)` tells us a subtree contains an error. This
 * helper locates the first ERROR or MISSING node so diagnostics can point
 * closer to the malformed source.
 */
TSNode find_first_error_node(const TSNode& node) {
    if (ts_node_is_error(node)) {
        return node;
    }
    if (ts_node_is_missing(node)) {
        return node;
    }

    const auto count = ts_node_named_child_count(node);
    for (uint32_t i = 0; i < count; ++i) {
        const auto child = ts_node_named_child(node, i);
        if (!ts_node_has_error(child)) {
            continue;
        }
        const auto found = find_first_error_node(child);
        if (!ts_node_is_null(found)) {
            return found;
        }
    }
    return TSNode{};
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
        const auto error_node = find_first_error_node(root);
        auto location_node = root;
        if (!ts_node_is_null(error_node)) {
            location_node = error_node;
        }
        result.diagnostics.push_back(ParseDiagnostic{
            "Tree-sitter reported syntax errors",
            to_source_location(path, ts_node_start_point(location_node)),
        });
        ts_tree_delete(tree);
        ts_parser_delete(parser);
        return result;
    }

    // 4. Extract supported declaration shells.
    extract_declarations(path, source, root, {}, result.file,
                         result.diagnostics);

    if (result.diagnostics.empty()) {
        result.success = true;
    } else {
        result.success = false;
    }

    ts_tree_delete(tree);
    ts_parser_delete(parser);
    return result;
}

} // namespace cjm::frontends::cxx::tree_sitter
