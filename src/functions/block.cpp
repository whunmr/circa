// Copyright (c) Andrew Fischer. See LICENSE file for license terms.

#include "circa/internal/for_hosted_funcs.h"

namespace circa {
namespace block_function {

    void format_source(caValue* source, Term* term)
    {
        format_name_binding(source, term);
        format_block_source(source, nested_contents(term), term);
    }

    void setup(Block* kernel)
    {
        // deprecated
        FUNCS.block_unevaluated = import_function(kernel, NULL, "block_unevaluated()");
        block_set_evaluation_empty(function_contents(FUNCS.block_unevaluated), true);

        // deprecated
        FUNCS.lambda = import_function(kernel, NULL, "lambda(any inputs :multiple) -> Block");
        as_function(FUNCS.lambda)->formatSource = format_source;
    }
}
}
