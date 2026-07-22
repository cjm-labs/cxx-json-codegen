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

// Run parser -> semantic -> generator and compile with a golden file.
void assert_pipeline_matches(const std::string& input_path,
                             const std::string& expected_path,
                             const std::string& actual_path) {
    // 1. Parse the C++ source fixture.
    const auto parse_result = cjm::parser::parse_source_file(input_path);
    assert(parse_result.success);

    // 2. Convert parser syntax into Metadata IR.
    const auto analysis_result =
        cjm::semantic::analyze_source_file(parse_result.file);
    assert(analysis_result.success);

    // 3. Generate backend output and compare it with the golden file.
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
}

} // namespace

int main() {
    assert_pipeline_matches("examples/basic/user.hpp",
                            "tests/golden/basic_pipeline.expected.cjm.hpp",
                            "tests/golden/basic_pipeline.actual.cjm.hpp");

    assert_pipeline_matches("tests/fixtures/pipeline_map_user.hpp",
                            "tests/golden/map_user.expected.cjm.hpp",
                            "tests/golden/pipeline_map_user.actual.cjm.hpp");
    return 0;
}
