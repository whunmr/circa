// Copyright 2008 Andrew Fischer

#include "circa.h"

namespace circa {
namespace list_get_element_function {

    void evaluate(Term* caller)
    {
        List& list = as<List>(caller->input(0));
        int index = as_int(caller->input(1));

        change_type(caller, list[index]->type);
        duplicate_value(list[index], caller);
    }

    void setup(Branch& kernel)
    {
        Term* main_func = import_c_function(kernel, evaluate,
                "function list-get-element(List, int) -> any");
        as_function(main_func).pureFunction = true;
    }
}
}
