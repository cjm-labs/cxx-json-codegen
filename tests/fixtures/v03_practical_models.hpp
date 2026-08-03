#pragma once

#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace company::model {
enum class Status {
    Ok,
    Failed,
};

using Sequence = std::uint64_t;

struct Detail {
    std::string source;
    std::uint32_t shard;
};

struct Event {
    Sequence sequence;
    std::int64_t timestamp_ns;
    Status status;
    Detail detail;
    std::vector<std::string> tags;
    std::map<std::string, std::vector<std::uint64_t>> buckets;
    std::optional<Sequence> retry_after; // json:",omitempty"
    std::optional<std::unordered_map<std::string, std::string>>
        attributes; // json:",omitempty"
};

} // namespace company::model
