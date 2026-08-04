#include <cassert>
#include <string_view>

namespace company::contract_test {

struct RecursiveAddress {};

struct RecursiveContract {};

} // namespace company::contract_test

#include "tests/golden/recursive_contract.expected.cjm.contract.hpp"

namespace {

// Assert one contract type descriptor has the expected type kind.
void assert_kind(const cjm::contract::type_descriptor& type,
                 cjm::contract::type_kind kind) {
    assert(type.kind == kind);
}

// Return the only contract type argument for a one-argument descriptor.
const cjm::contract::type_descriptor&
only_argument(const cjm::contract::type_descriptor& type) {
    assert(type.argument_count == 1);
    assert(type.arguments != nullptr);
    return type.arguments[0];
}

// Return the value descriptor for a string-keyed map descriptor.
const cjm::contract::type_descriptor&
map_string_value(const cjm::contract::type_descriptor& type) {
    assert_kind(type, cjm::contract::type_kind::map);
    assert(type.argument_count == 2);
    assert(type.arguments != nullptr);
    assert_kind(type.arguments[0], cjm::contract::type_kind::string);
    return type.arguments[1];
}

} // namespace

int main() {
    const auto& model =
        cjm::contract::model_traits<
            company::contract_test::RecursiveContract>::model;
    static_assert(
        cjm::contract::model_traits<
            company::contract_test::RecursiveContract>::model.field_count == 7);

    assert(model.cpp_name == std::string_view("RecursiveContract"));
    assert(model.qualified_name ==
           std::string_view("company::contract_test::RecursiveContract"));

    const auto& matrix = *model.fields[0].type;
    assert_kind(matrix, cjm::contract::type_kind::vector);
    const auto& matrix_row = only_argument(matrix);
    assert_kind(matrix_row, cjm::contract::type_kind::vector);
    assert_kind(only_argument(matrix_row),
                cjm::contract::type_kind::signed_integer);

    const auto& aliases = *model.fields[1].type;
    assert_kind(aliases, cjm::contract::type_kind::optional);
    const auto& alias_vector = only_argument(aliases);
    assert_kind(alias_vector, cjm::contract::type_kind::vector);
    assert_kind(only_argument(alias_vector), cjm::contract::type_kind::string);
    assert(model.fields[1].omit_empty == true);

    const auto& bucket_value = map_string_value(*model.fields[2].type);
    assert_kind(bucket_value, cjm::contract::type_kind::vector);
    assert_kind(only_argument(bucket_value),
                cjm::contract::type_kind::signed_integer);

    const auto& addresses = *model.fields[3].type;
    assert_kind(addresses, cjm::contract::type_kind::vector);
    const auto& address = only_argument(addresses);
    assert_kind(address, cjm::contract::type_kind::object);
    assert(address.qualified_name ==
           std::string_view("company::contract_test::RecursiveAddress"));

    const auto& maybe_address = *model.fields[4].type;
    assert_kind(maybe_address, cjm::contract::type_kind::optional);
    const auto& optional_address = only_argument(maybe_address);
    assert_kind(optional_address, cjm::contract::type_kind::object);
    assert(optional_address.qualified_name ==
           std::string_view("company::contract_test::RecursiveAddress"));

    const auto& address_by_id_value = map_string_value(*model.fields[5].type);
    assert_kind(address_by_id_value, cjm::contract::type_kind::object);
    assert(address_by_id_value.qualified_name ==
           std::string_view("company::contract_test::RecursiveAddress"));

    const auto& address_groups = *model.fields[6].type;
    assert_kind(address_groups, cjm::contract::type_kind::vector);
    const auto& address_group = only_argument(address_groups);
    assert_kind(address_group, cjm::contract::type_kind::vector);
    const auto& grouped_address = only_argument(address_group);
    assert_kind(grouped_address, cjm::contract::type_kind::object);
    assert(grouped_address.qualified_name ==
           std::string_view("company::contract_test::RecursiveAddress"));

    return 0;
}
