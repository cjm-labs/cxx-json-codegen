#include "parser/syntax.hpp"
#include "semantic/analysis.hpp"

#include <cassert>
#include <iostream>

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

    {
        cjm::parser::SourceFileSyntax file;
        file.path = "models.hpp";

        cjm::parser::EnumSyntax status;
        status.name = "Status";
        status.namespace_path = {"company", "model"};
        file.enums.push_back(status);

        cjm::parser::TypeAliasSyntax user_id;
        user_id.name = "UserId";
        user_id.target_type_spelling = "int";
        user_id.namespace_path = {"company", "model"};
        file.type_aliases.push_back(user_id);

        cjm::parser::DeclarationSyntax address;
        address.name = "Address";
        address.namespace_path = {"company", "model"};

        cjm::parser::FieldSyntax city;
        city.name = "city";
        city.type_spelling = "std::string";
        cjm::parser::CommentSyntax city_comment;
        city_comment.text = R"(json:"city")";
        city_comment.location.file = "models.hpp";
        city_comment.location.line = 10;
        city_comment.location.column = 23;
        city.comments.push_back(city_comment);
        address.fields.push_back(city);

        cjm::parser::DeclarationSyntax user;
        user.name = "User";
        user.namespace_path = {"company", "model"};

        cjm::parser::FieldSyntax id;
        id.name = "id";
        id.type_spelling = "UserId";
        cjm::parser::CommentSyntax id_comment;
        id_comment.text = R"(json:"id")";
        id_comment.location.file = "models.hpp";
        id_comment.location.line = 14;
        id_comment.location.column = 28;
        id.comments.push_back(id_comment);

        cjm::parser::FieldSyntax user_address;
        user_address.name = "address";
        user_address.type_spelling = "Address";
        cjm::parser::CommentSyntax address_comment;
        address_comment.text = R"(json:"address")";
        address_comment.location.file = "models.hpp";
        address_comment.location.line = 15;
        address_comment.location.column = 30;
        user_address.comments.push_back(address_comment);

        cjm::parser::FieldSyntax tags;
        tags.name = "tags";
        tags.type_spelling = "std::vector<std::string>";
        cjm::parser::CommentSyntax tags_comment;
        tags_comment.text = R"(json:"tags")";
        tags_comment.location.file = "models.hpp";
        tags_comment.location.line = 16;
        tags_comment.location.column = 40;
        tags.comments.push_back(tags_comment);

        cjm::parser::FieldSyntax nickname;
        nickname.name = "nickname";
        nickname.type_spelling = "std::optional<std::string>";
        cjm::parser::CommentSyntax nickname_comment;
        nickname_comment.text = R"(json:"nickname")";
        nickname_comment.location.file = "models.hpp";
        nickname_comment.location.line = 17;
        nickname_comment.location.column = 42;
        nickname.comments.push_back(nickname_comment);

        cjm::parser::FieldSyntax user_status;
        user_status.name = "status";
        user_status.type_spelling = "Status";
        cjm::parser::CommentSyntax status_comment;
        status_comment.text = R"(json:"status")";
        status_comment.location.file = "models.hpp";
        status_comment.location.line = 18;
        status_comment.location.column = 30;
        user_status.comments.push_back(status_comment);

        user.fields.push_back(id);
        user.fields.push_back(user_address);
        user.fields.push_back(tags);
        user.fields.push_back(nickname);
        user.fields.push_back(user_status);

        file.declarations.push_back(address);
        file.declarations.push_back(user);

        auto result = cjm::semantic::analyze_source_file(file);

        assert(result.success);
        assert(result.project.types.size() == 2);
        assert(result.project.types[0].qualified_name ==
               "company::model::Address");
        assert(result.project.types[1].qualified_name ==
               "company::model::User");

        const auto& fields = result.project.types[1].fields;
        assert(fields.size() == 5);
        assert(fields[0].type.kind ==
               cjm::metadata::FieldTypeKind::SignedInteger);
        assert(fields[1].type.kind ==
               cjm::metadata::FieldTypeKind::UserDefined);
        assert(fields[1].type.qualified_name == "company::model::Address");
        assert(fields[2].type.kind == cjm::metadata::FieldTypeKind::Vector);
        assert(fields[2].type.arguments[0].kind ==
               cjm::metadata::FieldTypeKind::String);
        assert(fields[3].type.kind == cjm::metadata::FieldTypeKind::Optional);
        assert(fields[3].type.arguments[0].kind ==
               cjm::metadata::FieldTypeKind::String);
        assert(fields[4].type.kind == cjm::metadata::FieldTypeKind::Enum);
        assert(fields[4].type.qualified_name == "company::model::Status");
    }
    {
        cjm::parser::SourceFileSyntax file;
        file.path = "unsupported.hpp";

        cjm::parser::DeclarationSyntax user;
        user.name = "User";

        cjm::parser::FieldSyntax callback;
        callback.name = "callback";
        callback.type_spelling = "std::function<void()>";
        callback.location.file = "unsupported.hpp";
        callback.location.line = 4;
        callback.location.column = 5;

        cjm::parser::CommentSyntax comment;
        comment.text = R"(json:"callback")";
        comment.location.file = "unsupported.hpp";
        comment.location.line = 4;
        comment.location.column = 34;
        callback.comments.push_back(comment);

        user.fields.push_back(callback);
        file.declarations.push_back(user);

        auto result = cjm::semantic::analyze_source_file(file);

        assert(!result.success);
        assert(result.project.types.empty());
        assert(result.diagnostics.size() == 1);
        assert(
            result.diagnostics[0].message ==
            "unsupported field type for JSON mapping: std::function<void()>");
    }
    {
        // Bare string is not accepted until parser syntax records using
        // declarations such as `using std::string`.
        cjm::parser::FieldSyntax field_name;
        field_name.name = "name";
        field_name.type_spelling = "string";
        field_name.comments = {{R"(json:"name")"}};

        cjm::parser::DeclarationSyntax user;
        user.name = "User";
        user.fields = {field_name};

        cjm::parser::SourceFileSyntax file;
        file.declarations = {user};
        auto result = cjm::semantic::analyze_source_file(file);
        assert(!result.success);
        assert(result.project.types.empty());
        assert(result.diagnostics.size() == 1);
        std::cerr << "diagnostics[0].message = "
                  << result.diagnostics[0].message << "\n";
        assert(result.diagnostics[0].message.find(
                   "unsupported field type for JSON mapping") !=
               std::string::npos);
    }
    {
        cjm::parser::SourceFileSyntax file;
        file.path = "aliases.hpp";

        cjm::parser::TypeAliasSyntax alias_userId;
        alias_userId.name = "UserId";
        alias_userId.target_type_spelling = "int";
        alias_userId.namespace_path = {"company", "model"};

        cjm::parser::TypeAliasSyntax alias_Id;
        alias_Id.name = "Id";
        alias_Id.target_type_spelling = "UserId";
        alias_Id.namespace_path = {"company", "model"};

        file.type_aliases = {alias_userId, alias_Id};

        cjm::parser::DeclarationSyntax user;
        user.name = "User";
        user.namespace_path = {"company", "model"};

        cjm::parser::FieldSyntax fieldId;
        fieldId.name = "id";
        fieldId.type_spelling = "Id";
        fieldId.comments.push_back({R"(json:"id")"});

        user.fields.push_back(fieldId);

        file.declarations.push_back(user);

        auto result = cjm::semantic::analyze_source_file(file);

        assert(result.success);
        assert(result.project.types.size() == 1);
        assert(result.project.types[0].fields.size() == 1);
        assert(result.project.types[0].fields[0].type.kind ==
               cjm::metadata::FieldTypeKind::SignedInteger);
        assert(result.project.types[0].qualified_name ==
               "company::model::User");
        assert(result.project.types[0].fields[0].type.qualified_name == "int");
    }
    {
        cjm::parser::SourceFileSyntax file;
        file.path = "cyclic_aliases.hpp";

        cjm::parser::TypeAliasSyntax alias_A;
        alias_A.name = "A";
        alias_A.target_type_spelling = "B";
        alias_A.namespace_path = {"company", "model"};

        cjm::parser::TypeAliasSyntax alias_B;
        alias_B.name = "B";
        alias_B.target_type_spelling = "A";
        alias_B.namespace_path = {"company", "model"};

        file.type_aliases = {alias_A, alias_B};

        cjm::parser::DeclarationSyntax user;
        user.name = "User";
        user.namespace_path = {"company", "model"};

        cjm::parser::FieldSyntax field_id;
        field_id.name = "id";
        field_id.type_spelling = "A";
        field_id.comments.push_back({R"(json:"id")"});

        user.fields.push_back(field_id);
        file.declarations = {user};

        auto result = cjm::semantic::analyze_source_file(file);

        assert(result.success == false);
        assert(result.project.types.empty());
        assert(result.diagnostics.size() == 1);
        assert(result.diagnostics[0].message.find(
                   "cyclic type alias detected") != std::string::npos);
    }
    {
        cjm::parser::SourceFileSyntax file;

        cjm::parser::FieldSyntax field_city;
        field_city.name = "city";
        field_city.type_spelling = "std::string";
        field_city.comments.push_back({R"(json:"city")"});

        cjm::parser::DeclarationSyntax address;
        address.name = "Address";
        address.namespace_path = {"company", "model"};
        address.fields = {field_city};

        cjm::parser::FieldSyntax field_address;
        field_address.name = "address";
        field_address.type_spelling = "Address";
        field_address.comments.push_back({R"(json:"address")"});

        cjm::parser::DeclarationSyntax user;
        user.name = "User";
        user.namespace_path = {"company", "model", "detail"};
        user.fields = {field_address};

        file.declarations = {address, user};

        auto result = cjm::semantic::analyze_source_file(file);
        assert(result.success);
        assert(result.project.types.size() == 2); // Address, User
        assert(result.project.types[1].fields.size() == 1);
        assert(result.project.types[1].fields[0].type.kind ==
               cjm::metadata::FieldTypeKind::UserDefined);
        assert(result.project.types[1].fields[0].type.qualified_name ==
               "company::model::Address");
    }
}
