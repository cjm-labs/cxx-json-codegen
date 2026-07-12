#pragma once

#include <string>
#include <vector>

namespace cjm::parser {

struct SourceLocation {
    std::string file;
    int line = 0;
    int column = 0;
};

struct CommentSyntax {
    std::string text;
    SourceLocation location;
};

struct FieldSyntax {
    std::string name;
    std::string type_spelling;
    SourceLocation location;
    std::vector<CommentSyntax> comments;
};

struct DeclarationSyntax {
    std::string name;
    SourceLocation location;
    std::vector<FieldSyntax> fields;
    std::vector<CommentSyntax> comments;
};

struct SourceFileSyntax {
    std::string path;
    std::vector<DeclarationSyntax> declarations;
    std::vector<CommentSyntax> comments;
};
} // namespace cjm::parser
