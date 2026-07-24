#include <cassert>
#include <string>

#include "frontends/cxx/tree_sitter/adapter.hpp"

int main() {
    {
        const std::string source = R"cpp(
namespace company::model {
struct User {
    std::string name;
};
}
)cpp";

        const auto result = cjm::frontends::cxx::tree_sitter::parse_source_text(
            "user.hpp", source);

        assert(result.success);
        assert(result.file.path == "user.hpp");
        assert(result.file.declarations.empty());
        assert(result.file.enums.empty());
        assert(result.file.type_aliases.empty());
        assert(result.diagnostics.empty());
    }
    {
        const std::string source = R"cpp(
struct User {
)cpp";
        const auto result = cjm::frontends::cxx::tree_sitter::parse_source_text(
            "broken.hpp", source);
        assert(!result.success);
        assert(!result.diagnostics.empty());
        assert(result.diagnostics[0].message.find("syntax errors") !=
               std::string::npos);
        assert(result.diagnostics[0].location.file == "broken.hpp");
    }

    return 0;
}
