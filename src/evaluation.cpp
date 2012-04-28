// Copyright (c) Andrew Fischer. See LICENSE file for license terms.

#include "common_headers.h"

#include "building.h"
#include "build_options.h"
#include "branch.h"
#include "code_iterators.h"
#include "dict.h"
#include "evaluation.h"
#include "function.h"
#include "introspection.h"
#include "kernel.h"
#include "list.h"
#include "locals.h"
#include "parser.h"
#include "stateful_code.h"
#include "names.h"
#include "term.h"
#include "type.h"

namespace circa {

Stack::Stack()
 : numFrames(0),
   stack(NULL),
   running(false),
   errorOccurred(false),
   world(NULL)
{
    gc_register_new_object((CircaObject*) this, &EVAL_CONTEXT_T, true);
}

Stack::~Stack()
{
    // clear error so that pop_frame doesn't complain about losing an errored frame.
    clear_error(this);

    while (numFrames > 0)
        pop_frame(this);
    free(stack);

    gc_on_object_deleted((CircaObject*) this);
}

void eval_context_list_references(CircaObject* object, GCReferenceList* list, GCColor color)
{
    // todo
}

void eval_context_print_multiline(std::ostream& out, Stack* context)
{
    out << "[Stack " << context << "]" << std::endl;
    for (int frameIndex = 0; frameIndex < context->numFrames; frameIndex++) {
        Frame* frame = get_frame(context, context->numFrames - 1 - frameIndex);
        Branch* branch = frame->branch;
        out << " [Frame " << frameIndex << ", branch = " << branch
             << ", pc = " << frame->pc
             << ", nextPc = " << frame->nextPc
             << "]" << std::endl;

        if (branch == NULL)
            continue;

        for (int i=0; i < frame->endPc; i++) {
            Term* term = branch->get(i);

            // indent
            for (int x = 0; x < frameIndex+1; x++)
                out << " ";

            if (frame->pc == i)
                out << ">";
            else
                out << " ";

            print_term(out, term);

            // current value
            if (!is_value(term)) {
                caValue* value = frame->registers[term->index];
                if (value == NULL)
                    out << " [<register OOB>]";
                else
                    out << "  [" << value->toString() << "]";
            }
            out << std::endl;
        }
    }
}

void eval_context_setup_type(Type* type)
{
    type->name = name_from_string("Stack");
    type->gcListReferences = eval_context_list_references;
}

Frame* get_frame(Stack* context, int depth)
{
    ca_assert(depth >= 0);
    ca_assert(depth < context->numFrames);
    return &context->stack[context->numFrames - 1 - depth];
}
Frame* get_frame_from_bottom(Stack* context, int index)
{
    ca_assert(index >= 0);
    ca_assert(index < context->numFrames);
    return &context->stack[index];
}
Frame* push_frame(Stack* context, Branch* branch, List* registers)
{
    context->numFrames++;
    context->stack = (Frame*) realloc(context->stack, sizeof(Frame) * context->numFrames);
    Frame* top = &context->stack[context->numFrames - 1];
    initialize_null(&top->registers);
    swap(registers, &top->registers);
    top->registers.resize(get_locals_count(branch));
    top->branch = branch;
    top->pc = 0;
    top->nextPc = 0;
    top->endPc = branch->length();
    top->loop = false;
    top->dynamicCall = false;
    top->override = false;
    top->stop = false;

    // We are now referencing this branch
    gc_mark_object_referenced(&branch->header);

    return top;
}
Frame* push_frame(Stack* context, Branch* branch)
{
    List registers;
    return push_frame(context, branch, &registers);
}
void pop_frame(Stack* stack)
{
    Frame* top = top_frame(stack);

    set_null(&top->registers);
    stack->numFrames--;
}

void push_frame_with_inputs(Stack* context, Branch* branch, caValue* inputs)
{
    // Fetch inputs and start preparing the new stack frame.
    List registers;
    registers.resize(get_locals_count(branch));
    
    // Cast inputs into placeholders
    if (inputs != NULL) {
        for (int i=0; i < list_length(inputs); i++) {
            Term* placeholder = get_input_placeholder(branch, i);
            if (placeholder == NULL)
                break;

            caValue* input = list_get(inputs, i);

            bool castSuccess = cast(input, placeholder->type, registers[i]);

            if (!castSuccess) {
                std::stringstream msg;
                msg << "Couldn't cast input " << input->toString()
                    << " (at index " << i << ")"
                    << " to type " << name_to_string(placeholder->type->name);
                raise_error_msg(context, msg.str().c_str());
                return;
            }
        }
    }

    // Push our frame (with inputs) onto the stack
    push_frame(context, branch, &registers);
}

void frame_set_stop_when_finished(Frame* frame)
{
    frame->stop = true;
}

void fetch_stack_outputs(Stack* context, caValue* outputs)
{
    Frame* top = top_frame(context);

    set_list(outputs, 0);

    for (int i=0;; i++) {
        Term* placeholder = get_output_placeholder(top->branch, i);
        if (placeholder == NULL)
            break;

        copy(top->registers[placeholder->index], circa_append(outputs));
    }
}

void finish_frame(Stack* stack)
{
    Frame* top = top_frame(stack);
    Branch* finishedBranch = top->branch;

    // Check to loop
    if (top->loop) {
        for_loop_finish_frame(stack);
        return;
    }

    // Check to finish dynamic_call
    if (top->dynamicCall) {
        finish_dynamic_call(stack);
        return;
    }

    // Hang on to the register list
    List registers;
    swap(&registers, &top->registers);

    // Pop frame
    pop_frame(stack);
    
    Frame* parentFrame = top_frame(stack);

    if (parentFrame->pc < parentFrame->branch->length()) {
        Term* finishedTerm = parentFrame->branch->get(parentFrame->pc);
        List* parentRegisters = &top_frame(stack)->registers;
        
        // Copy outputs to the parent frame, and advance PC.
        for (int i=0;; i++) {
            Term* placeholder = get_output_placeholder(finishedBranch, i);
            if (placeholder == NULL)
                break;

            swap(registers[placeholder->index], parentRegisters->get(finishedTerm->index + i));
        }
    }

    parentFrame->pc = parentFrame->nextPc;
}

Frame* top_frame(Stack* stack)
{
    if (stack->numFrames == 0)
        return NULL;
    return get_frame(stack, 0);
}
Branch* top_branch(Stack* stack)
{
    Frame* frame = top_frame(stack);
    if (frame == NULL)
        return NULL;
    return frame->branch;
}

void reset_stack(Stack* stack)
{
    while (stack->numFrames > 0)
        pop_frame(stack);
}

void evaluate_single_term(Stack* context, Term* term)
{
    Frame* frame = push_frame(context, term->owningBranch);
    frame->pc = term->index;
    frame->nextPc = term->index;
    frame->endPc = frame->pc + 1;

    run_interpreter(context);
}

void copy_locals_back_to_terms(Frame* frame, Branch* branch)
{
    // Copy stack back to the original terms.
    for (int i=0; i < branch->length(); i++) {
        Term* term = branch->get(i);
        if (is_value(term)) continue;
        caValue* val = frame->registers[term->index];
        if (val != NULL)
            copy(val, term_value(branch->get(i)));
    }
}

void insert_top_level_state(Stack* context, Branch* branch)
{
    Term* input = find_state_input(branch);
    if (input == NULL)
        return;

    copy(&context->state, top_frame(context)->registers[input->index]);
}

void save_top_level_state(Stack* context, Branch* branch)
{
    Term* output = find_state_output(branch);
    if (output == NULL || output->numInputs() < 1 || output->input(0) == NULL)
        return;

    move(top_frame(context)->registers[output->input(0)->index], &context->state);
}

void evaluate_branch(Stack* context, Branch* branch)
{
    set_branch_in_progress(branch, false);

    // Top-level call
    push_frame(context, branch);

    // Check to insert top-level state
    insert_top_level_state(context, branch);

    run_interpreter(context);

    if (!error_occurred(context)) {
        save_top_level_state(context, branch);
        pop_frame(context);
    }
}

void evaluate_save_locals(Stack* context, Branch* branch)
{
    // Top-level call
    push_frame(context, branch);

    // Check to insert top-level state
    insert_top_level_state(context, branch);

    run_interpreter(context);

    save_top_level_state(context, branch);

    copy_locals_back_to_terms(top_frame(context), branch);

    if (!error_occurred(context))
        pop_frame(context);
}

void evaluate_branch(Branch* branch)
{
    Stack context;
    evaluate_save_locals(&context, branch);
}

void insert_explicit_inputs(Stack* context, caValue* inputs)
{
    Frame* top = top_frame(context);

    int nextInput = 0;
    for (int i=0; i < top->branch->length(); i++) {
        if (nextInput > circa_count(inputs))
            break;

        Term* term = top->branch->get(i);
        if (term->function != FUNCS.input_explicit)
            continue;

        copy(circa_index(inputs, nextInput), top->registers[i]);
        nextInput++;
    }
}

void extract_explicit_outputs(Stack* context, caValue* inputs)
{
    Frame* top = top_frame(context);

    for (int i=0; i < top->branch->length(); i++) {
        Term* term = top->branch->get(i);
        if (term->function != FUNCS.output_explicit)
            continue;

        copy(top->registers[i], list_append(inputs));
    }
}

caValue* find_stack_value_for_term(Stack* context, Term* term, int stackDelta)
{
    if (term == NULL)
        return NULL;

    if (is_value(term))
        return term_value(term);

    for (int i=stackDelta; i < context->numFrames; i++) {
        Frame* frame = get_frame(context, i);
        if (frame->branch != term->owningBranch)
            continue;
        return frame->registers[term->index];
    }

    return NULL;
}

int num_inputs(Stack* context)
{
    return count_input_placeholders(top_frame(context)->branch);
}

void consume_inputs_to_list(Stack* context, List* list)
{
    int count = num_inputs(context);
    list->resize(count);
    for (int i=0; i < count; i++) {
        consume_input(context, i, list->get(i));
    }
}

caValue* get_input(Stack* context, int index)
{
    return get_frame_register(top_frame(context), index);
}

bool can_consume_output(Term* consumer, Term* input)
{
    // Disabled due to a few problems
    //  - Stateful values were being lost
    //  - Terms inside of loops were able to consume values outside the loop
    return false;

    //return !is_value(input) && input->users.length() == 1;
}

void consume_input(Stack* context, int index, caValue* dest)
{
    // Disable input consuming
    copy(get_input(context, index), dest);
}

bool consume_cast(Stack* context, int index, Type* type, caValue* dest)
{
    caValue* value = get_input(context, index);
    return cast(value, type, dest);
}

caValue* get_output(Stack* context, int index)
{
    Frame* frame = top_frame(context);
    Term* placeholder = get_output_placeholder(frame->branch, index);
    if (placeholder == NULL)
        return NULL;
    return get_frame_register(frame, placeholder->index);
}

Term* current_term(Stack* context)
{
    Frame* top = top_frame(context);
    return top->branch->get(top->pc);
}

Branch* current_branch(Stack* context)
{
    Frame* top = top_frame(context);
    return top->branch;
}

caValue* get_frame_register(Frame* frame, int index)
{
    return frame->registers[index];
}

caValue* get_register(Stack* context, Term* term)
{
    Frame* frame = top_frame(context);
    ca_assert(term->owningBranch == frame->branch);
    return frame->registers[term->index];
}

void create_output(Stack* context)
{
    Term* caller = current_term(context);
    caValue* output = get_output(context, 0);
    create(caller->type, output);
}

void raise_error(Stack* context)
{
    context->running = false;
    context->errorOccurred = true;
}
void raise_error_msg(Stack* context, const char* msg)
{
    Frame* top = top_frame(context);
    caValue* slot = get_frame_register(top, top->pc);
    set_error_string(slot, msg);
    raise_error(context);
}

bool error_occurred(Stack* context)
{
    return context->errorOccurred;
}

void evaluate_range(Stack* context, Branch* branch, int start, int end)
{
    set_branch_in_progress(branch, false);
    push_frame(context, branch);

    for (int i=start; i <= end; i++)
        evaluate_single_term(context, branch->get(i));

    if (error_occurred(context))
        return;

    copy_locals_back_to_terms(top_frame(context), branch);
    pop_frame(context);
}

void evaluate_minimum(Stack* context, Term* term, caValue* result)
{
    // Get a list of every term that this term depends on. Also, limit this
    // search to terms inside the current branch.
    
    Branch* branch = term->owningBranch;
    set_branch_in_progress(branch, false);

    push_frame(context, branch);

    bool *marked = new bool[branch->length()];
    memset(marked, false, sizeof(bool)*branch->length());

    marked[term->index] = true;

    for (int i=term->index; i >= 0; i--) {
        Term* checkTerm = branch->get(i);
        if (checkTerm == NULL)
            continue;

        if (marked[i]) {
            for (int inputIndex=0; inputIndex < checkTerm->numInputs(); inputIndex++) {
                Term* input = checkTerm->input(inputIndex);
                if (input == NULL)
                    continue;
                if (input->owningBranch != branch)
                    continue;
                // don't follow :meta inputs
                if (function_get_input_meta(as_function(term_value(checkTerm->function)),
                            inputIndex))
                    continue;
                marked[input->index] = true;
            }
        }
    }

    for (int i=0; i <= term->index; i++) {
        if (marked[i])
            evaluate_single_term(context, branch->get(i));
    }

    // Possibly save output
    if (result != NULL)
        copy(top_frame(context)->registers[term->index], result);

    delete[] marked;

    pop_frame(context);
}

caValue* evaluate(Stack* context, Branch* branch, std::string const& input)
{
    int prevHead = branch->length();
    Term* result = parser::compile(branch, parser::statement_list, input);
    evaluate_range(context, branch, prevHead, branch->length() - 1);
    return term_value(result);
}

caValue* evaluate(Branch* branch, Term* function, List* inputs)
{
    Stack context;

    TermList inputTerms;
    inputTerms.resize(inputs->length());

    for (int i=0; i < inputs->length(); i++)
        inputTerms.setAt(i, create_value(branch, inputs->get(i)));

    int prevHead = branch->length();
    Term* result = apply(branch, function, inputTerms);
    evaluate_range(&context, branch, prevHead, branch->length() - 1);
    return term_value(result);
}

caValue* evaluate(Term* function, List* inputs)
{
    Branch scratch;
    return evaluate(&scratch, function, inputs);
}

void clear_error(Stack* cxt)
{
    cxt->errorOccurred = false;
}

void print_error_stack(Stack* context, std::ostream& out)
{
    for (int frameIndex = 0; frameIndex < context->numFrames; frameIndex++) {
        Frame* frame = get_frame(context, context->numFrames - 1 - frameIndex);

        if (frame->override) {
            std::cout << "[override call]" << std::endl;
            continue;
        }

        if (frame->pc >= frame->branch->length()) {
            std::cout << "(end of frame)" << std::endl;
            continue;
        }

        Term* term = frame->branch->get(frame->pc);

        out << get_short_location(term) << " ";
        if (term->name != "")
            out << term->name << " = ";
        out << term->function->name;
        out << "()";
        out << " | ";
        out << get_frame_register(frame, frame->pc)->toString();
        std::cout << std::endl;
    }
}

void update_context_to_latest_branches(Stack* context)
{
    for (int i=0; i < context->numFrames; i++) {
        Frame* frame = get_frame(context, i);
        frame->registers.resize(get_locals_count(frame->branch));
    }
}

Branch* if_block_choose_branch(Stack* context, Term* term)
{
    // Find the accepted case
    Branch* contents = nested_contents(term);

    int termIndex = 0;
    while (contents->get(termIndex)->function == FUNCS.input)
        termIndex++;

    for (; termIndex < contents->length(); termIndex++) {
        Term* caseTerm = contents->get(termIndex);
        caValue* caseInput = find_stack_value_for_term(context, caseTerm->input(0), 0);

        // Fallback block has NULL input
        if (caseTerm->input(0) == NULL)
            return nested_contents(caseTerm);

        // Check type on caseInput
        if (!is_bool(caseInput)) {
            raise_error_msg(context, "Expected bool input");
            return NULL;
        }

        if (as_bool(caseInput))
            return nested_contents(caseTerm);
    }
    return NULL;
}

Branch* dynamic_method_choose_branch(Stack* stack, Term* term)
{
    caValue* object = find_stack_value_for_term(stack, term, 0);
    std::string functionName = term->stringPropOptional("syntax:functionName", "");

    Term* method = find_method((Branch*) top_branch(stack),
        (Type*) circa_type_of(object), functionName.c_str());

    if (method != NULL)
        return function_contents(method);

    return NULL;
}

EvaluateFunc get_override_for_branch(Branch* branch)
{
    // This relationship should be simplified.
    Term* owner = branch->owningTerm;
    if (owner == NULL)
        return NULL;

    if (!is_function(owner)) {
        return NULL;
    }

    Function* func = as_function(owner);

    // Subroutine no longer acts as an override
    if (func->evaluate == evaluate_subroutine)
        return NULL;

    return func->evaluate;
}

Branch* choose_branch(Stack* context, Term* term)
{
    if (is_value(term))
        return NULL;
    else if (term->function == FUNCS.if_block)
        return if_block_choose_branch(context, term);
        /* dynamic method currently works as an override
    else if (term->function == FUNCS.dynamic_method)
        return dynamic_method_choose_branch((caStack*) context, term);
        */
    else if (term->function == FUNCS.declared_state)
        return function_contents(term->function);
    else if (term->function == FUNCS.lambda)
        // FUNCS.lambda acts as a branch value
        return NULL;
    else if (term->nestedContents != NULL)
        return term->nestedContents;
    else if (term->function != NULL)
        return function_contents(term->function);
    else
        return NULL;
}

void start_interpreter_session(Stack* context)
{
    Branch* topBranch = top_frame(context)->branch;

    // Make sure there are no pending code updates.
    set_branch_in_progress(topBranch, false);

    // Check if our context needs to be updated following branch modification
    update_context_to_latest_branches(context);

    // Cast all inputs, in case they were passed in uncast.
    for (int i=0;; i++) {
        Term* placeholder = get_input_placeholder(topBranch, i);
        if (placeholder == NULL)
            break;
        caValue* slot = get_frame_register(top_frame(context), i);
        cast(slot, placeholder->type, slot);
    }
}

void step_interpreter(Stack* context)
{
    Frame* frame = top_frame(context);
    Branch* branch = frame->branch;

    // Advance pc to nextPc
    frame->pc = frame->nextPc;
    frame->nextPc = frame->pc + 1;

    // Check if we have finished this branch
    if (frame->pc >= frame->endPc) {

        // Exit if we have finished the topmost branch
        if (context->numFrames == 1 || frame->stop) {
            context->running = false;
            return;
        }

        // Finish this frame
        finish_frame(context);
        return;
    }

    if (frame->pc == 0) {
        // Check if there is a C override for this branch.
        EvaluateFunc override = get_override_for_branch(branch);

        if (override != NULL) {
            Frame* newFrame = top_frame(context);
            newFrame->override = true;

            // By default, we'll set nextPc to finish this frame on the next iteration.
            // The override func may change nextPc.
            newFrame->nextPc = top_frame(context)->endPc;

            // Call override
            override((caStack*) context);

            return;
        }
    }

    Term* currentTerm = branch->get(frame->pc);

    if (currentTerm == NULL)
        return;

    // Certain functions must be handled in-place
    if (currentTerm->function == FUNCS.output) {

        caValue* in = find_stack_value_for_term(context, currentTerm->input(0), 0);
        caValue* out = get_frame_register(frame, frame->pc);
        if (in == NULL)
            circa_set_null(out);
        else
            circa_copy(in, out);

        return;
    } else if (currentTerm->function == FUNCS.loop_output) {

        caValue* in = find_stack_value_for_term(context, currentTerm->input(0), 0);
        caValue* out = get_frame_register(frame, frame->pc);

        if (!is_list(out))
            set_list(out);

        copy(in, list_append(out));

        return;
    }

    // Choose the branch to use for the next frame
    Branch* nextBranch = choose_branch(context, currentTerm);

    // if_block_choose_branch can cause error
    if (error_occurred(context))
        return;

    // No branch, advance to next term.
    if (nextBranch == NULL || nextBranch->emptyEvaluation)
        return;

    // Push new frame
    push_frame(context, nextBranch);

    // Copy each input to the new frame
    for (int i=0, destIndex = 0; i < currentTerm->numInputs(); i++) {
        caValue* inputSlot = circa_input((caStack*) context, destIndex);
        Term* inputTerm = get_input_placeholder(nextBranch, destIndex);

        if (inputSlot == NULL || inputTerm == NULL)
            break;

        caValue* input = find_stack_value_for_term(context, currentTerm->input(i), 1);

        if (input == NULL) {
            set_null(inputSlot);
            continue;
        }

        // TODO: More efficient way of checking for multiple inputs
        bool multiple = inputTerm->boolPropOptional("multiple", false);

        if (multiple) {
            // This arg accepts multiple inputs: append to a list
            if (!is_list(inputSlot))
                set_list(inputSlot, 0);

            copy(input, list_append(inputSlot));

            // Advance to next input, don't change destIndex.
            continue;
        }

        cast(input, inputTerm->type, inputSlot);

        destIndex++;
    }

    // Special case for for-loops.
    if (currentTerm->function == FUNCS.for_func) {

        start_for_loop((caStack*) context);
        return;
    }
}

void run_interpreter(Stack* context)
{
    start_interpreter_session(context);

    context->errorOccurred = false;
    context->running = true;

    while (context->running) {
        step_interpreter(context);
    }
}

void run_interpreter_step(Stack* context)
{
    start_interpreter_session(context);

    context->running = true;
    step_interpreter(context);
    context->running = false;
}

void run_interpreter_steps(Stack* context, int steps)
{
    start_interpreter_session(context);

    context->running = true;
    step_interpreter(context);

    while (context->running && (steps--) > 0) {
        step_interpreter(context);
    }

    context->running = false;
}

void dynamic_call_func(caStack* stack)
{
    Branch* branch = as_branch(circa_input(stack, 0));
    caValue* inputContainer = circa_input(stack, 1);
    caValue* normalInputs = circa_index(inputContainer, 0);
    caValue* stateInput = circa_index(inputContainer, 1);

    Frame* frame = push_frame(stack, branch);
    frame->dynamicCall = true;

    // Copy inputs to the new frame
    int normalInputIndex = 0;
    for (int i=0;; i++) {
        Term* placeholder = get_input_placeholder(branch, i);
        if (placeholder == NULL)
            break;
        caValue* slot = circa_input(stack, i);
        if (is_state_input(placeholder))
            copy(stateInput, slot);
        else {
            caValue* normalInput = circa_index(normalInputs, normalInputIndex++);

            if (normalInput != NULL)
                copy(normalInput, slot);
            else
                set_null(slot);
        }
    }
}

void finish_dynamic_call(caStack* stack)
{
    // Hang on to registers list
    Frame* top = top_frame(stack);
    Branch* branch = top->branch;
    List registers;
    swap(&registers, &top->registers);

    pop_frame(stack);

    // Copy outputs to a DynamicOutputs container
    caValue* out = circa_output(stack, 0);
    create(TYPES.dynamicOutputs, out);
    caValue* normalOutputs = circa_index(out, 0);
    caValue* stateOutput = circa_index(out, 1);

    for (int i=0;; i++) {
        Term* placeholder = get_output_placeholder(branch, i);
        if (placeholder == NULL)
            break;
        caValue* slot = registers.get(placeholder->index);
        if (is_state_output(placeholder))
            copy(slot, stateOutput);
        else {
            caValue* normalInput = circa_append(normalOutputs);

            if (normalInput != NULL)
                copy(slot, normalInput);
        }
    }
}

} // namespace circa

