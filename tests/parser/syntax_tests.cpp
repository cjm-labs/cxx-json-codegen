#include "parser/parser.hpp"
#include "parser/syntax.hpp"

#include <cassert>
#include <string>

int main() {
    {

        cjm::parser::SourceFileSyntax file;
        file.path = "user.hpp";

        cjm::parser::DeclarationSyntax user;
        user.name = "User";
        user.location.file = "user.hpp";
        user.location.line = 3;
        user.location.column = 8;

        cjm::parser::FieldSyntax name;
        name.name = "name";
        name.type_spelling = "std::string";
        name.location.file = "user.hpp";
        name.location.line = 4;
        name.location.column = 17;

        cjm::parser::CommentSyntax comment;
        comment.text = R"(json:"name")";
        comment.location.file = "user.hpp";
        comment.location.line = 4;
        comment.location.column = 30;

        name.comments.push_back(comment);
        user.fields.push_back(name);
        file.declarations.push_back(user);

        assert(file.path == "user.hpp");
        assert(file.declarations.size() == 1);
        assert(file.declarations[0].name == "User");
        assert(file.declarations[0].fields.size() == 1);
        assert(file.declarations[0].fields[0].type_spelling == "std::string");
        assert(file.declarations[0].fields[0].comments[0].text ==
               R"(json:"name")");
        assert(file.declarations[0].fields[0].location.line == 4);
    }

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

        auto missing =
            cjm::parser::parse_source_file("examples/basic/missing.hpp");
        assert(!missing.success);
        assert(missing.error.location.file == "examples/basic/missing.hpp");
    }
    return 0;
}
