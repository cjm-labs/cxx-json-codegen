#include <iostream>
#include <string>

namespace {

void print_help(std::ostream& out) {
    out << "cjm - C++ JSON metadata code generator\n"
        << "\n"
        << "Usage:\n"
        << "    cjm --help\n"
        << "    cjm help\n"
        << "    cjm generate --input <header> --outout <file>\n";
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

        std::cout << "cjm: generate is not implemented yet\n";
        std::cout << "input: " << options.input << "\n";
        std::cout << "output: " << options.output << "\n";
        return 0;
    }

    std::cerr << "cjm: unknown command: " << command << "\n";
    std::cerr << "run 'cjm --help' for usage.\n";

    return 1;
}
