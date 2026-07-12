#pragma once

#include <string>

struct User {
    std::string name; // json:"name"
    int age = 0;      // json:"age"
};
