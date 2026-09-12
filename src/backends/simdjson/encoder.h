#pragma once

#include "core/ir/model.hpp"

#include <iosfwd>

namespace cjm::generator::simdjson::detail {

// Generate the experimental encode error and public API declarations.
void generate_encode_error_model(std::ostringstream& out);

// Generate an object encoder for a model with boolean fields.
void generate_bool_object_encode_function(std::ostringstream& out,
                                          const metadata::TypeModel& type);

// Generate the public root encoder for one model.
void generate_root_encode_function(std::ostringstream& out,
                                   const metadata::TypeModel& type);

} // namespace cjm::generator::simdjson::detail
