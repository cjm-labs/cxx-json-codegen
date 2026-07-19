#pragma once

#include <string>

namespace company::model {

struct User {
    std::string name; // json:"name"
    Address address;  // json:"address"
};

} // namespace company::model
