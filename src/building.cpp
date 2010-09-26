// Copyright (c) 2007-2010 Paul Hodge. All rights reserved.

#include "circa.h"
#include "debug_valid_objects.h"
#include "names.h"

namespace circa {

Term* apply(Branch& branch, Term* function, RefList const& inputs, std::string const& name)
{
    ca_assert(function != NULL);

    // If 'function' is actually a type, create a value instead.
    if (is_type(function)) {
        if (inputs.length() == 0) {
            Term* result = create_value(branch, function);
            result->setBoolProp("constructor", true);
            return result;
        } else if (inputs.length() == 1) {
            Term* result = apply(branch, CAST_FUNC, inputs);
            change_type(result, function);
            return result;
        } else {
            throw std::runtime_error("Constructors with multiple arguments not yet supported.");
        }
    }

    if (!is_callable(function))
        throw std::runtime_error("Term "+function->name+" is not callable");

    // Create the term
    Term* result = branch.appendNew();

    result->function = function;

    if (name != "")
        branch.bindName(result, name);

    // Initialize inputs
    for (int i=0; i < inputs.length(); i++)
        set_input(result, i, inputs[i]);

    Term* outputType = function_get_specialized_output_type(function, result);

    ca_assert(outputType != NULL);
    ca_assert(is_type(outputType));

    change_type(result, outputType);

    post_input_change(result);

    return result;
}

void set_input(Term* term, int index, Term* input)
{
    assert_valid_term(term);

    Ref previousInput = NULL;
    if (index < term->numInputs())
        previousInput = term->input(index);

    term->inputs.setAt(index, input);

    if (index >= term->inputInfoList.length())
        term->inputInfoList.resize(index+1);

    // Add 'term' to the user list of input
    if (input != NULL && term != input)
        input->users.appendUnique(term);

    // Check if we should remove 'term' from the user list of previousInput
    if (previousInput != NULL && !is_actually_using(previousInput, term))
        previousInput->users.remove(term);

    post_input_change(term);
}

void set_inputs(Term* term, RefList const& inputs)
{
    assert_valid_term(term);

    RefList previousInputs = term->inputs;

    term->inputs = inputs;

    term->inputInfoList.resize(inputs.length());

    // Add 'term' as a user to these new inputs
    for (int i=0; i < inputs.length(); i++)
        if (inputs[i] != NULL)
            inputs[i]->users.appendUnique(term);

    // Check to remove 'term' from user list of any previous inputs
    for (int i=0; i < previousInputs.length(); i++) {
        Term* previousInput = previousInputs[i];
        if (previousInput != NULL && !is_actually_using(previousInput, term))
            previousInput->users.remove(term);
    }

    post_input_change(term);
}

void update_input_info(Term* term, int index)
{
    InputInfo& info = term->inputInfo(index);
    Term* inputTerm = term->input(index);

    if (inputTerm == NULL) {
        info.relativeScope = -1;
        return;
    }

    // Find the first common branch.
    Branch* commonBranch = find_first_common_branch(term, inputTerm);

    // Find the distance from the term to the common branch, this is
    // the relativeScope distance.
    info.relativeScope = 0;
    Term* walk = term;
    while (commonBranch->owningTerm != walk) {

        // In certain cases, we don't count a scope layer.
        bool countLayer = true;
        if (walk->function == IF_BLOCK_FUNC)
            countLayer = false;
        else if (walk->name == "#inner_rebinds")
            countLayer = false;
        else if (walk->name == "#outer_rebinds")
            countLayer = false;

        if (countLayer)
            info.relativeScope++;

        walk = get_parent_term(walk);
    }
    info.relativeScope--;

    // Special case for joining terms
    if (inputTerm->function == JOIN_FUNC) {
        Term* enclosingBlock = get_parent_term(get_parent_term(inputTerm));
        info.registerIndex = enclosingBlock->registerIndex + 1 + inputTerm->index;
    } else {
        info.registerIndex = inputTerm->registerIndex;
    }

    #if 0

    //std::cout << "common branch: " << std::endl;
    //dump_branch(*commonBranch);

    if (commonBranch == NULL)
        internal_error("No common branch in update_input_info");


    // Find the distance from the inputTerm to common branch; this is usually 1, but
    // it might be greater if the input is nested in a namespace or something.
    int nestedStepCount = 0;
    walk = inputTerm;
    while (commonBranch->owningTerm != walk) {
        if (include_step_in_relative_input_location(walk))
            nestedStepCount++;
        walk = get_parent_term(walk);
    }

    info.setNestedStepCount(nestedStepCount);

    // Walk from inputTerm to common branch again, this time we record the indexes used.
    walk = inputTerm;
    int i = nestedStepCount-1;
    while (commonBranch->owningTerm != walk) {
        if (include_step_in_relative_input_location(walk))
            info.steps[i--].index = walk->index;
        walk = get_parent_term(walk);
    }

    // Join terms are not evaluated until their stack frame is gone, so account for that
    // here.
    if (term->function == JOIN_FUNC)
        info.relativeScope--;
    #endif
}

void post_input_change(Term* term)
{
    for (int i=0; i < term->numInputs(); i++)
        update_input_info(term, i);

    FunctionAttrs::PostInputChange func = function_t::get_attrs(term->function).postInputChange;
    if (func) {
        func(term);
    }
}

void update_register_indices(Branch& branch)
{
    int next = 0;

    for (int i=0; i < branch.length(); i++) {
        Term* term = branch[i];

        term->registerIndex = next;

        int registerCount = 1;

        FunctionAttrs::GetRegisterCount getRegisterCount
            = function_t::get_attrs(term->function).getRegisterCount;

        if (getRegisterCount != NULL)
            registerCount = getRegisterCount(term);

        next += registerCount;
    }

    branch.registerCount = next;
}

bool is_actually_using(Term* user, Term* usee)
{
    for (int i=0; i < usee->numInputs(); i++)
        if (usee->input(i) == user)
            return true;

    return false;
}

void clear_all_users(Term* term)
{
    for (int i=0; i < term->numInputs(); i++) {
        Term* user = term->input(i);
        if (user == NULL) continue;
        user->users.remove(term);
    }
}

Term* create_duplicate(Branch& branch, Term* original, std::string const& name, bool copyBranches)
{
    ca_assert(original != NULL);

    Term* term = apply(branch, original->function, original->inputs, name);
    change_type(term, original->type);
    change_type((TaggedValue*) term, original->value_type);

    if (copyBranches || !is_branch(original))
        copy(original, term);

    if (copyBranches)
        duplicate_branch(original->nestedContents, term->nestedContents);

    term->sourceLoc = original->sourceLoc;
    duplicate_branch(original->properties, term->properties);

    return term;
}

std::string default_name_for_hidden_state(const std::string& termName)
{
    std::stringstream new_value_name;
    new_value_name << "#hidden_state";
    if (termName != "") new_value_name << "_for_" << termName;
    return new_value_name.str();
}

Term* apply(Branch& branch, std::string const& functionName, RefList const& inputs, std::string const& name)
{
    Term* function = find_named(branch, functionName);
    if (function == NULL)
        throw std::runtime_error("function not found: "+functionName);

    Term* result = apply(branch, function, inputs, name);
    result->setStringProp("syntax:functionName", functionName);
    return result;
}

Term* create_value(Branch& branch, Term* type, std::string const& name)
{
    // This function is safe to call while bootstrapping.
    ca_assert(type != NULL);
    ca_assert(is_type(type));

    Term *term = branch.appendNew();

    if (name != "")
        branch.bindName(term, name);

    term->function = VALUE_FUNC;
    term->type = type;
    change_type(term, type);
    reset(term);

    return term;
}

Term* create_value(Branch& branch, std::string const& typeName, std::string const& name)
{
    Term* type = NULL;

    type = find_named(branch, typeName);

    if (type == NULL)
        throw std::runtime_error("Couldn't find type: "+typeName);

    return create_value(branch, type, name);
}

Term* create_stateful_value(Branch& branch, Term* type, Term* defaultValue,
        std::string const& name)
{
    Term* result = apply(branch, get_global("get_state_field"),
            RefList(NULL, NULL, defaultValue), name);
    change_type(result, type);
    return result;
}

Term* create_string(Branch& branch, std::string const& s, std::string const& name)
{
    Term* term = create_value(branch, STRING_TYPE, name);
    make_string(term, s);
    return term;
}

Term* create_int(Branch& branch, int i, std::string const& name)
{
    Term* term = create_value(branch, INT_TYPE, name);
    make_int(term, i);
    return term;
}

Term* create_float(Branch& branch, float f, std::string const& name)
{
    Term* term = create_value(branch, FLOAT_TYPE, name);
    make_float(term, f);
    return term;
}

Term* create_bool(Branch& branch, bool b, std::string const& name)
{
    Term* term = create_value(branch, BOOL_TYPE, name);
    make_bool(term, b);
    return term;
}

Term* create_ref(Branch& branch, Term* ref, std::string const& name)
{
    Term* term = create_value(branch, REF_TYPE, name);
    make_ref(term, ref);
    return term;
}
Term* create_void(Branch& branch, std::string const& name)
{
    return create_value(branch, VOID_TYPE, name);
}

Term* create_list(Branch& branch, std::string const& name)
{
    Term* term = create_value(branch, LIST_TYPE, name);
    return term;
}

Branch& create_branch(Branch& owner, std::string const& name)
{
    return apply(owner, BRANCH_FUNC, RefList(), name)->nestedContents;
}

Branch& create_namespace(Branch& branch, std::string const& name)
{
    return create_value(branch, NAMESPACE_TYPE, name)->nestedContents;
}

Term* create_type(Branch& branch, std::string name)
{
    Term* term = create_value(branch, TYPE_TYPE);

    if (name != "") {
        as_type(term).name = name;
        branch.bindName(term, name);
    }

    return term;
}

Term* create_empty_type(Branch& branch, std::string name)
{
    Term* type = create_type(branch, name);
    return type;
}

Term* create_branch_based_type(Branch& branch, std::string const& name)
{
    Term* term = create_type(branch, name);
    initialize_branch_based_type(term);
    return term;
}

Term* duplicate_value(Branch& branch, Term* term)
{
    Term* dup = create_value(branch, term->type);
    copy(term, dup);
    return dup;
}

Term* procure_value(Branch& branch, Term* type, std::string const& name)
{
    Term* existing = branch[name];
    if (existing == NULL)
        existing = create_value(branch, type, name);
    else
        change_type(existing, type);
    return existing;
}

Term* procure_int(Branch& branch, std::string const& name)
{
    return procure_value(branch, INT_TYPE, name);
}

Term* procure_float(Branch& branch, std::string const& name)
{
    return procure_value(branch, FLOAT_TYPE, name);
}

Term* procure_bool(Branch& branch, std::string const& name)
{
    return procure_value(branch, BOOL_TYPE, name);
}

void resize_list(Branch& list, int numElements, Term* type)
{
    ca_assert(numElements >= 0);

    // Add terms if necessary
    for (int i=list.length(); i < numElements; i++)
        create_value(list, type);

    // Remove terms if necessary
    bool anyRemoved = false;
    for (int i=numElements; i < list.length(); i++) {
        list.set(i, NULL);
        anyRemoved = true;
    }

    if (anyRemoved)
        list.removeNulls();
}

void set_step(Term* term, float step)
{
    term->setFloatProp("step", step);
}

float get_step(Term* term)
{
    return term->floatPropOptional("step", 1.0);
}

void create_rebind_branch(Branch& rebinds, Branch& source, Term* rebindCondition, bool outsidePositive)
{
    rebinds.clear();

    std::vector<std::string> reboundNames;
    list_names_that_this_branch_rebinds(source, reboundNames);

    Branch& outerScope = *source.owningTerm->owningBranch;
    for (unsigned i=0; i < reboundNames.size(); i++) {
        std::string name = reboundNames[i];
        Term* outerVersion = find_named(outerScope, name);
        Term* innerVersion = source[name];

        Term* pos = outsidePositive ? outerVersion : innerVersion;
        Term* neg = outsidePositive ? innerVersion : outerVersion ;
        apply(rebinds, COND_FUNC, RefList(rebindCondition, pos, neg), name);
    }
}

} // namespace circa
