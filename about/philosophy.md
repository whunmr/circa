---
layout: page
title: "Philosophy"
description: ""
layout: about
author: Andrew Fischer
---

# Philosophy

Every good language needs a philosophy, and here is ours.

### By making easy things trivial, we make nearly-impossible things merely hard

Lots of features in Circa could be accused of being a whole lot of (our) work for a small
amount of gain. But every little distraction that we can remove means that the coder
can give slightly more attention to the real problems.

### Code is mutable data.

Even source text files are fair game to be modified by the system.

### Always know where values came from.

In order to accomplish many of the features we want, such as modifying anything at runtime,
and results-based feedback, we need to know how each value was generated. This means
that the system should retain information about data-flow, and it should discourage or eliminate
mutable state.

### Limiting the language is good.

By limiting the language we mean discouraging extra layers of expression, like
inheritance, generic types, or arbitrary control flow. A limited language is
easier to statically reason about, this makes introspection and reflection drastically
easier. Turing-completeness is overrated.

### Tooling and reflection can help overcome the problems of a limited language.

Instead of giving the coder more ways to express their logic, we can
make it easier to create code with the available language.

### Minimize the need for planning.

Too often, software is like architecture, where the whole thing must be thrown away
if the foundations are built incorrectly.
