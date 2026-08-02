# Generated Model Contract

CJM v0.4 introduces a generated model contract for downstream tools.

The goal is to let schema generators, typed endpoint libraries, CLI/RPC
binders, documentation tools, and other downstream targets consume model facts
without depending on CJM internal IR structures or parser implementation
details.

This is a design document for the v0.4 contract. The v0.4 implementation
started with generated `model_traits` data for supported mappings.

---

# Problem

Generated `to_json` and `from_json` functions are enough for serialization, but
they are not enough for downstream tooling.

A downstream tool may need to know:

- which C++ type is being described
- which fields participate in CJM metadata
- C++ field names
- JSON field names
- field order
- type categories
- ignored fields
- `omitempty`
- source locations when practical

Today those facts exist inside CJM's Metadata IR, but downstream tools should
not include or depend on `cjm::metadata::ProjectModel`, `TypeModel`,
`FieldModel`, or parser-specific syntax types.

---

# Design Goal

The generated model contract should be:

- generated C++
- stable enough for downstream experiments
- independent from parser implementation details
- independent from CJM internal Metadata IR layout
- deterministic
- readable
- usable from ordinary CMake targets

The contract should not become:

- a runtime reflection system
- a general C++ AST
- an HTTP framework API
- a schema backend by itself
- a plugin system

---

# Layering

The contract is produced from validated Metadata IR:

```text
Parser
  |
  v
Semantic Analysis
  |
  v
Metadata IR
  |
  v
Generated C++ contract
  |
  v
Downstream tools
```

Downstream tools consume generated artifacts. They must not consume parser
nodes, Tree-sitter nodes, or CJM internal IR objects.

---

# Proposed Public Support Header

The generated code should use a small public support header:

```cpp
#include <cjm/model_contract.hpp>
```

The support header owns stable public contract types such as:

```cpp
namespace cjm::contract {

enum class type_kind {
    bool_,
    signed_integer,
    unsigned_integer,
    floating_point,
    string,
    enum_,
    array,
    vector,
    map,
    optional,
    object
};

struct source_location {
    const char* file;
    unsigned line;
    unsigned column;
};

struct enum_value_descriptor {
    const char* cpp_name;
    const char* json_name;
};

struct type_descriptor {
    type_kind kind;
    const char* spelling;
    const char* qualified_name;
    const type_descriptor* arguments;
    unsigned argument_count;
    const enum_value_descriptor* enum_values;
    unsigned enum_value_count;
};

struct field_descriptor {
    const char* cpp_name;
    const char* json_name;
    bool ignored;
    bool omit_empty;
    source_location location;
    const type_descriptor* type;
};

struct model_descriptor {
    const char* cpp_name;
    const char* qualified_name;
    source_location location;
    const field_descriptor* fields;
    unsigned field_count;
};

template <typename T>
struct model_traits;

} // namespace cjm::contract
```

This support header is public API. It should be small, stable, and independent
from parser and backend implementation details.

---

# Proposed Generated Shape

For a user model:

```cpp
namespace company::model {

struct User {
    std::string name; // json:"name"
    std::optional<std::string> nickname; // json:"nickname,omitempty"
};

} // namespace company::model
```

CJM may generate contract data similar to:

```cpp
namespace cjm::contract {

namespace generated_company_model_User {

inline constexpr type_descriptor string_type{
    type_kind::string,
    "std::string",
    "std::string",
    nullptr,
    0,
    nullptr,
    0,
};

inline constexpr type_descriptor optional_string_args[] = {
    string_type,
};

inline constexpr type_descriptor optional_string_type{
    type_kind::optional,
    "std::optional<std::string>",
    "std::optional",
    optional_string_args,
    1,
    nullptr,
    0,
};

inline constexpr field_descriptor fields[] = {
    {
        "name",
        "name",
        false,
        false,
        {"user.hpp", 4, 5},
        &string_type,
    },
    {
        "nickname",
        "nickname",
        false,
        true,
        {"user.hpp", 5, 5},
        &optional_string_type,
    },
};

inline constexpr model_descriptor model{
    "User",
    "company::model::User",
    {"user.hpp", 3, 1},
    fields,
    2,
};

} // namespace generated_company_model_User

template <>
struct model_traits<company::model::User> {
    static constexpr model_descriptor model{
        "User",
        "company::model::User",
        {"user.hpp", 3, 1},
        generated_company_model_User::fields,
        2,
    };
};

} // namespace cjm::contract
```

