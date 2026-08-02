#pragma once

#include "core/ir/model.hpp"

#include <string>

namespace cjm::generator::schema {

// Generate deterministic JSON Schema text from validated Metadata IR.
std::string generate_schema(const metadata::ProjectModel& project);

} // namespace cjm::generator::schema
