---
title: Term
layout: docs
---

<div class="bottom_right_note">Page generated at 2012-06-08 17:12</div>
<h3><span class="minor">type</span> Term</h3>

<p>A single unit of code. Each Term has a function, and 0 or more input Terms. A Term may have a local name. Every term has a Branch parent, and a term may optionally have a nested Branch.</p>
<p>Methods:</p>
<p><a href="/docs/Term.assign.html">Term.assign</a>(_, any)</p>
<p><a href="/docs/Term.asint.html">Term.asint</a>(_) -> int</p>
<p><a href="/docs/Term.asfloat.html">Term.asfloat</a>(_) -> number</p>
<p><a href="/docs/Term.parent.html">Term.parent</a>(_) -> Branch</p>
<p><a href="/docs/Term.contents.html">Term.contents</a>(_) -> Branch</p>
<p><a href="/docs/Term.name.html">Term.name</a>(self) -> String</p>
<p><a href="/docs/Term.to_string.html">Term.to_string</a>(_) -> String</p>
<p><a href="/docs/Term.to_source_string.html">Term.to_source_string</a>(_) -> String</p>
<p><a href="/docs/Term.function.html">Term.function</a>(_) -> Branch</p>
<p><a href="/docs/Term.get_type.html">Term.get_type</a>(_) -> Type</p>
<p><a href="/docs/Term.value.html">Term.value</a>(_) -> any</p>
<p><a href="/docs/Term.tweak.html">Term.tweak</a>(_, number steps)</p>
<p><a href="/docs/Term.input.html">Term.input</a>(_, int) -> Term</p>
<p><a href="/docs/Term.inputs.html">Term.inputs</a>(_) -> List</p>
<p><a href="/docs/Term.num_inputs.html">Term.num_inputs</a>(_) -> int</p>
<p><a href="/docs/Term.source_location.html">Term.source_location</a>(_) -> any</p>
<p><a href="/docs/Term.location_string.html">Term.location_string</a>(_) -> String</p>
<p><a href="/docs/Term.global_id.html">Term.global_id</a>(_) -> int</p>
<p><a href="/docs/Term.properties.html">Term.properties</a>(_) -> Map</p>
<p><a href="/docs/Term.property.html">Term.property</a>(_, String) -> any</p>
<p><a href="/docs/Term.is_function.html">Term.is_function</a>(self) -> bool</p>
<p><a href="/docs/Term.is_type.html">Term.is_type</a>(self) -> bool</p>
<p><a href="/docs/Term.is_input_placeholder.html">Term.is_input_placeholder</a>(self) -> bool</p>
<p><a href="/docs/Term.is_comment.html">Term.is_comment</a>(self) -> bool</p>

<p><span class="extra_minor">Term#11</span></p>
