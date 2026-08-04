#pragma once

#include <array>
#include <map>
#include <optional>
#include <string>
#include <vector>

namespace company::model {

struct Address {
    std::string city;
};

struct Config {
    std::vector<
        std::string
    > names;

    std::map<
        std::string,
        int
    > counts;

    std::vector<
        std::optional<int>
    > maybe_scores;

    std::map<
        std::string,
        std::vector<int>
    > buckets;

    std::optional<
        Address
    > address;

    std::array<
        int,
        4
    > samples;

    std::optional<
        std::string
    > nickname; // json:",omitempty"
};

} // namespace company::model
