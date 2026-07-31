#pragma once

#include "core/ir/model.hpp"

#include <string>

namespace cjm::generator::contract {

// Generate a standalone model contract header from validated Metadata IR.
std::string generate_header(const metadata::ProjectModel& project);

} // namespace cjm::generator::contract
