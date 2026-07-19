#include <cassert>
#include <optional>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

namespace company::model {
enum class Status {
    Active,
    Disabled,
};

struct Address {
    std::string city;
};

struct User {
    std::string name;
    int age = 0;
    int internal_id = 0;
    std::vector<std::string> tags;
    Address address;
    Status status = Status::Active;
    std::optional<std::string> nickname;
    std::optional<int> score;
};

} // namespace company::model

#include "tests/golden/basic_user.expected.cjm.hpp"

int main() {
    company::model::User user;
    user.name = "Ming";
    user.age = 25;
    user.tags = {"compiler", "metadata"};
    user.address.city = "Framingham";
    user.status = company::model::Status::Active;
    user.nickname = "fang";
    user.score = 100;

    nlohmann::json json = user;

    company::model::User decoded = json.get<company::model::User>();

    assert(decoded.name == "Ming");
    assert(decoded.age == 25);
    assert(decoded.tags.size() == 2);
    assert(decoded.address.city == "Framingham");
    assert(decoded.nickname.has_value());
    assert(decoded.score == 100);

    return 0;
}
