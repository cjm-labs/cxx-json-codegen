#pragma once

#include <cstdint>
#include <string>

/// Defines every scalar field kind covered by the initial decode spike.
struct ScalarValues {
    bool enabled;
    std::int32_t count;
    std::uint32_t limit;
    double ratio;
    std::string name;
};
