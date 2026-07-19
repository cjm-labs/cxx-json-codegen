#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "backends/nlohmann/cpp_generator.hpp"
#include "frontends/cxx/parser/parser.hpp"
#include "frontends/cxx/semantic/analysis.hpp"

namespace {

constexpr int kExitSuccess = 0;
constexpr int kExitFailure = 1;
constexpr int kExitUsageError = 2;

void print_help(std::ostream& out) {
    out << "cjm - C++ JSON metadata code generator\n"
        << "\n"
        << "Usage:\n"
        << "    cjm --help\n"
        << "    cjm help\n"
        << "    cjm generate --input <header> --output <file>\n";
}

struct GenerateOptions {
    std::vector<std::string> inputs;
    std::string output;
};

bool parse_generate_options(int argc, char** argv, GenerateOptions& options) {
    for (int i = 2; i < argc; ++i) {
        const std::string arg = argv[i];

        if (arg == "--input") {
            if (i + 1 >= argc) {
                std::cerr << "cjm: --input requires a value\n";
                return false;
            }

            options.inputs.push_back(argv[++i]);
            continue;
        }
        if (arg == "--output") {
            if (i + 1 >= argc) {
                std::cerr << "cjm: --output requires a value\n";
                return false;
            }
            options.output = argv[++i];
            continue;
        }
        std::cerr << "cjm: unknown generate option: " << arg << "\n";
        return false;
    }

    if (options.inputs.empty()) {
        std::cerr << "cjm: generate requires --input <header>\n";
        return false;
    }
    if (options.output.empty()) {
        std::cerr << "cjm: generate requires --output <file>\n";
        return false;
    }
    return true;
}

} // namespace

int main(int argc, char** argv) {
    if (argc <= 1) {
        print_help(std::cout);
        return kExitSuccess;
    }

    const std::string command = argv[1];

    if (command == "--help" || command == "-h" || command == "help") {
        print_help(std::cout);
        return kExitSuccess;
    }

    if (command == "generate") {
        GenerateOptions options;
        if (!parse_generate_options(argc, argv, options)) {
            std::cerr << "Run 'cjm --help' for usage.\n";
            return kExitUsageError;
        }

        const auto& input = options.inputs[0];
        const auto parse_result = cjm::parser::parse_source_file(input);
        if (!parse_result.success) {
            std::cerr << "cjm: " << parse_result.error.message << ": "
                      << parse_result.error.location.file << "\n";
            return kExitFailure;
        }

        const auto analysis_result =
            cjm::semantic::analyze_source_file(parse_result.file);

        if (!analysis_result.success) {
            for (const auto& diagnostic : analysis_result.diagnostics) {
                std::cerr << diagnostic.location.file << ":"
                          << diagnostic.location.line << ":"
                          << diagnostic.location.column << ": "
                          << diagnostic.message << "\n";
            }
            return kExitFailure;
        }

        const auto generated =
            cjm::generator::generate_header(analysis_result.project);

        std::ofstream output(options.output);
        if (!output.is_open()) {
            std::cerr << "cjm: failed to open output file: " << options.output
                      << "\n";
            return kExitFailure;
        }
        output << generated;

        std::cout << "cjm: generated " << options.output << " from " << input
                  << "\n";
        return kExitSuccess;
    }

    std::cerr << "cjm: unknown command: " << command << "\n";
    std::cerr << "run 'cjm --help' for usage.\n";

    return kExitUsageError;
}
