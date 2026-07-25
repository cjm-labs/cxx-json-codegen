#include <cassert>
#include <string>

#include "frontends/cxx/tree_sitter/adapter.hpp"

int main() {
    {
        const std::string source = R"cpp(
namespace company::model {
struct User {
    std::string name; // json:"name"
};
}
)cpp";

        const auto result = cjm::frontends::cxx::tree_sitter::parse_source_text(
            "user.hpp", source);

        assert(result.success);
        assert(result.file.path == "user.hpp");

        assert(result.file.declarations.size() == 1);

        const auto& user = result.file.declarations[0];
        assert(user.name == "User");
        assert(user.namespace_path.size() == 2);
        assert(user.namespace_path[0] == "company");
        assert(user.namespace_path[1] == "model");
        assert(user.location.file == "user.hpp");
        assert(user.location.line == 3);
        assert(user.location.column == 1);

        assert(user.fields.size() == 1);

        const auto& name = user.fields[0];
        assert(name.name == "name");
        assert(name.type_spelling == "std::string");
        assert(name.location.file == "user.hpp");
        assert(name.location.line == 4);
        assert(name.location.column == 5);
        assert(name.comments.size() == 1);
        assert(name.comments[0].text == "// json:\"name\"");
        assert(name.comments[0].location.file == "user.hpp");
        assert(name.comments[0].location.line == 4);
        assert(name.comments[0].location.column == 23);

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
