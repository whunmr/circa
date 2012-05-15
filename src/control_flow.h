// Copyright (c) Andrew Fischer. See LICENSE file for license terms.

#pragma once

namespace circa {

// Evaluation helpers
void early_finish_frame(caStack* stack, Frame* frame);
void control_flow_setup_funcs(Branch* kernel);

Term* find_intermediate_result_for_output(Term* location, Term* output);
void update_exit_points(Branch* branch);

} // namespace circa
