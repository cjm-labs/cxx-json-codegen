#include "user.hpp"

int main() {

    User user;
    user.name = "Ada";
    user.age = 42;
    return user.age == 42 ? 0 : 1;
}
