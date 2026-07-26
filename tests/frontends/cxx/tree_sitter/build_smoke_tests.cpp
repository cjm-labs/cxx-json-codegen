#include <cassert>
#include <cstdint>
#include <cstring>
#include <string>

#include <tree_sitter/api.h>
#include "tree-sitter-cpp.h"

int main() {
    auto* parser = ts_parser_new();
    assert(parser != nullptr);

    // Check that the pinned runtime accepts the pinned C++ grammar.
    assert(ts_parser_set_language(parser, tree_sitter_cpp()));

    const std::string source = R"cpp(
namespace company::model {
struct User {
    std::string name;
};
}
)cpp";

    auto* tree =
        ts_parser_parse_string(parser, nullptr, source.c_str(),
                               static_cast<std::uint32_t>(source.size()));
    assert(tree != nullptr);

    const auto root = ts_tree_root_node(tree);
    assert(!ts_node_has_error(root));
    assert(std::strcmp(ts_node_type(root), "translation_unit") == 0);

    ts_tree_delete(tree);
    ts_parser_delete(parser);
}
