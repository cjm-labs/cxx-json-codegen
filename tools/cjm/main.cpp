#include <fstream>
#include <iostream>
#include <string>

#include "generator/cpp_generator.hpp"
#include "metadata/model.hpp"

namespace {

void print_help(std::ostream& out) {
    out << "cjm - C++ JSON metadata code generator\n"
        << "\n"
        << "Usage:\n"
        << "    cjm --help\n"
        << "    cjm help\n"
        << "    cjm generate --input <header> --output <file>\n";
}

struct GenerateOptions {
    std::string input;
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

            options.input = argv[++i];
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

    if (options.input.empty()) {
        std::cerr << "cjm: generate requires --input <header>\n";
        return false;
    }
    if (options.output.empty()) {
        std::cerr << "cjm: generate requires --output <file>\n";
        return false;
    }
    return true;
}

// Temporary bridge until parser and semantic analysis can build the real
// metadata model.
cjm::metadata::ProjectModel make_temporary_project_model() {
    using namespace cjm::metadata;

    TypeModel user;
    user.name = "User";

    FieldModel name;
    name.name = "name";
    name.type.spelling = "std::string";
    name.json.name = "name";

    FieldModel age;
    age.name = "age";
    age.type.spelling = "int";
    age.json.name = "age";

    user.fields.push_back(name);
    user.fields.push_back(age);

    ProjectModel project;
    project.types.push_back(user);

    return project;
}

} // namespace

int main(int argc, char** argv) {
    if (argc <= 1) {
        print_help(std::cout);
        return 0;
    }

    const std::string command = argv[1];

    if (command == "--help" || command == "-h" || command == "help") {
        print_help(std::cout);
        return 0;
    }

    if (command == "generate") {
        GenerateOptions options;
        if (!parse_generate_options(argc, argv, options)) {
            std::cerr << "Run 'cjm --help' for usage.\n";
            return 1;
        }

        const auto project = make_temporary_project_model();
        const auto generated = cjm::generator::generate_header(project);

        std::ofstream output(options.output);
        if (!output.is_open()) {
            std::cerr << "cjm: failed to open output file: " << options.output
                      << "\n";
            return 1;
        }
        output << generated;

        std::cout << "cjm: generated " << options.output << " from "
                  << options.input << "\n";
        return 0;
    }

    std::cerr << "cjm: unknown command: " << command << "\n";
    std::cerr << "run 'cjm --help' for usage.\n";

    return 1;
}
