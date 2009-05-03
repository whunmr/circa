// Copyright 2009 Andrew Fischer

#include "circa.h"

namespace circa {

Term* get_for_loop_iterator(Term* loopTerm)
{
    return loopTerm->state->asBranch()["contents"]->asBranch()[0];
}

} // namespace circa
