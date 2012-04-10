---
title: "Current Status"
description: ""
layout: about
author: Andrew Fischer
---

<h2>Known issues in alpha-4</h2>
<ul>
<li>Garbage collection is disabled, so GCed objects are never collected. Fortunately not all types are GCed.</li>
<li>Bindings for the reflection API are not complete.</li>
<li>Performance problems in the interpreter</li>
</ul>
<h2>Future plans</h2>
<ul>
<li>Syntax: Probably require the 'var' keyword in front of new name bindings.</li>
<li>JIT or cross-compilation (for performance).</li>
<li>Implement the <a href="feedback.html">feedback system</a>.</li>
<li>Some method for running Circa code in a browser.</li>
</ul>
