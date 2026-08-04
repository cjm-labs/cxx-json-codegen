#pragma once

#include <array>
#include <map>
#include <optional>
#include <string>
#include <vector>

namespace company::model {

enum class Status {
    Active,
    Disabled,
};

struct Address {
    std::string city;
};

struct RecursiveTypes {
    std::vector<std::vector<int>> matrix;
    std::vector<std::optional<int>> optional_scores;
    std::optional<std::vector<std::string>> aliases;
    std::map<std::string, std::vector<int>> buckets;
    std::array<std::optional<int>, 4> optional_samples;
    std::vector<Status> statuses;
    std::optional<Status> maybe_status;
    std::vector<Address> addresses;
    std::optional<Address> maybe_address;
    std::map<std::string, Address> address_by_id;
    std::vector<std::vector<Address>> address_groups;
};

} // namespace company::model
