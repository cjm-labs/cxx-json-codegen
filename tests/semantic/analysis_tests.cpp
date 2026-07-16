#include "parser/syntax.hpp"
#include "semantic/analysis.hpp"

#include <cassert>

int main() {
    {

        cjm::semantic::AnalysisResult result;
        result.success = false;

        cjm::semantic::Diagnostic diagnostic;
        diagnostic.location.file = "user.hpp";
        diagnostic.location.line = 4;
        diagnostic.location.column = 17;
        diagnostic.message = "invalid CJM metadata";

        result.diagnostics.push_back(diagnostic);

        assert(!result.success);
        assert(result.diagnostics.size() == 1);
        assert(result.diagnostics[0].location.file == "user.hpp");
        assert(result.diagnostics[0].location.line == 4);
        assert(result.diagnostics[0].location.column == 17);
        assert(result.diagnostics[0].message == "invalid CJM metadata");
    }
    {
        cjm::semantic::SourceLocation location;
        location.file = "user.hpp";
        location.line = 4;
        location.column = 30;

        auto valid = cjm::semantic::parse_json_field_metadata(R"(json:"name")",
                                                              location);

        assert(valid.found);
        assert(valid.success);
        assert(valid.json_name == "name");
        assert(!valid.omit_empty);
        assert(!valid.ignored);
        assert(valid.diagnostic.message.empty());

        auto ordinary = cjm::semantic::parse_json_field_metadata(
            "ordinary comment", location);
        assert(!ordinary.found);
        assert(!ordinary.success);

        auto invalid =
            cjm::semantic::parse_json_field_metadata("json:name", location);
        assert(invalid.found);
        assert(!invalid.success);
        assert(invalid.diagnostic.location.file == "user.hpp");
        assert(!invalid.diagnostic.message.empty());

        auto omit = cjm::semantic::parse_json_field_metadata(
            R"(json:"nickname,omitempty")", location);
        assert(omit.found);
        assert(omit.success);
        assert(omit.json_name == "nickname");
        assert(omit.omit_empty);
        assert(!omit.ignored);

        auto ignored =
            cjm::semantic::parse_json_field_metadata(R"(json:"-")", location);
        assert(ignored.found);
        assert(ignored.success);
        assert(ignored.json_name == "-");
        assert(ignored.ignored);
        assert(!ignored.omit_empty);

        auto unsupported = cjm::semantic::parse_json_field_metadata(
            R"(json:"name,unknown")", location);
        assert(unsupported.found);
        assert(!unsupported.success);
        assert(unsupported.diagnostic.location.file == "user.hpp");
        assert(unsupported.diagnostic.message ==
               "unsupported CJM json metadata option: unknown");
    }
    {
        cjm::parser::SourceFileSyntax file;
        file.path = "user.hpp";

        cjm::parser::DeclarationSyntax user;
        user.name = "User";

        cjm::parser::FieldSyntax name;
        name.name = "name";
        name.type_spelling = "std::string";

        cjm::parser::CommentSyntax name_comment;
        name_comment.text = R"(json:"name")";
        name_comment.location.file = "user.hpp";
        name_comment.location.line = 4;
        name_comment.location.column = 30;
        name.comments.push_back(name_comment);

        user.fields.push_back(name);
        file.declarations.push_back(user);

        auto result = cjm::semantic::analyze_source_file(file);

        assert(result.success);
        assert(result.project.types.size() == 1);
        assert(result.project.types[0].name == "User");
        assert(result.project.types[0].fields.size() == 1);
        assert(result.project.types[0].fields[0].name == "name");
        assert(result.project.types[0].fields[0].type.spelling ==
               "std::string");
        assert(result.project.types[0].fields[0].json.name == "name");
    }
    {
        cjm::parser::SourceFileSyntax file;
        file.path = "user.hpp";

        cjm::parser::DeclarationSyntax user;
        user.name = "User";

        cjm::parser::FieldSyntax name;
        name.name = "name";
        name.type_spelling = "std::string";

        cjm::parser::CommentSyntax name_comment;
        name_comment.text = R"(json:"name")";
        name_comment.location.file = "user.hpp";
        name_comment.location.line = 4;
        name_comment.location.column = 30;
        name.comments.push_back(name_comment);

        cjm::parser::FieldSyntax password;
        password.name = "password";
        password.type_spelling = "std::string";

        cjm::parser::CommentSyntax ignored_comment;
        ignored_comment.text = R"(json:"-")";
        ignored_comment.location.file = "user.hpp";
        ignored_comment.location.line = 5;
        ignored_comment.location.column = 34;
        password.comments.push_back(ignored_comment);

        cjm::parser::FieldSyntax nickname;
        nickname.name = "nickname";
        nickname.type_spelling = "std::string";

        cjm::parser::CommentSyntax nickname_comment;
        nickname_comment.text = R"(json:"nickname,omitempty")";
        nickname_comment.location.file = "user.hpp";
        nickname_comment.location.line = 6;
        nickname_comment.location.column = 34;
        nickname.comments.push_back(nickname_comment);

        user.fields.push_back(name);
        user.fields.push_back(password);
        user.fields.push_back(nickname);
        file.declarations.push_back(user);

        auto result = cjm::semantic::analyze_source_file(file);

        assert(result.success);
        assert(result.project.types.size() == 1);
        assert(result.project.types[0].fields.size() == 2);
        assert(result.project.types[0].fields[0].name == "name");
        assert(result.project.types[0].fields[1].name == "nickname");
        assert(result.project.types[0].fields[1].json.name == "nickname");
        assert(result.project.types[0].fields[1].json.omit_empty);
    }

    {
        cjm::parser::SourceFileSyntax file;
        file.path = "user.hpp";

        cjm::parser::DeclarationSyntax user;
        user.name = "User";

        cjm::parser::FieldSyntax first_name;
        first_name.name = "first_name";
        first_name.type_spelling = "std::string";

        cjm::parser::CommentSyntax first_comment;
        first_comment.text = R"(json:"name")";
        first_comment.location.file = "user.hpp";
        first_comment.location.line = 4;
        first_comment.location.column = 31;
        first_name.comments.push_back(first_comment);

        cjm::parser::FieldSyntax last_name;
        last_name.name = "last_name";
        last_name.type_spelling = "std::string";

        cjm::parser::CommentSyntax second_comment;
        second_comment.text = R"(json:"name")";
        second_comment.location.file = "user.hpp";
        second_comment.location.line = 5;
        second_comment.location.column = 31;
        last_name.comments.push_back(second_comment);

        user.fields.push_back(first_name);
        user.fields.push_back(last_name);
        file.declarations.push_back(user);

        auto result = cjm::semantic::analyze_source_file(file);

        assert(!result.success);
        assert(result.diagnostics.size() == 1);
        assert(result.diagnostics[0].location.line == 5);
        assert(result.diagnostics[0].message ==
               "duplicate JSON field name: name");
    }
    {
        cjm::parser::SourceFileSyntax file;
        file.path = "user.hpp";

        cjm::parser::DeclarationSyntax user;
        user.name = "User";

        cjm::parser::FieldSyntax name;
        name.name = "name";
        name.type_spelling = "std::string";

        cjm::parser::CommentSyntax invalid_comment;
        invalid_comment.text = "json:name";
        invalid_comment.location.file = "user.hpp";
        invalid_comment.location.line = 4;
        invalid_comment.location.column = 30;
        name.comments.push_back(invalid_comment);

        user.fields.push_back(name);
        file.declarations.push_back(user);

        auto result = cjm::semantic::analyze_source_file(file);

        assert(!result.success);
        assert(result.project.types.empty());
        assert(result.diagnostics.size() == 1);
        assert(result.diagnostics[0].location.file == "user.hpp");
        assert(result.diagnostics[0].location.line == 4);
        assert(!result.diagnostics[0].message.empty());
    }
    return 0;
}
