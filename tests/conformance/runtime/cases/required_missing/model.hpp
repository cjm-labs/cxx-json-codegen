#pragma once

#include <cstdint>
#include <string>

/// Defines required fields for the missing-field conformance case.
struct RequiredValues {
    std::string name;
    std::int32_t count;
};
