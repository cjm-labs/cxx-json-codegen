#include "semantic/analysis.hpp"

namespace cjm::semantic {

JsonFieldMetadataResult
parse_json_field_metadata(const std::string& comment,
                          const SourceLocation& location) {
    JsonFieldMetadataResult result;

    const std::string prefix = "json:";
    if (comment.rfind(prefix, 0) != 0) {
        result.found = false;
        return result;
    }

    result.found = true;
    const auto value = comment.substr(prefix.size());
    if (value.size() < 2 || value.front() != '"' || value.back() != '"') {
        result.success = false;
        result.diagnostic.location = location;
        result.diagnostic.message =
            R"(invalid CJM json metadata, expected json:"name")";
        return result;
    }

    result.json_name = value.substr(1, value.size() - 2);
    if (result.json_name.empty()) {
        result.success = false;
        result.diagnostic.location = location;
        result.diagnostic.message =
            "invalid CJM json metadata: empty JSON field name";
        return result;
    }
    result.success = true;
    return result;
}
} // namespace cjm::semantic
