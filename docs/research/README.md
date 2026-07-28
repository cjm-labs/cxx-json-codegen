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

Research is a conditional derivative of product success, not a product
requirement.

- Product usefulness, correctness, and user experience take priority.
- Research questions should emerge from real architecture and downstream use.
- Product features require independent product justification.
- Experimental baselines remain isolated from production code.
- Research documents do not create product commitments.
- Evaluations must identify exact CJM releases and commits.
- Evidence must distinguish observation, inference, and verified conclusion.
- Negative results must be preserved rather than hidden.

---

# Research Investment Policy

CJM preserves research optionality, but it does not assume that a formal paper
will be written.

Low-cost evidence preservation is part of normal product development because it
also improves architecture review, testing, documentation, and reproducibility.

High-cost research work begins only after explicit go/no-go review.

Publication, including publication in a top conference or journal, is a
possible bonus rather than a product milestone or project KPI.

CJM success does not depend on publication.

A successful product with no paper is still a successful outcome.

A rejected research hypothesis does not invalidate the product.

A negative research result must be preserved honestly.

No venue should be selected before the evidence and paper contribution are
clear.

---

# Research ROI

Research return should be evaluated in three separate categories.

## Product ROI

Product ROI remains the primary return.

Benefits include:

- improved correctness;
- stronger tests;
- better documentation;
- clearer architecture;
- better diagnostics;
- better maintenance history;
- external user trust;
- long-term open-source value.

## Option-Value ROI

Low-cost evidence preservation creates the option to write a future paper
without forcing that decision today.

This includes:

- versioned evidence;
- historical comparisons;
- preserved regressions;
- exact frontend and backend change records;
- real downstream cases.

This is currently the highest-ROI research activity.

## Publication ROI

Publication ROI is unknown until the product, pilot, evaluation, related-work
analysis, and manuscript are substantially clearer.

Possible benefits include:

- academic credibility;
- PhD application evidence;
- research-oriented career value;
- compiler and software-engineering visibility;
- formal dissemination of generalizable results.

Costs may include:

- experiment engineering;
- baseline implementation;
- data curation;
- statistical work;
- writing and revision;
- uncertain review outcomes.

Publication ROI should be evaluated only after a bounded pilot.

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
