#include <fstream>
#include <sstream>

#include "parser/parser.hpp"

namespace cjm::parser {

ParseResult parse_source_file(const std::string& path) {
    std::ifstream input(path);

    if (!input.is_open()) {
        ParseResult result;
        result.success = false;
        result.error.message = "failed to open source file";
        result.error.location.file = path;
        return result;
    }

    std::ostringstream buffer;
    buffer << input.rdbuf();

    ParseResult result;
    result.success = true;
    result.file.path = path;
    return result;
}

} // namespace cjm::parser
