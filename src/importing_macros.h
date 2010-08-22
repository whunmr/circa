// Copyright (c) 2007-2010 Paul Hodge. All rights reserved.

#pragma once

#include "common_headers.h"

#define CA_START_FUNCTIONS \
    struct _circa_StaticFuncDeclaration; \
    std::vector<_circa_StaticFuncDeclaration*> _circa_START_FUNCTIONS; \
    struct _circa_StaticFuncDeclaration { \
        const char* _header; \
        circa::EvaluateFunc _func; \
        _circa_StaticFuncDeclaration(const char* header, circa::EvaluateFunc func) \
            : _header(header), _func(func) \
        { _circa_START_FUNCTIONS.push_back(this); } \
    };



#define CA_DEFINE_FUNCTION(fname, header) \
    CA_FUNCTION(evaluate_##fname); \
    static _circa_StaticFuncDeclaration _static_decl_for_##fname(header, evaluate_##fname); \
    CA_FUNCTION(evaluate_##fname)

#define EVALUATION_ARGS _circa_cxt, _circa_caller, _circa_func, _circa_inputs, _circa_output

#define INPUT(index) (_circa_stack->getIndex(_circa_op->inputs[index].stackIndex))
#define FLOAT_INPUT(index) circa::to_float(INPUT(index))
#define BOOL_INPUT(index) circa::as_bool(INPUT(index))
#define STRING_INPUT(index) circa::as_string(INPUT(index)).c_str()
#define INT_INPUT(index) circa::as_int(INPUT(index))
#define NUM_INPUTS (_circa_op->numInputs)
#define CALLER (_circa_op->caller)
#define INPUT_TERM(index) (CALLER->input(index))
#define CAPTURE_INPUTS(list) (capture_inputs(_circa_stack, _circa_op, (list)))
//#define INPUTS (_circa_inputs)
#define OUTPUT (_circa_stack->getIndex(_circa_op->outputIndex))
#define CONTEXT (_circa_cxt)
#define FUNCTION (_circa_op->function)
#define CONTEXT_AND_CALLER CONTEXT, CALLER


#define CA_SETUP_FUNCTIONS(branch) {\
    for (size_t i=0; i < _circa_START_FUNCTIONS.size(); i++) \
        import_function(branch, _circa_START_FUNCTIONS[i]->_func,\
                _circa_START_FUNCTIONS[i]->_header);\
    }
