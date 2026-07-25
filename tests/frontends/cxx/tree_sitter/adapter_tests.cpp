#include <cassert>
#include <string>

#include "frontends/cxx/tree_sitter/adapter.hpp"

int main() {
    {
        const std::string source = R"cpp(
namespace company::model {
enum class Status {
    Ok,
};

using Sequence = std::uint64_t;

struct User {
    Sequence id; // json:"id"
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
        assert(user.location.line == 9);
        assert(user.location.column == 1);

        assert(user.fields.size() == 2);

        const auto& name = user.fields[0];
        assert(name.name == "id");
        assert(name.type_spelling == "Sequence");
        assert(name.location.file == "user.hpp");
        assert(name.location.line == 10);
        assert(name.location.column == 5);
        assert(name.comments.size() == 1);
        assert(name.comments[0].text == "// json:\"id\"");
        assert(name.comments[0].location.file == "user.hpp");
        assert(name.comments[0].location.line == 10);
        assert(name.comments[0].location.column == 18);

        assert(result.file.enums.size() == 1);
        auto status = result.file.enums[0];
        assert(status.name == "Status");
        assert(status.namespace_path.size() == 2);
        assert(status.namespace_path[0] == "company");
        assert(status.namespace_path[1] == "model");
        assert(status.location.file == "user.hpp");
        assert(status.location.line == 3);
        assert(status.location.column == 1);

        assert(result.file.type_aliases.size() == 1);
        auto sequence = result.file.type_aliases[0];
        assert(sequence.name == "Sequence");
        assert(sequence.target_type_spelling == "std::uint64_t");
        assert(sequence.namespace_path.size() == 2);
        assert(sequence.namespace_path[0] == "company");
        assert(sequence.namespace_path[1] == "model");

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
