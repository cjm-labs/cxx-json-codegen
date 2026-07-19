#include "backends/nlohmann/cpp_generator.hpp"
#include "frontends/cxx/parser/parser.hpp"
#include "frontends/cxx/semantic/analysis.hpp"

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace {

std::string read_file(const std::string& path) {
    std::fstream file(path);
    assert(file.is_open());

    std::ostringstream contents;
    contents << file.rdbuf();
    return contents.str();
}

void write_file(const std::string& path, const std::string& contents) {
    std::ofstream file(path);
    assert(file.is_open());
    file << contents;
}

} // namespace

int main() {
    const auto parse_result =
        cjm::parser::parse_source_file("examples/basic/user.hpp");
    assert(parse_result.success);

    const auto analysis_result =
        cjm::semantic::analyze_source_file(parse_result.file);
    assert(analysis_result.success);

    const std::string expected_path =
        "tests/golden/basic_pipeline.expected.cjm.hpp";
    const std::string actual_path =
        "tests/golden/basic_pipeline.actual.cjm.hpp";

    const std::string generated =
        cjm::generator::generate_header(analysis_result.project);
    const std::string expected = read_file(expected_path);

    if (generated != expected) {
        write_file(actual_path, generated);
        std::cerr << "golden mismatch\n"
                  << "expected: " << expected_path << "\n"
                  << "actual: " << actual_path << "\n";
    }

    assert(generated == expected);

    return 0;
}
