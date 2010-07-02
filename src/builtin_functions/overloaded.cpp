// Copyright (c) 2007-2010 Paul Hodge. All rights reserved.

#include "circa.h"

namespace circa {
namespace overloaded_function {

    bool is_overloaded_function(Term* func);

    CA_FUNCTION(evaluate_overloaded)
    {
        Term* func = FUNCTION;
        List& overloads = function_t::get_attrs(func).parameters;

        // Dynamically find the right overload.
        for (int i=0; i < overloads.length(); i++) {
            Term* overload = as_ref(overloads[i]);

            if (inputs_fit_function_dynamic(overload, INPUTS)) {
                change_type(OUTPUT, type_contents(function_t::get_output_type(overload)));
                evaluate_term(CONTEXT, CALLER, overload, INPUTS, OUTPUT);
                return;
            }
        }

        error_occurred(CONTEXT, CALLER, "No usable overload found");
    }

    Term* statically_specialize_function(Term* func, RefList const& inputs)
    {
        if (!is_function(func))
            return func;
        if (!is_overloaded_function(func))
            return func;

        List& overloads = function_t::get_attrs(func).parameters;

        for (int i=0; i < overloads.length(); i++) {
            Term* overload = as_ref(overloads[i]);

            if (inputs_statically_fit_function(overload, inputs))
                return overload;
        }

        // no overload found
        return func;
    }

    bool is_overloaded_function(Term* func)
    {
        assert(is_function(func));
        return function_t::get_attrs(func).evaluate == evaluate_overloaded;
    }

    int num_overloads(Term* func)
    {
        return function_t::get_attrs(func).parameters.length();
    }

    Term* get_overload(Term* func, int index)
    {
        return function_t::get_attrs(func).parameters[index]->asRef();
    }

    Term* find_overload(Term* func, const char* name)
    {
        for (int i=0; i < num_overloads(func); i++) {
            Term* overload = get_overload(func, i);
            if (overload->name == name)
                return overload;
        }
        return NULL;
    }

    void setup_overloaded_function(Term* term, std::string const& name,
            RefList const& overloads)
    {
        function_t::set_name(term, name);
        function_t::get_attrs(term).evaluate = evaluate_overloaded;

        List& parameters = function_t::get_attrs(term).parameters;
        parameters.clear();
        parameters.resize(overloads.length());

        assert(overloads.length() > 0);
        int argumentCount = function_t::num_inputs(overloads[0]);
        bool variableArgs = false;
        RefList outputTypes;

        for (int i=0; i < overloads.length(); i++) {
            make_ref(parameters[i], overloads[i]);

            if (argumentCount != function_t::num_inputs(overloads[i]))
                variableArgs = true;
            if (function_t::get_variable_args(overloads[i]))
                variableArgs = true;
            outputTypes.append(function_t::get_output_type(overloads[i]));
        }


        Branch& result = as_branch(term);
        result.shorten(1);
        for (int i=0; i < argumentCount; i++)
            apply(result, INPUT_PLACEHOLDER_FUNC, RefList());
        Term* outputType = find_common_type(outputTypes);
        create_value(result, outputType, "#out");

        function_t::set_variable_args(term, variableArgs);
    }

    CA_FUNCTION(evaluate)
    {
        if (NUM_INPUTS == 0)
            return error_occurred(CONTEXT, CALLER, "Number of inputs must be >0");

        setup_overloaded_function(CALLER, CALLER->name, INPUTS);
    }

    Term* create_overloaded_function(Branch& branch, std::string const& name,
        RefList const& overloads)
    {
        Term* result = create_value(branch, FUNCTION_TYPE, name);
        setup_overloaded_function(result, name, overloads);
        return result;
    }

    void append_overload(Term* overloadedFunction, Term* overload)
    {
        assert(is_overloaded_function(overloadedFunction));

        List& parameters = function_t::get_attrs(overloadedFunction).parameters;
        make_ref(parameters.append(), overload);
    }

    void setup(Branch& kernel)
    {
        OVERLOADED_FUNCTION_FUNC = import_function(kernel, evaluate,
                "overloaded_function(Function...) -> Function");
    }
}
}
