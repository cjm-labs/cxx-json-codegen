# Scalar Decode Success

```text 
id: scalar_success
direction: decode
profile: core
capability: none
model: model.hpp
input: input.json
expected: expected.json
```


## Purpose

Verify that one required field of every scalar kind supported by the initial
decode spike can be decoded into a complete C++ object.

## Expected Behavior

The decoder reads the complete raw JSON text and returns a `ScalarValues` 
object containing the values recorded in `expected.json`.

The effective JSON names are the C++ field names because the model does not 
declare explicit JSON field metadata.
