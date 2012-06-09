---
title: Reflection
layout: docs
---

<div class="bottom_right_note">Page generated at 2012-06-08 17:12</div>
<p>Functions and types for inspecting and modifying Circa's internals, including the interpreter and code data.</p>
<h5>Types</h5>

<table>
  <tr>
    <td><a href="/docs/Branch.html">Branch</a></td>
  </tr>
  <tr>
    <td><a href="/docs/Frame.html">Frame</a></td>
  </tr>
  <tr>
    <td><a href="/docs/Interpreter.html">Interpreter</a></td>
  </tr>
  <tr>
    <td><a href="/docs/Term.html">Term</a></td>
  </tr>
  <tr>
    <td><a href="/docs/Type.html">Type</a></td>
  </tr>
  <tr>
  </tr>
  <tr>
  </tr>
  <tr>
  </tr>
  <tr>
  </tr>
  <tr>
  </tr>
</table>


<h5>Functions</h5>

<table>
  <tr>
    <td><a href="/docs/Branch.call.html">Branch.call</a></td>
    <td><a href="/docs/Branch.outputs.html">Branch.outputs</a></td>
    <td><a href="/docs/Interpreter.run.html">Interpreter.run</a></td>
    <td><a href="/docs/Term.is_input_placeholder.html">Term.is_input_placeholder</a></td>
  </tr>
  <tr>
    <td><a href="/docs/Branch.dump.html">Branch.dump</a></td>
    <td><a href="/docs/Branch.owner.html">Branch.owner</a></td>
    <td><a href="/docs/Interpreter.run_steps.html">Interpreter.run_steps</a></td>
    <td><a href="/docs/Term.is_type.html">Term.is_type</a></td>
  </tr>
  <tr>
    <td><a href="/docs/Branch.file_signature.html">Branch.file_signature</a></td>
    <td><a href="/docs/Branch.statements.html">Branch.statements</a></td>
    <td><a href="/docs/Interpreter.set_state_input.html">Interpreter.set_state_input</a></td>
    <td><a href="/docs/Term.location_string.html">Term.location_string</a></td>
  </tr>
  <tr>
    <td><a href="/docs/Branch.find_term.html">Branch.find_term</a></td>
    <td><a href="/docs/Branch.terms.html">Branch.terms</a></td>
    <td><a href="/docs/Interpreter.top_frame.html">Interpreter.top_frame</a></td>
    <td><a href="/docs/Term.name.html">Term.name</a></td>
  </tr>
  <tr>
    <td><a href="/docs/Branch.format_function_heading.html">Branch.format_function_heading</a></td>
    <td><a href="/docs/Branch.version.html">Branch.version</a></td>
    <td><a href="/docs/Interpreter.toString.html">Interpreter.toString</a></td>
    <td><a href="/docs/Term.num_inputs.html">Term.num_inputs</a></td>
  </tr>
  <tr>
    <td><a href="/docs/Branch.format_source.html">Branch.format_source</a></td>
    <td><a href="/docs/branch_ref.html">branch_ref</a></td>
    <td><a href="/docs/is_overloaded_func.html">is_overloaded_func</a></td>
    <td><a href="/docs/Term.parent.html">Term.parent</a></td>
  </tr>
  <tr>
    <td><a href="/docs/Branch.functions.html">Branch.functions</a></td>
    <td><a href="/docs/Frame.branch.html">Frame.branch</a></td>
    <td><a href="/docs/lookup_branch_ref.html">lookup_branch_ref</a></td>
    <td><a href="/docs/Term.properties.html">Term.properties</a></td>
  </tr>
  <tr>
    <td><a href="/docs/Branch.get_static_errors.html">Branch.get_static_errors</a></td>
    <td><a href="/docs/Frame.pc.html">Frame.pc</a></td>
    <td><a href="/docs/make_interpreter.html">make_interpreter</a></td>
    <td><a href="/docs/Term.property.html">Term.property</a></td>
  </tr>
  <tr>
    <td><a href="/docs/Branch.get_static_errors_formatted.html">Branch.get_static_errors_formatted</a></td>
    <td><a href="/docs/Frame.pc_term.html">Frame.pc_term</a></td>
    <td><a href="/docs/overload:get_contents.html">overload:get_contents</a></td>
    <td><a href="/docs/Term.source_location.html">Term.source_location</a></td>
  </tr>
  <tr>
    <td><a href="/docs/Branch.get_term.html">Branch.get_term</a></td>
    <td><a href="/docs/Frame.register.html">Frame.register</a></td>
    <td><a href="/docs/Term.asfloat.html">Term.asfloat</a></td>
    <td><a href="/docs/Term.to_source_string.html">Term.to_source_string</a></td>
  </tr>
  <tr>
    <td><a href="/docs/Branch.get_top_comments.html">Branch.get_top_comments</a></td>
    <td><a href="/docs/Frame.registers.html">Frame.registers</a></td>
    <td><a href="/docs/Term.asint.html">Term.asint</a></td>
    <td><a href="/docs/Term.to_string.html">Term.to_string</a></td>
  </tr>
  <tr>
    <td><a href="/docs/Branch.has_static_error.html">Branch.has_static_error</a></td>
    <td><a href="/docs/Interpreter.error_message.html">Interpreter.error_message</a></td>
    <td><a href="/docs/Term.assign.html">Term.assign</a></td>
    <td><a href="/docs/Term.tweak.html">Term.tweak</a></td>
  </tr>
  <tr>
    <td><a href="/docs/Branch.input.html">Branch.input</a></td>
    <td><a href="/docs/Interpreter.errored.html">Interpreter.errored</a></td>
    <td><a href="/docs/Term.contents.html">Term.contents</a></td>
    <td><a href="/docs/Term.value.html">Term.value</a></td>
  </tr>
  <tr>
    <td><a href="/docs/Branch.inputs.html">Branch.inputs</a></td>
    <td><a href="/docs/Interpreter.frame.html">Interpreter.frame</a></td>
    <td><a href="/docs/Term.function.html">Term.function</a></td>
    <td><a href="/docs/term_ref.html">term_ref</a></td>
  </tr>
  <tr>
    <td><a href="/docs/Branch.is_method.html">Branch.is_method</a></td>
    <td><a href="/docs/Interpreter.frames.html">Interpreter.frames</a></td>
    <td><a href="/docs/Term.get_type.html">Term.get_type</a></td>
    <td><a href="/docs/Type.declaringTerm.html">Type.declaringTerm</a></td>
  </tr>
  <tr>
    <td><a href="/docs/Branch.is_null.html">Branch.is_null</a></td>
    <td><a href="/docs/Interpreter.get_state_output.html">Interpreter.get_state_output</a></td>
    <td><a href="/docs/Term.global_id.html">Term.global_id</a></td>
    <td><a href="/docs/Type.is_handle.html">Type.is_handle</a></td>
  </tr>
  <tr>
    <td><a href="/docs/Branch.link.html">Branch.link</a></td>
    <td><a href="/docs/Interpreter.output.html">Interpreter.output</a></td>
    <td><a href="/docs/Term.input.html">Term.input</a></td>
    <td><a href="/docs/Type.name.html">Type.name</a></td>
  </tr>
  <tr>
    <td><a href="/docs/Branch.list_configs.html">Branch.list_configs</a></td>
    <td><a href="/docs/Interpreter.pop_frame.html">Interpreter.pop_frame</a></td>
    <td><a href="/docs/Term.inputs.html">Term.inputs</a></td>
    <td><a href="/docs/Type.property.html">Type.property</a></td>
  </tr>
  <tr>
    <td><a href="/docs/Branch.name.html">Branch.name</a></td>
    <td><a href="/docs/Interpreter.push_frame.html">Interpreter.push_frame</a></td>
    <td><a href="/docs/Term.is_comment.html">Term.is_comment</a></td>
  </tr>
  <tr>
    <td><a href="/docs/Branch.output.html">Branch.output</a></td>
    <td><a href="/docs/Interpreter.reset.html">Interpreter.reset</a></td>
    <td><a href="/docs/Term.is_function.html">Term.is_function</a></td>
  </tr>
</table>


