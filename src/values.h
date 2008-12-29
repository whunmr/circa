// Copyright 2008 Andrew Fischer

#ifndef CIRCA_VALUES_INCLUDED
#define CIRCA_VALUES_INCLUDED

#include "common_headers.h"

namespace circa {

void alloc_value(Term* term);

// recycle_value will either call duplicate_value or steal_value, depending
// on heuristics
void recycle_value(Term* source, Term* dest);

void duplicate_value(Term* source, Term* dest);

// Attempt to 'steal' the output value from source. This is more efficient
// than copying, and useful if 1) dest needs a copy of source's value, and
// 2) you don't think that anyone will need the value at source.
void steal_value(Term* source, Term* dest);

void dealloc_value(Term* term);

void update_owner(Term* term);

bool values_equal(Term* a, Term* b);

Term* create_value(Branch* branch, Term* type);

Term* string_value(Branch& branch, std::string const& s, std::string const& name="");
Term* int_value(Branch& branch, int i, std::string const& name="");
Term* float_value(Branch& branch, float f, std::string const& name="");


} // namespace circa

#endif
