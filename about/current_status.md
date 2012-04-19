---
title: "Current Status"
description: ""
layout: about
author: Andrew Fischer
---

# Current status

<div class="article_credit">last updated April 14, 2012 - Andrew Fischer</div>

The latest version is alpha-4. This version is not recommended for public consumption. The language and API may change in non-backwards-compatible ways.


### Command-line tool

The command-line tool is feature complete; this tool can load & run a script file, and run a REPL. This tool has been the primary method of testing & developing the core language.

### Graphical environment

The Qt-based GUI is in progress, about 50% complete. This is intended to be a Processing-like tool that comes
equipped with bindings for drawing, doing audio, and handling user input.

### Visual editor

We want to have a tool (or many) that takes advantage of Circa's abilities to modify
and introspect the system at runtime. The user will be able to create and modify code without
ever touching a text file. This one is very early in development.

### Embeddability

The public C api is almost ready for a release, this will be released alongside the GUI tool.

## Known issues in the core language
<ul>
<li>Garbage collection is disabled, so GCed objects are never collected. (the good news is that not all types are GCed)</li>
<li>Type inferrence is lacking, especially around container types. Generally, every time you access a list element, you need a manual cast to the expected type. </li>
<li>API and libraries are still in progress.</li>
<li>Performance problems in the interpreter (such as one malloc per function call)</li>
<li>Only one instance of the core library can exist per process (due to use of globals)</li>
<li>Lacking documentation and a spec</li>
</ul>
# Future plans
<ul>
<li>Syntax: Probably require the 'var' keyword in front of new name bindings.</li>
<li>JIT or cross-compilation (for performance).</li>
<li>Implement the <a href="feedback.html">feedback system</a>.</li>
<li>Run in a web browser.</li>
</ul>
