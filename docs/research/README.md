# CJM Research Track

This directory records research questions that emerge from CJM's real product
development.

CJM is developed as an open-source product first. Research documents may study,
measure, and evaluate CJM, but they do not define the product roadmap or public
API.

---

# Purpose

The research track preserves product-driven evidence, open research questions,
and future evaluation plans for CJM.

The intended direction is:

```text
Real user problem
        |
        v
CJM product design and implementation
        |
        v
Real releases and downstream use
        |
        v
Accumulated engineering evidence
        |
        v
Research questions
        |
        v
Reproducible evaluation
```

The research track should not invert this relationship. CJM should not gain
research-only product code merely to support a desired paper claim.

---

# Product-First Research Policy

CJM is developed as an open-source product first.

Research may study, measure, and evaluate CJM, but it must not override the
product roadmap or introduce features solely to strengthen a paper claim.

- Product usefulness, correctness, and user experience take priority.
- Research questions should emerge from real architecture and downstream use.
- Product features require independent product justification.
- Experimental baselines remain isolated from production code.
- Research documents do not create product commitments.
- Evaluations must identify exact CJM releases and commits.
- Evidence must distinguish observation, inference, and verified conclusion.
- Negative results must be preserved rather than hidden.

---

# Current Research Status

No paper is currently being submitted.

No final research claim has been established.

The current research work is a prospectus and evidence-collection framework.
Product development remains the active priority.

---

# Relationship to Product Documents

Research documentation is subordinate to CJM's product documents.

Authority order:

```text
AGENTS.md
    |
    v
ARCHITECTURE.md
    |
    v
docs/philosophy.md
    |
    v
docs/vision.md
    |
    v
docs/design/
    |
    v
ROADMAP.md
    |
    v
docs/research/
```

Start with:

- [Development Guide](../../AGENTS.md)
- [Architecture](../../ARCHITECTURE.md)
- [Philosophy](../philosophy.md)
- [Vision](../vision.md)
- [Roadmap](../../ROADMAP.md)
- [Design Notes](../design/)

Research documents may record hypotheses and evidence requirements. They must
not redefine CJM's product identity, supported syntax, public API, or release
criteria.

---

# Document Index

- [Research Prospectus](research-prospectus.md)
- [Evaluation Plan](evaluation-plan.md)
- [Evidence Log](evidence-log.md)

---

# Repository Strategy

Current location:

```text
docs/research/
```

This keeps early research planning close to the product facts it studies.

A separate repository may be useful later:

```text
cjm-labs/cjm-research
```

That repository should be created only when there is substantial content that
does not belong in a product checkout, such as:

- paper source
- malformed or unsupported source corpora
- baseline implementations
- benchmark harnesses
- raw measurements
- analysis scripts
- figures
- Docker or reproducibility environments
- artifact-evaluation packages

The future research repository should pin exact CJM releases and commits,
isolate experiment-only dependencies, preserve raw data, and avoid becoming a
runtime or build dependency of CJM.
