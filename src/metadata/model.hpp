#pragma once

#include <cstdint>
#include <string>

namespace cjm::metadata {

struct SourceLocation {
  std::string file;
  std::uint32_t line = 0;
  std::uint32_t column = 0;
};

struct JsonFieldMetadata {
  std::string name;
  bool omit_empty = false;
};
} // namespace cjm::metadata
