# Trailing Content

```text
id: trailing_content
direction: decode
profile: core
capability: backend_owned_trailing_content
model: model.hpp
input: input.json
expected: expected.json
```

## Purpose

Verify that raw-text decode rejects non-whitespace content after a complete
root JSON value.

## Expected Behavior

The first object is a complete root value. The second object is trailing
non-whitespace content.

The decoder returns no `TrailingContentValue` object. The portable error code
is `trailing_content`, and the structured path is the empty root path.
