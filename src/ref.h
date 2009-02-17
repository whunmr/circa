// Copyright 2008 Paul Hodge

#ifndef CIRCA_REFERENCE_INCLUDED
#define CIRCA_REFERENCE_INCLUDED

#include "common_headers.h"

namespace circa {

struct Ref
{
    Term* _target;
    Term* _owner;

    Ref(Term *owner)
      : _target(NULL),
        _owner(owner)
    {
    }

    ~Ref()
    {
        set(NULL);
    }

    // Copy constructor
    Ref(Ref const& copy)
      : _target(NULL),
        _owner(copy._owner)
    {
        set(copy._target);
    }

    // Assignment copy
    Ref& operator=(Ref const& rhs)
    {
        _owner = rhs._owner;
        set(rhs._target);
        return *this;
    }

    void set(Term* target);

    // Convenience = overload
    Ref& operator=(Term* target)
    {
        set(target);
        return *this;
    }

    operator Term*()
    {
        return _target;
    }

    static void removeRef(Term* term, Ref* ref);
};

} // namespace circa

#endif // CIRCA_REFERENCE_INCLUDED
