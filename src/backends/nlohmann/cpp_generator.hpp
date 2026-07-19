#pragma once

#include "core/ir/model.hpp"

#include <string>

namespace cjm::generator {

// Generates the contents of one *.cjm.hpp header from validated metadata.
std::string generate_header(const metadata::ProjectModel &project);

} // namespace cjm::generator