using namespace circa;

// Public API

extern "C" {

caStack* circa_alloc_stack(caWorld* world)
{
    Stack* stack = new Stack();
    stack->world = world;
    return stack;
}

void circa_dealloc_stack(caStack* stack)
{
    delete (Stack*) stack;
}

bool circa_has_error(caStack* stack)
{
    return error_occurred(stack);
}
void circa_clear_error(caStack* stack)
{
    clear_error(stack);
}
void circa_clear_stack(caStack* stack)
{
    reset_stack(stack);
}
void circa_run_function(caStack* stack, caFunction* func, caValue* inputs)
{
    Branch* branch = function_contents((Function*) func);
    
    set_branch_in_progress(branch, false);
    
    push_frame_with_inputs(stack, branch, inputs);
    
    run_interpreter(stack);
    
    // Save outputs to the user's list.
    fetch_stack_outputs(stack, inputs);
    
    if (!error_occurred(stack)) {
        pop_frame(stack);
    }
}

void circa_push_function(caStack* stack, caFunction* func)
{
    Branch* branch = function_contents((Function*) func);
    
    set_branch_in_progress(branch, false);
    
    push_frame(stack, branch);
}

bool circa_push_function_by_name(caStack* stack, const char* name)
{
    caFunction* func = circa_find_function(NULL, name);

    if (func == NULL) {
        // TODO: Save this error on the stack instead of stdout
        std::cout << "Function not found: " << name << std::endl;
        return false;
    }

    circa_push_function(stack, func);
    return true;
}

caValue* circa_frame_input(caStack* stack, int index)
{
    Frame* top = top_frame(stack);
    
    if (top == NULL)
        return NULL;

    if (top->branch->get(index)->function != FUNCS.input)
        return NULL;
    
    return top->registers[index];
}

caValue* circa_frame_output(caStack* stack, int index)
{
    Frame* top = top_frame(stack);

    int realIndex = top->branch->length() - index - 1;

    if (top->branch->get(realIndex)->function != FUNCS.output)
        return NULL;

    return top->registers.get(realIndex);
}

void circa_run(caStack* stack)
{
    run_interpreter(stack);
}

void circa_pop(caStack* stack)
{
    pop_frame(stack);
}

caBranch* circa_top_branch(caStack* stack)
{
    return (caBranch*) top_frame(stack)->branch;
}

caValue* circa_input(caStack* stack, int index)
{
    return get_input(stack, index);
}
int circa_num_inputs(caStack* stack)
{
    return num_inputs(stack);
}
int circa_int_input(caStack* stack, int index)
{
    return circa_int(circa_input(stack, index));
}

float circa_float_input(caStack* stack, int index)
{
    return circa_to_float(circa_input(stack, index));
}
float circa_bool_input(caStack* stack, int index)
{
    return circa_bool(circa_input(stack, index));
}

const char* circa_string_input(caStack* stack, int index)
{
    return circa_string(circa_input(stack, index));
}

caValue* circa_output(caStack* stack, int index)
{
    return get_output(stack, index);
}

void circa_output_error(caStack* stack, const char* msg)
{
    set_error_string(circa_output(stack, 0), msg);
    raise_error(stack);
}

caTerm* circa_caller_input_term(caStack* stack, int index)
{
    return circa_term_get_input(circa_caller_term(stack), index);
}

caBranch* circa_caller_branch(caStack* stack)
{
    Frame* frame = get_frame(stack, 1);
    if (frame == NULL)
        return NULL;
    return frame->branch;
}

caTerm* circa_caller_term(caStack* stack)
{
    Stack* cxt = (Stack*) stack;
    if (cxt->numFrames < 2)
        return NULL;
    Frame* frame = get_frame(cxt, 1);
    return frame->branch->get(frame->pc);
}

void circa_print_error_to_stdout(caStack* stack)
{
    print_error_stack(stack, std::cout);
}
int circa_frame_count(caStack* stack)
{
    return stack->numFrames;
}
void circa_stack_restore_height(caStack* stack, int height)
{
    while (stack->numFrames > height)
        pop_frame(stack);
}

} // extern "C"
