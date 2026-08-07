# Required Field Missing

```text
id: required_missing
direction: decode
profile: core
capability: none
model: model.hpp
input: input.json
expected: expected.json
```

## Purpose

Verify that omitting a required field produces a portable missing-field error.

## Expected Behavior

Decoding fails because `count` is absent from the input object.

The decoder returns no partial `RequiredValues` object. The error code is 
`missing_required_field`, and its structured path contains the effective JSOn 
name `count`.
