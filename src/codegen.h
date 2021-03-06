// Copyright (c) Andrew Fischer. See LICENSE file for license terms.

// codegen.cpp
//
// Classes that take compiled Circa code and generate equivalent code in another
// language.
//

#pragma once

#include "common_headers.h"

namespace circa {

std::string generate_cpp_headers(Block* block);

} // namespace circa
