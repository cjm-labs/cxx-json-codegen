#include <cassert>
#include <cstdint>

#include <nlohmann/json.hpp>

namespace company::model {

struct Metrics {
    std::int8_t s8 = 0;
    std::int16_t s16 = 0;
    std::int32_t s32 = 0;
    std::int64_t s64 = 0;
    std::uint8_t u8 = 0;
    std::uint16_t u16 = 0;
    std::uint32_t u32 = 0;
    std::uint64_t u64 = 0;
};

inline void to_json(nlohmann::json& j, const Metrics& value) {
    j["s8"] = value.s8;
    j["s16"] = value.s16;
    j["s32"] = value.s32;
    j["s64"] = value.s64;
    j["u8"] = value.u8;
    j["u16"] = value.u16;
    j["u32"] = value.u32;
    j["u64"] = value.u64;
}

inline void from_json(const nlohmann::json& j, Metrics& value) {
    j.at("s8").get_to(value.s8);
    j.at("s16").get_to(value.s16);
    j.at("s32").get_to(value.s32);
    j.at("s64").get_to(value.s64);
    j.at("u8").get_to(value.u8);
    j.at("u16").get_to(value.u16);
    j.at("u32").get_to(value.u32);
    j.at("u64").get_to(value.u64);
}

} // namespace company::model

int main() {
    company::model::Metrics metrics;
    metrics.s8 = -8;
    metrics.s16 = -160;
    metrics.s32 = -32000;
    metrics.s64 = -64000;
    metrics.u8 = 8;
    metrics.u16 = 160;
    metrics.u32 = 32000;
    metrics.u64 = 64000;

    // 1. Verify fixed-width numeric fields can be serialized.
    nlohmann::json json = metrics;
    assert(json.at("s32") == -32000);
    assert(json.at("u64") == 64000);

    // 2. Verify fixed-width numeric fields can be deserialized.
    company::model::Metrics decoded = json.get<company::model::Metrics>();
    assert(decoded.s8 == -8);
    assert(decoded.s16 == -160);
    assert(decoded.s32 == -32000);
    assert(decoded.s64 == -64000);
    assert(decoded.u8 == 8);
    assert(decoded.u16 == 160);
    assert(decoded.u32 == 32000);
    assert(decoded.u64 == 64000);

    return 0;
}
