#pragma once

#include "core/ir/model.hpp"

#include <string>

namespace cjm::generator::simdjson {

struct GenerationResult {
    bool success = false;
    std::string header;
    std::string error;
};

// Generate an experimental simdjson header from supported Metadata IR.
GenerationResult generate_header(const metadata::ProjectModel& project);

} // namespace cjm::generator::simdjson
