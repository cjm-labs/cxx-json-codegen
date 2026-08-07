# Integer Overflow

```text
id: integer_overflow
direction: decode
profile: core
capability: none
model: model.hpp
input: input.json
expected: expected.json
```

## Purpose

Verify that a valid JSON integer outside the target C++ integer range produces
a portable overflow error.

## Expected Behavior

The JSON value `128` is an integer, but it is greater than the maximum value of
`std::int8_t`.

The decoder returns no partial `NarrowIntegerValue` object. The portable error
code is `integer_overflow`, and the structured path identifies `count`.
