#include <cassert>
#include <map>
#include <optional>
#include <string>
#include <unordered_map>

#include <nlohmann/json.hpp>

namespace company::model {
struct User {
    std::map<std::string, int> labels;
    std::unordered_map<std::string, std::string> aliases;
    std::optional<std::map<std::string, int>> optional_labels;
    std::optional<std::unordered_map<std::string, std::string>>
        optional_aliases;
};

} // namespace company::model

#include "tests/golden/map_user.expected.cjm.hpp"

int main() {
    company::model::User user;
    user.labels = {{"one", 1}, {"two", 2}};
    user.aliases = {{"id", "u-1"}, {"name", "Ming"}};
    user.optional_labels = std::map<std::string, int>{{"score", 100}};
    user.optional_aliases =
        std::unordered_map<std::string, std::string>{{"nickname", "fang"}};

    // 1. Verify generated to_json handles required and optional maps.
    nlohmann::json json = user;
    assert(json.at("labels").at("one") == 1);
    assert(json.at("aliases").at("id") == "u-1");
    assert(json.contains("optional_labels"));
    assert(json.contains("optional_aliases"));

    // 2. Verify generated from_json reconstructs map fields.
    company::model::User decoded = json.get<company::model::User>();
    assert(decoded.labels.at("two") == 2);
    assert(decoded.aliases.at("name") == "Ming");
    assert(decoded.optional_labels.has_value());
    assert(decoded.optional_labels->at("score") == 100);
    assert(decoded.optional_aliases.has_value());
    assert(decoded.optional_aliases->at("nickname") == "fang");

    // 3. Verify omitempty still omits empty optional map fields.
    company::model::User sparse;
    sparse.labels = {{"only", 1}};
    sparse.aliases = {{"id", "u-2"}};

    nlohmann::json sparse_json = sparse;
    assert(!sparse_json.contains("optional_labels"));
    assert(!sparse_json.contains("optional_aliases"));

    return 0;
}
