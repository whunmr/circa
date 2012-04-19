---
title: "Introduction"
description: ""
layout: about
author: Andrew Fischer
---

# Introduction

Circa is a language designed for live coding. We've built the language from scratch
to be a fun, productive language that allows the coder to see the
effects of their code in realtime. The runtime takes inspiration from Lisp and
Smalltalk, where code is mutable data and the interpreter is heavily introspectable.
The syntax is similar to Python and Ruby, but under the
familiar syntax is a language with static typing, tamed side effects, and a few
interesting tricks. We hope that Circa will be a
language of choice for the creative coder.

# Current status

It's important to note that Circa is currently alpha-level, and the implementation
has known issues. There aren't any downloadable binaries ready yet, but we're working
on that. Read more about the <a href="/about/current_status.html">current status</a>.

# Features

## Reload code, preserve state

Most interpreted languages can already support the loading of new code at runtime,
but one common problem is how to preserve the program's current state. In some
existing solutions, the only code that can be reloaded is stateless.
Circa solves the problem by making state a first-class
entity in the language, and it knows how to preserve it across a reload. Often
one can write an entire program without ever restarting.
See the article on <a href="/about/inlined_state.html">inlined state</a> for more information.

## Metaprogramming and reflection

Compiled code is stored in a simple and normalized format, making reflection and
code modification easy. A rich reflection API is built-in.
Automatic code generation and refactoring is encouraged.

## Introspectable

The interpreter is designed to be heavily introspectable, so you can poke at the
internals and understand what it's doing, why it's doing it, and what it will do next.

## Hybrid textual and visual programming

Code can be edited as text, or edited as a structured AST graph. Changes to the AST can
be saved back to source text with no loss of comments or whitespace.

## Designed for productivity

Here's a laundry list of design decisions:

 * Runtime typing with static type inferrence
 * Eager evaluation
 * Call-by-value
 * Values are immutable

## Embeddable

Accessible with a clean C API, one of the goals is to be Almost As Embeddable As Lua.

## Open source

Source is available on <a href="https://github.com/paulhodge/circa">Github</a>, under the liberal MIT license.

# Author

This project is a labor of love by Andrew Fischer, who has written C++ for so many years that he never wants to stop to recompile ever again. Found on Github as <a href="https://github.com/paulhodge">paulhodge</a> and on Twitter as <a href="https://twitter.com/#!/hodgepaul">hodgepaul</a>.

Logo designed by Kurt Schafer.
