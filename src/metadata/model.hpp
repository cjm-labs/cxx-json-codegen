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

enum class FieldTypeKind {
    Bool,
    SignedInteger,
    UnsignedInteger,
    FloatingPoint,
    String,
    UserDefined
};

struct FieldType {
    FieldTypeKind kind;
    std::string spelling;
    std::string qualified_name;
};

struct FieldModel {
    std::string name;
    FieldType type;
    JsonFieldMetadata json;
    SourceLocation source_location;
};
} // namespace cjm::metadata
