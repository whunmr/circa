// Copyright (c) 2007-2009 Andrew Fischer. All rights reserved.

#include "common_headers.h"

namespace circa {

Term* find_named(Branch& branch, std::string const& name);

Term* get_named(Branch& branch, std::string const& name);

// Look for a dot separated name. Ie, with an input of a.b.c, we'll look for 'a',
// then if found we'll look inside that for 'b', etc.
Term* get_dot_separated_name(Branch& branch, std::string const& name);

bool name_is_reachable_from(Term* term, Branch& branch);

// Get a name of 'term' which is valid in 'branch'. This might simply return term's
// name, or if term is inside a namespace or object, this would return a
// dot-separated name.
std::string get_relative_name(Branch& branch, Term* term);
std::string get_relative_name(Term* location, Term* term);

void expose_all_names(Branch& source, Branch& destination);

} // namespace circa