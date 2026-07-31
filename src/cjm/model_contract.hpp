#pragma once

#include <cstdint>

namespace cjm::contract {

// Stable category names used by generated model descriptors.
enum class type_kind {
    bool_,
    signed_integer,
    unsigned_integer,
    floating_point,
    string,
    enum_,
    array,
    vector,
    map,
    optional,
    object
};

// Source location in the user's model file.
struct source_location {
    const char* file = "";
    std::uint32_t line = 0;
    std::uint32_t column = 0;
};

// Describes one supported enum value exposed through generated contracts.
struct enum_value_descriptor {
    const char* cpp_name = "";
    const char* json_name = "";
};

// Describes one field type in generated contract metadata.
struct type_descriptor {
    type_kind kind = type_kind::object;
    const char* spelling = "";
    const char* qualified_name = "";
    const type_descriptor* arguments = nullptr;
    std::uint32_t argument_count = 0;
    const enum_value_descriptor* enum_values = nullptr;
    std::uint32_t enum_value_count = 0;
};

// Describes one CJM-managed field.
struct field_descriptor {
    const char* cpp_name = "";
    const char* json_name = "";
    bool ignored = false;
    bool omit_empty = false;
    source_location location;
    const type_descriptor* type = nullptr;
};

// Describes one generated C++ model type.
struct model_descriptor {
    const char* cpp_name = "";
    const char* qualified_name = "";
    source_location location;
    const field_descriptor* fields = nullptr;
    std::uint32_t field_count = 0;
};

// Specialized by generated CJM headers for supported model types.
template <typename T> struct model_traits;

} // namespace cjm::contract
