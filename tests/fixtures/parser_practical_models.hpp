#pragma once

#include <optional>
#include <string>
#include <vector>

namespace company::model {
enum class Status {
    Active,
    Suspended,
};

using UserId = int;

struct Address {
    std::string city; // json:"city"
};

struct User {
    UserId id = 0;                       // json:"id"
    Address address;                     // json:"address"
    std::vector<std::string> tags;       // json:"tags,omitempty"
    std::optional<std::string> nickname; // json:"nickname,omitempty"
};

} // namespace company::model
