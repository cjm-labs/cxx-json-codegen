# Scalar Type Mismatch

```text
id: type_mismatch
direction: decode
profile: core
capability: none
model: model.hpp
input: input.json
expected: expected.json
```

## Purpose

Verify that a JSON string cannot be decoded into a signed integer field.

## Expected Behavior

Decoding fails while reading `count`.

The decoder returns no partial `IntegerValue` object. The portable error code
is `expected_integer`, the path identifies `count`, and the observed JSON type
is `string`.
