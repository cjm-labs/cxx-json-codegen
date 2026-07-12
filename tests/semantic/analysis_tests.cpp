#include "semantic/analysis.hpp"

#include <cassert>

int main() {
    {

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
    }
    {
        cjm::semantic::SourceLocation location;
        location.file = "user.hpp";
        location.line = 4;
        location.column = 30;

        auto valid = cjm::semantic::parse_json_field_metadata(R"(json:"name")",
                                                              location);

        assert(valid.found);
        assert(valid.success);
        assert(valid.json_name == "name");
        assert(valid.diagnostic.message.empty());

        auto ordinary = cjm::semantic::parse_json_field_metadata(
            "ordinary comment", location);
        assert(!ordinary.found);
        assert(!ordinary.success);

        auto invalid =
            cjm::semantic::parse_json_field_metadata("json:name", location);
        assert(invalid.found);
        assert(!invalid.success);
        assert(invalid.diagnostic.location.file == "user.hpp");
        assert(!invalid.diagnostic.message.empty());
    }
    return 0;
}
