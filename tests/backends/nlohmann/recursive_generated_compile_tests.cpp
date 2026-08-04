#include <array>
#include <cassert>
#include <map>
#include <optional>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

namespace company::model {

struct Address {
    std::string city;
};

struct RecursiveNlohmann {
    std::vector<std::vector<int>> matrix;
    std::optional<std::vector<std::string>> aliases;
    std::map<std::string, std::vector<int>> buckets;
    std::array<std::vector<int>, 2> sample_rows;
    std::vector<Address> addresses;
    std::optional<Address> maybe_address;
    std::map<std::string, Address> address_by_id;
    std::vector<std::vector<Address>> address_groups;
};

} // namespace company::model

#include "tests/golden/recursive_nlohmann.expected.cjm.hpp"

int main() {
    company::model::RecursiveNlohmann value;
    value.matrix = {{1, 2}, {3, 4}};
    value.aliases = std::vector<std::string>{"primary", "secondary"};
    value.buckets = {{"odd", {1, 3}}, {"even", {2, 4}}};
    value.sample_rows = {{{1, 2}, {3, 4}}};
    value.addresses = {{"Boston"}, {"Cambridge"}};
    value.maybe_address = company::model::Address{"Framingham"};
    value.address_by_id = {{"home", {"Boston"}}, {"work", {"Cambridge"}}};
    value.address_groups = {{{"Boston"}, {"Cambridge"}}, {{"Worcester"}}};

    nlohmann::json json = value;

    assert(json.at("matrix").at(1).at(0) == 3);
    assert(json.at("aliases").at(0) == "primary");
    assert(json.at("buckets").at("even").at(1) == 4);
    assert(json.at("sample_rows").at(1).at(0) == 3);
    assert(json.at("addresses").at(0).at("city") == "Boston");
    assert(json.at("maybe_address").at("city") == "Framingham");
    assert(json.at("address_by_id").at("work").at("city") == "Cambridge");
    assert(json.at("address_groups").at(0).at(1).at("city") == "Cambridge");

    const auto decoded = json.get<company::model::RecursiveNlohmann>();

    assert(decoded.matrix == value.matrix);
    assert(decoded.aliases.has_value());
    assert(decoded.aliases->at(1) == "secondary");
    assert(decoded.buckets.at("odd").at(1) == 3);
    assert(decoded.sample_rows.at(0).at(1) == 2);
    assert(decoded.addresses.at(1).city == "Cambridge");
    assert(decoded.maybe_address.has_value());
    assert(decoded.maybe_address->city == "Framingham");
    assert(decoded.address_by_id.at("home").city == "Boston");
    assert(decoded.address_groups.at(1).at(0).city == "Worcester");

    company::model::RecursiveNlohmann sparse;
    sparse.matrix = {{1}};
    sparse.buckets = {{"single", {1}}};
    sparse.sample_rows = {{{1}, {2}}};
    sparse.addresses = {{"Boston"}};
    sparse.address_by_id = {{"home", {"Boston"}}};
    sparse.address_groups = {{{"Boston"}}};

    nlohmann::json sparse_json = sparse;
    assert(!sparse_json.contains("aliases"));
    assert(!sparse_json.contains("maybe_address"));

    return 0;
}
