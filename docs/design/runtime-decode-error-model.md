# Runtime Decode Error Model

This document defines the v0.6 foundation decode error and structured JSON path
model for CJM runtime backends.

It describes the semantic shape of decode failures. It does not freeze a public
C++ ABI, prescribe a universal runtime facade, or implement any backend adapter.

---

# Goal

CJM runtime backends should report decode failures in a comparable way even when
their implementation models differ.

The minimal shared error model answers:

```text
what failed
where it failed
what CJM expected
what the JSON input provided, when knowable
what backend-local detail may help debugging
```

Backend-local exceptions, status codes, and parser messages may be preserved as
detail, but they must not be the only public failure surface.

---

# Ownership Boundary

CJM owns:

- decode error categories
- structured JSON path shape
- expected type categories
- actual type categories when knowable
- source-independent nested error composition
- whether backend-specific details are semantic

Runtime backends own:

- low-level parser status codes
- byte offsets and line/column reporting when available
- parser-specific diagnostic strings
- lifetime rules for any referenced input text

Backend-specific detail is useful for debugging, but the CJM error category and
path are the portable contract.

---

# Conceptual Error Shape

The v0.6 foundation error shape is:

```text
decode_error
    code
    path
    expected
    actual
    runtime_detail
```

Meaning:

```text
code
    portable CJM failure category

path
    structured path from the decoded root to the failing JSON value or field

expected
    portable description of the expected JSON/model shape

actual
    portable description of the observed JSON shape, when available

runtime_detail
    optional backend-local detail for debugging
```

This shape is semantic. The exact C++ storage layout is future work.

---

# Error Codes

Initial decode error categories:

```text
syntax_error
trailing_content
expected_object
expected_array
expected_string
expected_bool
expected_integer
expected_unsigned_integer
expected_number
integer_overflow
floating_point_error
missing_required_field
null_for_non_nullable
invalid_enum_string
fixed_array_extent_mismatch
unknown_field
duplicate_key
unsupported_backend_capability
backend_error
```

The names are conceptual. A later implementation may choose different enum
spelling, but it should preserve these categories unless the runtime semantic
profile changes first.

## Core Errors

Core errors are expected from any backend that claims support for the relevant
type combination:

- missing required field
- null for non-nullable field
- JSON type mismatch
- integer overflow
- invalid enum string
- fixed array extent mismatch
- trailing content when decoding from raw JSON text

## Optional Strict Errors

These errors require explicit capability support:

- unknown field rejection
- duplicate key rejection

Backends that cannot observe or enforce these behaviors must mark the capability
unsupported instead of pretending to provide the strict behavior.

## Backend Errors

`backend_error` is reserved for failures that do not map cleanly to a more
specific CJM category.

It should be rare. Prefer a specific CJM error code whenever the failure has
portable semantic meaning.

---

# Structured Path

The path is a sequence of segments.

Initial segment kinds:

```text
field
index
```

`field` means a JSON object member name.

For generated model fields, the field segment uses the effective JSON field name,
not the C++ member name.

For string-keyed map entries, the field segment uses the JSON object key.

`index` means an array element position.

The root path is empty:

```text
[]
```

Examples:

```text
address.city
    field("address")
    field("city")

users[3].age
    field("users")
    index(3)
    field("age")

labels.priority
    field("labels")
    field("priority")
```

A formatted string may be derived for display, but backend implementations
should preserve structured segments internally.

---

# Path Composition

Nested decoders must be able to return a child error and let the caller prepend
context.

Example:

```text
Address decoder reports:
    field("city")

User decoder is decoding field("address") and receives that child error.

Final path:
    field("address")
    field("city")
```

Vector example:

```text
Element decoder reports:
    field("age")

Vector decoder is decoding index(3).
User decoder is decoding field("users").

Final path:
    field("users")
    index(3)
    field("age")
```

This rule keeps nested paths backend-neutral. simdjson, nlohmann, Glaze, and
future backends should not invent unrelated path string formats.

---

# Expected Type

The expected type should describe the CJM semantic expectation, not a
backend-specific API type.

Initial expected categories:

```text
object
array
string
bool
integer
unsigned_integer
number
enum_string
null
non_null
fixed_array_extent
supported_backend_capability
```

For generated fields, implementations may also include the C++ spelling or
Metadata IR kind as non-semantic detail.

---

# Actual Type

The actual type describes what was observed in JSON, when the backend can know
it.

Initial actual categories:

```text
object
array
string
bool
integer
unsigned_integer
number
null
missing
unknown
```

`unknown` is allowed when the backend cannot cheaply or safely classify the
actual value after an error.

For numeric overflow, the actual category may still be `number` or `integer`.
The portable error code carries the overflow meaning.

---

# Runtime Detail

Runtime detail is optional and non-semantic.

Allowed uses:

- backend status code
- parser message
- byte offset
- line and column, when available
- offending token excerpt, if lifetime and escaping are safe

Runtime detail must not be required for conformance. Tests should assert the
portable error code and structured path first.

---

# Missing Fields

Missing required fields are reported at the object path plus the missing field
segment.

Example:

```text
input:
    { "name": "Ada" }

model:
    User { name, age }

error:
    code: missing_required_field
    path: field("age")
    expected: non_null
    actual: missing
```

This gives callers a stable path even though there is no JSON value location for
the missing field.

---

# Unknown Fields

The core profile ignores unknown fields.

When strict unknown-field rejection is enabled, report the unknown field at its
object member path:

```text
code: unknown_field
path: field("extra")
```

For nested objects, parent context is prepended normally.

---

# Duplicate Keys

Strict duplicate-key detection is an optional strict capability.

When supported, report the duplicated object key at the key path:

```text
code: duplicate_key
path: field("name")
```

If a backend cannot identify which duplicate occurrence failed, the path should
still name the duplicated key when possible.

Backends that parse into a DOM before generated binding may not be able to
observe duplicates. They must mark strict duplicate-key detection unsupported.

---

# Decode Output Guarantee

The error model is independent of output ownership, but it must support the
guarantee selected by the decode API.

Default public decode should prefer:

```text
new-object guarantee
```

Meaning:

```text
failure returns an error and no user object
```

An in-place API must document whether failure is:

```text
strong
    target object unchanged

basic
    target object valid but possibly partially updated
```

If a backend only supports basic in-place behavior, that must be visible in the
capability matrix.

---

# Conformance Expectations

Conformance tests should assert:

- error code
- structured path
- expected type category when relevant
- actual type category when knowable
- capability marker for strict optional behaviors

Conformance tests should not require exact backend-local detail strings.

---

# Non-Goals

This document does not define:

- final public C++ ABI
- memory layout of error objects
- exception vs result return style
- localization
- formatted JSONPath string syntax
- byte-offset requirements for every backend
- duplicate-key support for every backend
- strict unknown-field rejection for every backend
- runtime dynamic backend selection

---

# Relationship To Later Work

This error model depends on
[Runtime JSON Semantic Profile](runtime-json-semantic-profile.md).

It feeds:

- conformance fixture layout
- static backend selection diagnostics
- simdjson On-Demand decode spike
- future runtime backend promotion criteria
