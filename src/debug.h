// Copyright (c) Paul Hodge. See LICENSE file for license terms.

// Various code for debugging purposes. This code shouldn't have an effect on a
// release build.

#pragma once

#include "common_headers.h"

namespace circa {

// Setting this to true will make us abort trap on the next name lookup.
extern bool DEBUG_TRAP_NAME_LOOKUP;

// Setting this to true will make us abort trap on the next call to error_occurred()
extern bool DEBUG_TRAP_ERROR_OCCURRED;

// Spit out this branch's raw contents to std::cout
void dump(Branch& branch);
void dump_with_props(Branch& branch);

void dump(Value& value);
void dump(Value* value);

} // namespace circa