The exact generated namespace may change during implementation, but it must be
deterministic and collision-resistant enough for generated headers.

---

# Field Ordering

Field order in the generated contract must match `TypeModel.fields` order.

This keeps downstream behavior deterministic and consistent with generated JSON
field order.

---

# Type Categories

The public `type_kind` values should map from Metadata IR `FieldTypeKind`:

```text
Bool            -> bool_
SignedInteger   -> signed_integer
UnsignedInteger -> unsigned_integer
FloatingPoint   -> floating_point
String          -> string
Enum            -> enum_
Array           -> array
Vector          -> vector
Map             -> map
Optional        -> optional
UserDefined     -> object
```

Container-like types should expose their arguments through `type_descriptor`.

Examples:

```text
std::array<int, 4>
  kind = array
  arguments = [signed_integer]

std::vector<std::string>
  kind = vector
  arguments = [string]

std::map<std::string, int>
  kind = map
  arguments = [string, signed_integer]

std::optional<User>
  kind = optional
  arguments = [object]
```

Enum types should expose the string values that CJM knows how to generate and
parse.

Example:

```text
enum class Status {
  Active,
  Disabled,
};

Status
  kind = enum_
  enum_values = [
    {cpp_name = "Active", json_name = "Active"},
    {cpp_name = "Disabled", json_name = "Disabled"},
  ]
```

For v0.4, `cpp_name` and `json_name` are the same because custom enum rename
metadata is not yet supported.

---

# Ignored Fields

If Semantic Analysis keeps ignored fields in the Metadata IR with an empty JSON
name, the generated contract should expose:

```text
ignored = true
json_name = ""
```

The nlohmann backend may skip ignored fields during `to_json` and `from_json`,
but downstream tools may still want to know that the field was explicitly
managed and ignored.

---

# Optional And Required Semantics

v0.4 should expose what CJM already knows:

- the field type category may be `optional`
- `omit_empty` records whether `omitempty` was requested

v0.4 should not invent full required/default semantics before those metadata
features land.

Future releases may add explicit required/default metadata to the contract.

---

# Source Locations

Source locations are useful for downstream diagnostics.

The v0.4 contract may expose best-effort source locations from Metadata IR:

```cpp
{"include/model/user.hpp", 12, 5}
```

Path stability is not guaranteed across all build systems yet. Downstream tools
should treat source locations as diagnostic hints, not identity keys.

---

# Generated File Placement

The generated model contract may initially live in the same `*.cjm.hpp` file as
the nlohmann integration.

This keeps the first implementation simple and lets downstream targets consume
one generated header.

Future versions may split generated artifacts, for example:

```text
user.cjm.hpp
user.cjm.meta.hpp
```

Splitting should happen only when a concrete backend or packaging need appears.

---

# CMake Consumption

The contract should be consumed through the generated artifact handles added in
v0.4.1:

```cmake
cjm_generate(
    TARGET app
    HEADERS user.hpp
    GENERATED_TARGET app_cjm_generated
    GENERATED_HEADERS_VAR app_cjm_headers
    GENERATED_INCLUDE_DIR_VAR app_cjm_include_dir
)

add_dependencies(tool app_cjm_generated)
target_sources(tool PRIVATE ${app_cjm_headers})
target_include_directories(tool PRIVATE ${app_cjm_include_dir})
```

Downstream tools should not rely on generated file paths by guessing
repository-internal layout.

---

# Compatibility

For v0.4, the generated model contract should be considered experimental but
intentional.

Compatibility expectations:

- names and fields should be documented
- breaking changes may still happen before v1.0
- v1.0 should either stabilize the contract or mark specific parts
  experimental
- generated nlohmann JSON behavior should remain compatible unless explicitly
  changed by a separate issue

---

# Out Of Scope

This design does not itself implement:

- the JSON Schema backend
- OpenAPI route generation
- HTTP endpoint binding
- runtime reflection registry
- plugin loading
- custom converter behavior
- required/default metadata
- package installation

---

# Implementation Direction

The follow-up implementation issue should proceed bottom-up:

1. Add the public support header with contract data structures.
2. Add small tests for contract type construction.
3. Generate contract data for one simple model.
4. Add golden tests for the generated contract section.
5. Extend coverage to supported v0.3 field categories.
6. Verify downstream CMake consumption through generated artifact handles.

The implementation should not change parser behavior, Semantic Analysis
behavior, or nlohmann JSON serialization behavior.
