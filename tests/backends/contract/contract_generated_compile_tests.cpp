#include <cassert>
#include <string_view>

namespace company::model {

struct Address {};

enum class Status {
    Active,
};

struct User {};

} // namespace company::model

#include "tests/golden/basic_user.expected.cjm.contract.hpp"

int main() {
    // Verify that generated model_traits is usable from normal C++ code.
    {
        const auto& model =
            cjm::contract::model_traits<company::model::Address>::model;

        static_assert(
            cjm::contract::model_traits<company::model::Address>::model
                .field_count == 1);

        assert(model.cpp_name == std::string_view("Address"));
        assert(model.qualified_name ==
               std::string_view("company::model::Address"));
        assert(model.fields[0].cpp_name == std::string_view("city"));
        assert(model.fields[0].type->kind == cjm::contract::type_kind::string);
    }
    // Verify that field metadata remains inspectable through the public API.
    {
        const auto& model =
            cjm::contract::model_traits<company::model::User>::model;
        static_assert(cjm::contract::model_traits<company::model::User>::model
                          .field_count == 9);
        assert(model.fields[0].cpp_name == std::string_view("name"));
        assert(model.fields[0].json_name == std::string_view("name"));
        assert(model.fields[5].omit_empty);
        assert(model.fields[7].ignored);
        assert(model.fields[7].json_name == std::string_view(""));

        assert(model.fields[2].type->kind == cjm::contract::type_kind::vector);
        assert(model.fields[2].type->argument_count == 1);
        assert(model.fields[2].type->arguments[0].kind ==
               cjm::contract::type_kind::string);

        assert(model.fields[6].type->kind ==
               cjm::contract::type_kind::optional);
        assert(model.fields[6].type->arguments[0].kind ==
               cjm::contract::type_kind::signed_integer);

        assert(model.fields[8].cpp_name == std::string_view("attributes"));
        assert(model.fields[8].type->kind == cjm::contract::type_kind::map);
        assert(model.fields[8].type->argument_count == 2);
        assert(model.fields[8].type->arguments[0].kind ==
               cjm::contract::type_kind::string);
        assert(model.fields[8].type->arguments[1].kind ==
               cjm::contract::type_kind::signed_integer);
    }
    return 0;
}
