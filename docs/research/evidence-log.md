# CJM Research Evidence Log

This file preserves research-relevant evidence created during normal product
development.

It is not a marketing changelog.

Evidence entries should distinguish observable facts from inference and should
avoid stronger conclusions than the linked evidence supports.

---

# Entry Template

```markdown
## YYYY-MM-DD - Evidence title

### Product Reference

- Release:
- Commit:
- Pull request:
- Issue:
- Downstream project:

### Product Change

Describe what changed in the real product.

### Research Relevance

- Related research question:
- Relevant hypothesis:

### Product Value

What product decision, test, diagnostic, or user experience does this evidence
improve independently of research?

### Research Option Value

Could this evidence support a future research question? If so, which one?

### Additional Research Cost

What extra work would be required to turn this observation into publishable
evidence?

### Current Investment Decision

- Preserve only
- Candidate for pilot
- Pilot active
- Full study approved
- Stopped

### Observation

Record the observable fact.

### Evidence

Link to:

- tests;
- golden files;
- release notes;
- dogfood reports;
- CI runs;
- code changes;
- measurements.

### What This Supports

State the limited conclusion supported by the evidence.

### What This Does Not Prove

Explicitly identify conclusions that cannot yet be drawn.

### Follow-Up Evidence Needed

Record the next missing evidence.
```

---

## 2026-07-27 - Tree-sitter becomes the production C++ frontend path

### Product Reference

- Release: `v0.3.6`
- Commit: `4e59acf5840b095bd1b11c0c1f0d6d5e7487b25d`
- Pull request: `#141`
- Issue:
- Downstream project:

### Product Change

CJM promoted the Tree-sitter-backed C++ frontend from research spike to the
default production parser path for the current practical syntax surface.

### Research Relevance

- Related research question: RQ1 - Frontend Correctness and Safety
- Relevant hypothesis: A strict adapter over an error-recovering parser can
  preserve CJM's fail-closed managed-declaration contract.

### Observation

The release notes state that the user-facing CLI and CMake workflow remained
unchanged while parser implementation moved to Tree-sitter. The release also
records fail-closed behavior for unsupported managed declarations and retention
of generated-output compatibility.

### Evidence

- [v0.3.6 Release Notes](../releases/v0.3.6.md)
- [Tree-sitter Frontend Recommendation](../design/tree-sitter-frontend-recommendation.md)
- [Tree-sitter Adapter Tests](../../tests/frontends/cxx/tree_sitter/adapter_tests.cpp)
- [Pipeline Golden Tests](../../tests/backends/nlohmann/pipeline_golden_tests.cpp)

### What This Supports

This supports the limited claim that CJM can replace its bootstrap parser with a
Tree-sitter-backed production C++ frontend while preserving the current public
workflow and tested generated-output behavior.

### What This Does Not Prove

This does not prove that Tree-sitter is compiler-equivalent, that CJM supports
full C++, or that the strict adapter is safer than all alternative frontend
designs.

### Follow-Up Evidence Needed

- a versioned malformed and unsupported declaration corpus;
- a naive Tree-sitter adapter baseline;
- measured false-acceptance and false-rejection rates;
- downstream dogfood after parser upgrades.

---

## 2026-07-27 - Metadata IR separates C++ frontend syntax from nlohmann backend output

### Product Reference

- Release: latest verified release `v0.3.6`
- Commit: current `main` commit `2237dc5b6e22a45ec35181b9cb2501f642a93d86`
- Pull request:
- Issue:
- Downstream project:

### Product Change

The repository layout separates Metadata IR, C++ frontend code, and nlohmann
backend code into distinct directories.

### Research Relevance

- Related research question: RQ2 - Frontend/Backend Extensibility
- Relevant hypothesis: A shared Metadata IR can reduce coupling between source
  understanding and backend-specific generated integrations.

### Observation

