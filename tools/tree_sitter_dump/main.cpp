#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <tree_sitter/api.h>
#include "tree-sitter-cpp.h"

namespace {

std::string read_file(const std::string& path) {
    std::ifstream input(path);
    if (!input) {
        return {};
    }

    std::ostringstream buffer;
    buffer << input.rdbuf();
    return buffer.str();
}

std::string node_text(const std::string& source, const TSNode& node) {
    const auto begin = ts_node_start_byte(node);
    const auto end = ts_node_end_byte(node);
    return source.substr(begin, end - begin);
}

void print_indent(int depth) {
    for (int i = 0; i < depth; ++i) {
        std::cout << "  ";
    }
}

void dump_tree(const std::string& source, const TSNode node, int depth = 0) {
    print_indent(depth);

    const auto start = ts_node_start_point(node);
    const auto end = ts_node_end_point(node);

    std::cout << ts_node_type(node) << " [" << start.row + 1 << ":"
              << start.column + 1 << "-" << end.row + 1 << ":" << end.column + 1
              << "]";

    if (ts_node_is_named(node)) {
        std::cout << " named";
    }
    if (ts_node_has_error(node)) {
        std::cout << " has error";
    }
    if (ts_node_is_error(node)) {
        std::cout << " ERROR";
    }
    if (ts_node_is_missing(node)) {
        std::cout << " MISSING";
    }

    const auto text = node_text(source, node);
    if (text.size() <= 60 && text.find('\n') == std::string::npos) {
        std::cout << " text=\"" << text << "\"";
    }
    std::cout << "\n";

    const auto child_count = ts_node_child_count(node);
    for (uint32_t i = 0; i < child_count; ++i) {
        dump_tree(source, ts_node_child(node, i), depth + 1);
    }
}

} // namespace

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "usage: cjm_tree_sitter_dump <source-file>\n";
        return 1;
    }

    const std::string path = argv[1];
    const auto source = read_file(path);
    if (source.empty()) {
        std::cerr << "failed to read source file: " << path << "\n";
        return 1;
    }

    auto* parser = ts_parser_new();
    if (parser == nullptr) {
        std::cerr << "failed to create Tree-sitter parser\n";
        return 1;
    }
    if (!ts_parser_set_language(parser, tree_sitter_cpp())) {
        std::cerr << "failed to attach tree-sitter-cpp grammar\n";
        ts_parser_delete(parser);
        return 1;
    }
    auto* tree =
        ts_parser_parse_string(parser, nullptr, source.c_str(),
                               static_cast<std::uint32_t>(source.size()));
    if (tree == nullptr) {
        std::cerr << "failed to parse source file\n";
        ts_parser_delete(parser);
        return 1;
    }

    dump_tree(source, ts_tree_root_node(tree));

    ts_tree_delete(tree);
    ts_parser_delete(parser);
    return 0;
}
