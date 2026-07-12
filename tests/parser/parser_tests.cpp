#include "parser/parser.hpp"
#include <cassert>

int main() {
    {
        auto ok = cjm::parser::parse_source_file("examples/basic/user.hpp");
        assert(ok.success);
        assert(ok.file.path == "examples/basic/user.hpp");
        assert(ok.file.declarations.size() == 1);
        assert(ok.file.declarations[0].name == "User");
        assert(ok.file.declarations[0].fields.size() == 2);
        assert(ok.file.declarations[0].fields[0].name == "name");
        assert(ok.file.declarations[0].fields[0].type_spelling ==
               "std::string");
        assert(ok.file.declarations[0].fields[1].name == "age");
        assert(ok.file.declarations[0].fields[1].type_spelling == "int");

        assert(ok.file.declarations[0].fields[0].comments.size() == 1);
        assert(ok.file.declarations[0].fields[0].comments[0].text ==
               R"(json:"name")");
        assert(ok.file.declarations[0].fields[1].comments.size() == 1);
        assert(ok.file.declarations[0].fields[1].comments[0].text ==
               R"(json:"age")");

        auto missing =
            cjm::parser::parse_source_file("examples/basic/missing.hpp");
        assert(!missing.success);
        assert(missing.error.location.file == "examples/basic/missing.hpp");
    }

    return 0;
}
