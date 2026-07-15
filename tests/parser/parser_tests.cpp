#include "parser/parser.hpp"
#include <cassert>
#include <iostream>

namespace {

void print_namespace_path(const std::vector<std::string>& namespace_path) {
    if (namespace_path.empty()) {
        std::cerr << "<global>";
        return;
    }

    for (std::size_t i = 0; i < namespace_path.size(); ++i) {
        if (i > 0) {
            std::cerr << "::";
        }
        std::cerr << namespace_path[i];
    }
}

void dump_source_file(const cjm::parser::SourceFileSyntax& file) {
    std::cerr << "parsed file: " << file.path << "\n";
    std::cerr << " enums: " << file.enums.size() << "\n";
    for (const auto& enum_syntax : file.enums) {
        std::cerr << "    enum ";
        print_namespace_path(enum_syntax.namespace_path);
        std::cerr << "::" << enum_syntax.name << "\n";
    }

    std::cerr << "  type aliases: " << file.type_aliases.size() << "\n";
    for (const auto& alias : file.type_aliases) {
        std::cerr << "    using ";
        print_namespace_path(alias.namespace_path);
        std::cerr << "::" << alias.name << " = " << alias.target_type_spelling
                  << "\n";
    }

    std::cerr << "  declarations: " << file.declarations.size() << "\n";
    for (const auto& declaration : file.declarations) {
        std::cerr << "    struct ";
        print_namespace_path(declaration.namespace_path);
        std::cerr << "::" << declaration.name
                  << " fields=" << declaration.fields.size() << "\n";
        for (const auto& field : declaration.fields) {
            std::cerr << "    " << field.type_spelling << " " << field.name
                      << "\n";
        }
    }
}
} // namespace

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
    {
        auto ok = cjm::parser::parse_source_file(
            "tests/fixtures/parser_practical_models.hpp");
        assert(ok.success);

        assert(ok.file.enums.size() == 1);
        assert(ok.file.enums[0].name == "Status");
        assert(ok.file.enums[0].namespace_path.size() == 2);
        assert(ok.file.enums[0].namespace_path[0] == "company");
        assert(ok.file.enums[0].namespace_path[1] == "model");

        if (ok.file.type_aliases.size() != 1) {
            std::cerr << "expected exactly one type alias\n";
            dump_source_file(ok.file);
            return 1;
        }
        assert(ok.file.type_aliases[0].name == "UserId");
        assert(ok.file.type_aliases[0].target_type_spelling == "int");
        assert(ok.file.type_aliases[0].namespace_path.size() == 2);

        assert(ok.file.declarations.size() == 2);
        assert(ok.file.declarations[0].name == "Address");
        assert(ok.file.declarations[0].namespace_path.size() == 2);
        assert(ok.file.declarations[0].namespace_path[0] == "company");
        assert(ok.file.declarations[0].namespace_path[1] == "model");

        assert(ok.file.declarations[1].name == "User");
        assert(ok.file.declarations[1].namespace_path.size() == 2);
        assert(ok.file.declarations[1].fields.size() == 4);
        assert(ok.file.declarations[1].fields[0].type_spelling == "UserId");
        assert(ok.file.declarations[1].fields[1].type_spelling == "Address");
        assert(ok.file.declarations[1].fields[2].type_spelling ==
               "std::vector<std::string>");
        assert(ok.file.declarations[1].fields[3].type_spelling ==
               "std::optional<std::string>");
    }
    return 0;
}
