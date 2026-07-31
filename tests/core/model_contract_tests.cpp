#include "cjm/model_contract.hpp"

#include <cassert>
#include <string_view>

namespace {

struct User {};

} // namespace

template <> struct cjm::contract::model_traits<User> {
    static constexpr cjm::contract::type_descriptor string_type{
        cjm::contract::type_kind::string,
        "std::string",
        "std::string",
        nullptr,
        0,
    };

    static constexpr cjm::contract::field_descriptor fields[] = {
        {
            "name",
            "name",
            false,
            false,
            {"user.hpp", 3, 5},
            &string_type,
        },
    };

    static constexpr cjm::contract::model_descriptor model{
        "User", "User", {"user.hpp", 1, 1}, fields, 1,
    };
};

int main() {
    {
        const auto& model = cjm::contract::model_traits<User>::model;

        assert(model.cpp_name == std::string_view("User"));
        assert(model.field_count == 1);
        assert(model.fields[0].json_name == std::string_view("name"));
        assert(model.fields[0].type->kind == cjm::contract::type_kind::string);
    }
    return 0;
}
