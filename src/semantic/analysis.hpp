#pragma once

#include "metadata/model.hpp"

#include <string>
#include <vector>

namespace cjm::semantic {

struct SourceLocation {
    std::string file;
    int line = 0;
    int column = 0;
};

struct Diagnostic {
    SourceLocation location;
    std::string message;
};

struct AnalysisResult {
    metadata::ProjectModel project;
    std::vector<Diagnostic> diagnostics;
    bool success = false;
};

} // namespace cjm::semantic
