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
    std::string source; // json:"source"
    std::uint32_t shard;  // json:"shard"
};

struct Event {
    Sequence sequence;             // json:"sequence"
    std::int64_t timestamp_ns;     // json:"timestamp_ns"
    Status status;                 // json:"status"
    Detail detail;                 // json:"detail"
    std::vector<std::string> tags; // json:"tags"
    std::map<std::string, std::vector<std::uint64_t>> buckets; // json:"buckets"
    std::optional<Sequence> retry_after; // json:"retry_after,omitempty"
    std::optional<std::unordered_map<std::string, std::string>> attributes; // json:"attributes,omitempty"
};

} // namespace company::model
