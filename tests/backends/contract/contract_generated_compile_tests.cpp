#include <cassert>
#include <string_view>

namespace company::model {

struct Address {};

enum class Status {
    Active,
    Disabled,
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
    // Verify normalized JSON mapping metadata exposed by the contract API.
    {
        const auto& model =
            cjm::contract::model_traits<company::model::User>::model;
        static_assert(cjm::contract::model_traits<company::model::User>::model
                          .field_count == 10);
        assert(model.fields[0].cpp_name == std::string_view("name"));
        assert(model.fields[0].json_name == std::string_view("name"));

        assert(model.fields[5].json_name == std::string_view("nickname"));
        assert(model.fields[5].cpp_name == std::string_view("nickname"));
        assert(model.fields[5].omit_empty == true);
        assert(model.fields[5].ignored == false);

        assert(model.fields[7].json_name == std::string_view(""));
        assert(model.fields[7].cpp_name == std::string_view("internal_id"));
        assert(model.fields[7].ignored == true);
        assert(model.fields[7].omit_empty == false);

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

        assert(model.fields[9].cpp_name == std::string_view("samples"));
        assert(model.fields[9].type->kind == cjm::contract::type_kind::array);
        assert(model.fields[9].type->argument_count == 1);
        assert(model.fields[9].type->arguments[0].kind ==
               cjm::contract::type_kind::signed_integer);

        assert(model.fields[4].type->kind == cjm::contract::type_kind::enum_);
        assert(model.fields[4].type->enum_value_count == 2);
        assert(model.fields[4].type->enum_values[0].cpp_name ==
               std::string_view("Active"));
        assert(model.fields[4].type->enum_values[0].json_name ==
               std::string_view("Active"));
        assert(model.fields[4].type->enum_values[1].cpp_name ==
               std::string_view("Disabled"));
    }
    return 0;
}
