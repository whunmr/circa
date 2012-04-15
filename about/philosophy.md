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

We go to great lengths to save the user from doing tasks that would only take
them a few extra seconds. But, each distraction we can remove gives the user more
attention for the real problems.

### Code is mutable data.

Even source text files are fair game to be modified by the system. In a sane way, of course.

### Evaluations should be repeatable and understandable

The system should retain data-flow information, so that we can properly react to arbitrary
runtime changes. Any evaluation that is non-repeatable is a potential problem.

### Limiting the language is good.

By limiting the language we mean discouraging extra layers of expression, like
inheritance, operator overloading, or arbitrary control flow. A limited language is
easier to statically reason about, this makes introspection and reflection drastically
easier. Turing-completeness is overrated!

### Tooling and reflection can help overcome the problems of a limited language.

Instead of giving the coder more ways to express their logic, we can
make it easier to create code with the available language.

### Minimize the need for planning.

Too often, software is like architecture, where the whole thing must be thrown away
if the foundations are built incorrectly. This causes the coder to hesitate before putting
new ideas into code. We prefer an environment where the author
can write code with abandon, and fix any problems later.
