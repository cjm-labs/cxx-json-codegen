#include <simdjson.h>

// Verify that the selected simdjson dependency exposes the On-Demand API.
int main() {
    simdjson::ondemand::parser parser;
    (void)parser;
    return 0;
}
