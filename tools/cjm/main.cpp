#include <iostream>
#include <string>

namespace {

void print_help(std::ostream& out) {
    out << "cjm - C++ JSON metadata code generator\n"
        << "\n"
        << "Usage :\n "
        << "    cjm --help\n"
        << "    cjm help\n"
        << "    cjm generate [options]\n";
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
        std::cerr << "cjm: generate is not implemented yet\n";
        return 1;
    }

    std::cerr << "cjm: unknown command: " << command << "\n";
    std::cerr << "run 'cjm --help' for usage.\n";

    return 1;
}
