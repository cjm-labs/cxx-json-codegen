# cxx-json-map
A Clang-based C++ struct-to-JSON mapping code generator.

public aggregate struct
-> generate to_json/from_json
-> support nlohmann/json backend

cxx-json-map/
├── README.md
├── CMakeLists.txt
├── docs/
│   ├── design.md
│   └── roadmap.md
├── include/
│   └── cxx_json_map/
│       └── annotations.hpp
├── tools/
│   └── cxx-json-map/
│       └── main.cpp
├── examples/
│   └── basic/
│       ├── bench_result.hpp
│       └── main.cpp
├── tests/
│   └── golden/
│       ├── basic.input.hpp
│       └── basic.expected.hpp
└── generated/
