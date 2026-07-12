#pragma once

#include "parser/syntax.hpp"

#include <string>

namespace cjm::parser {

struct ParserError {
    std::string message;
    SourceLocation location;
};

struct ParseResult {
    SourceFileSyntax file;
    ParserError error;
    bool success = false;
};

ParseResult parse_source_file(const std::string& path);

} // namespace cjm::parser
