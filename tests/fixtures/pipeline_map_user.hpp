#pragma once

#include <map>
#include <optional>
#include <string>
#include <unordered_map>

namespace company::model {

struct User {
    std::map<std::string, int> labels;                    // json:"labels"
    std::unordered_map<std::string, std::string> aliases; // json:"aliases"
    std::optional<std::map<std::string, int>> optional_labels; // json:"optional_labels,omitempty"
    std::optional<std::unordered_map<std::string, std::string>> optional_aliases; // json:"optional_aliases,omitempty"
};

} // namespace company::model
