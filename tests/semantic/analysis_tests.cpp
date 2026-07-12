#include "semantic/analysis.hpp"

#include <cassert>

int main() {
    cjm::semantic::AnalysisResult result;
    result.success = false;

    cjm::semantic::Diagnostic diagnostic;
    diagnostic.location.file = "user.hpp";
    diagnostic.location.line = 4;
    diagnostic.location.column = 17;
    diagnostic.message = "invalid CJM metadata";

    result.diagnostics.push_back(diagnostic);

    assert(!result.success);
    assert(result.diagnostics.size() == 1);
    assert(result.diagnostics[0].location.file == "user.hpp");
    assert(result.diagnostics[0].location.line == 4);
    assert(result.diagnostics[0].location.column == 17);
    assert(result.diagnostics[0].message == "invalid CJM metadata");

    return 0;
}
