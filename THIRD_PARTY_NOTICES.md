# Third-Party Notices

CJM uses or may evaluate the following third-party open-source projects.

This document is informational. Refer to each upstream project for the full and
authoritative license text.

## nlohmann/json

CJM's first official backend generates integration code for `nlohmann/json`.
CJM's build and test workflow may also use `nlohmann/json` through CMake when
it is not already available on the system.

- Project: https://github.com/nlohmann/json
- Website: https://json.nlohmann.me
- License: MIT License
- Copyright: Copyright (c) 2013-2026 Niels Lohmann

`nlohmann/json` is a third-party project. CJM is not affiliated with or
endorsed by the nlohmann/json project.

## Tree-sitter

Tree-sitter is planned for evaluation as a possible future C++ frontend
implementation detail during CJM's frontend parser research work.

- Project: https://github.com/tree-sitter/tree-sitter
- Website: https://tree-sitter.github.io
- License: MIT License
- Copyright: Copyright (c) 2018 Max Brunsfeld

Tree-sitter is not part of CJM's current production parser.

## tree-sitter-cpp

`tree-sitter-cpp` may be evaluated together with Tree-sitter during the planned
C++ frontend parser research work.

- Project: https://github.com/tree-sitter/tree-sitter-cpp
- License: MIT License
- Copyright: Copyright (c) 2014 Max Brunsfeld

CJM is not affiliated with or endorsed by the Tree-sitter or tree-sitter-cpp
projects.
