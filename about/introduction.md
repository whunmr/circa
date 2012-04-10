---
title: "Introduction"
description: ""
layout: about
author: Andrew Fischer
---

# Introduction

Circa is a language designed for live coding. We've built the language from scratch
to be a fun, productive language that allows the coder to see the
effects of their code in realtime. The syntax will be familiar to users
of modern scripting languages like Python or Ruby. We hope that
Circa will be a language of choice for the creative coder.

# Current status

It's important to note that Circa is currently alpha-level, and the implementation
has known issues. Read about the <a href="/about/current_status.html">current status</a>.

# Features

## Reload code, preserve state

Most interpreted languages can already support the loading of new code at runtime,
but one common problem is how to preserve the program's current state.
Circa solves this problem by making state a first-class
entity in the language.
See the article on <a href="about/inlined_state.html">inlined state</a> for more information.

## Rich reflection API

Code objects are stored in a simple and normalized format, making reflection and
code modification easy. Automatic code generation is encouraged.

## Introspectable

The interpreter is designed to be heavily introspectable, so you can poke at the
internals and see what is going on. We were heavily inspired by Smalltalk's abilities here.

## Embeddable

Written in C++ and accessible with a C API, our goal is to be Almost As Embeddable As Lua.

## Open source

Source is available on Github, under the liberal MIT license.