The Metadata IR is defined in `src/core/ir/model.hpp`. The C++ parser and
semantic layers live under `src/frontends/cxx/`. The implemented backend lives
under `src/backends/nlohmann/`.

### Evidence

- [Metadata IR](../../src/core/ir/model.hpp)
- [C++ Parser Frontend](../../src/frontends/cxx/parser/)
- [C++ Semantic Analysis](../../src/frontends/cxx/semantic/)
- [Tree-sitter Adapter](../../src/frontends/cxx/tree_sitter/)
- [nlohmann Backend](../../src/backends/nlohmann/)
- [Architecture](../../ARCHITECTURE.md)

### What This Supports

This supports the limited claim that CJM's current source tree has an explicit
architectural boundary between frontend syntax handling, Metadata IR, and the
implemented nlohmann backend.

### What This Does Not Prove

This does not prove that adding a C frontend or JSON Schema backend will be
low-cost. It also does not prove `N + M` scaling behavior.

### Follow-Up Evidence Needed

- product-driven implementation of a second frontend or backend;
- dependency-edge measurements before and after that implementation;
- comparison against direct frontend-to-backend generation.

---

## 2026-07-27 - ull-md-engine consumes CJM through the public CMake workflow

### Product Reference

- Release: `v0.3.0`
- Commit:
- Pull request:
- Issue:
- Downstream project: [lmingzhi618/ull-md-engine](https://github.com/lmingzhi618/ull-md-engine)

### Product Change

The `ull-md-engine` downstream project consumed CJM through the documented
`FetchContent` and `cjm_generate(...)` workflow and used generated
`nlohmann/json` integration for practical model types.

### Research Relevance

- Related research question: RQ4 - Practicality
- Relevant hypothesis: CJM's build-time generation workflow can be practical in
  a real downstream CMake project.

### Observation

The dogfood report records two downstream passes: a JSON logging smoke test and
a broader practical mapping surface test. Both completed successfully in the
downstream project.

### Evidence

- [ull-md-engine Dogfood Report](../dogfood/ull-md-engine-v0.3.0.md)
- [v0.3.0 Release Notes](../releases/v0.3.0.md)

### What This Supports

This supports the limited claim that CJM v0.3.0 could be consumed by one real
downstream CMake project for a small logging model and a broader practical
mapping-surface model.

### What This Does Not Prove

This does not prove broad adoption, universal downstream compatibility, or that
CJM has production maturity across many projects.

### Follow-Up Evidence Needed

- additional downstream projects;
- issue reports from external users;
- repeated dogfood after CMake and generated-contract changes;
- documented configure/build friction measurements.

---

## 2026-07-27 - Golden tests preserve deterministic generated nlohmann output

### Product Reference

- Release: latest verified release `v0.3.6`
- Commit: current `main` commit `2237dc5b6e22a45ec35181b9cb2501f642a93d86`
- Pull request:
- Issue:
- Downstream project:

### Product Change

The test suite includes golden tests for generated `nlohmann/json` integration
output.

### Research Relevance

- Related research question: RQ4 - Practicality
- Relevant hypothesis: readable and deterministic generated output is a
  practical advantage of CJM's build-time approach.

### Observation

Golden files under `tests/golden/` are compared against generated output by the
generator and pipeline tests.

### Evidence

- [Golden Files](../../tests/golden/)
- [Generator Tests](../../tests/backends/nlohmann/cpp_generator_tests.cpp)
- [Pipeline Golden Tests](../../tests/backends/nlohmann/pipeline_golden_tests.cpp)

### What This Supports

This supports the limited claim that CJM treats generated output as a stable,
testable artifact.

### What This Does Not Prove

This does not prove that all future backends will be deterministic or that
generated output is optimal for every user's style preferences.

### Follow-Up Evidence Needed

- golden tests for future generated model contracts;
- golden tests for future JSON Schema output if implemented;
- formatting and readability evaluation across larger model sets.
