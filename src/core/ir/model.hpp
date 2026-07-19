#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace cjm::metadata {

// Parser-independent location in the user's source file.
struct SourceLocation {
    std::string file;
    std::uint32_t line = 0;
    std::uint32_t column = 0;
};

// Validated JSON metadata attached to a generated field.
struct JsonFieldMetadata {
    std::string name;
    bool omit_empty = false;
};

// Parser-independent field type categories understood by semantic analysis.
enum class FieldTypeKind {
    Bool,
    SignedInteger,
    UnsignedInteger,
    FloatingPoint,
    String,
    Enum,
    Vector,
    Optional,
    UserDefined
};

// Parser-independent field type information.
struct FieldType {
    FieldTypeKind kind;
    std::string spelling;
    std::string qualified_name;
    std::vector<FieldType> arguments;
};

// One C++ data member selected for generation.
struct FieldModel {
    std::string name;
    FieldType type;
    JsonFieldMetadata json;
    SourceLocation source_location;
};

// One C++ type selected for generation.
struct TypeModel {
    std::string name;
    std::vector<std::string> namespace_path;
    std::string qualified_name;
    std::vector<FieldModel> fields;
    SourceLocation source_location;
};

// One CJM generation unit.
struct ProjectModel {
    std::vector<TypeModel> types;
};
} // namespace cjm::metadata
