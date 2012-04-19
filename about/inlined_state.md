---
title: "Inlined State"
description: ""
layout: about
author: Andrew Fischer
date: 4-14-2012
---

<h1>Inlined state in Circa</h1>
<div class="article_credit">written April 14, 2012 - Andrew Fischer</div>

Inlined state is one of my favorite features
in Circa, because it started out as a pragmatic solution to a problem and
unexpectedly turned out to be a really fun and highly productive feature.

### Motivation

Let's first define the problem. In Circa, we support changes to source code at runtime.
We can consider two categories of code changes:

 * *Structured* changes - Editing code at the AST level. The system understands each
   change, and it knows exactly how the old version relates to the new version.
   The user can only perform changes that the system understands.

 * *Unstructured* changes - Editing text with a separate editor. From the interpreter's
   point of view, all we see is the old text and the new text. We can use algorithms to
   try to better understand the change (such as the diff algorithm), but depending on
   how big the change is, we might misunderstand the user's intentions. Here, the user
   is not limited by what change operations they can use.

We want to support both changes, and we want to preserve the program's current state
across each change.

Preserving state across structured changes definitely feels doable, depending on how
complex the change operations are that we allow. In this article I'm only going to focus
on handling *unstructured* changes. (For further reading about handling structured
changes, Olov Johansson wrote a <a href="http://blog.lassus.se/files/liveprogramming.pdf">thesis</a>
on solving the problem for Javascript.)

There's one more caveat before we get started: this article doesn't talk about how
to preserve the call stack across modification. In the current scheme, a block of code must
run until completion before we can replace it with new code. We definitely want
to also be able to preserve the call stack, this is necessary for code that blocks
or has long-running loops. Call-stack migration is planned for the future.

So anyway, the problem is to migrate state from one piece of Circa code to another,
with no guarantees as to how the two pieces of code are related.

### Declaring state

We declare our stateful values with the `state` keyword. Since most Circa functions
are pure, stateful values are our equivalent for mutable variables.

    state a = 1
    
Every time we call the same block of code, the stateful values are preserved from the
last iteration.

    state total_connections = 0
    total_connections += count_new_connections()

We refer to this as *inlined* state because the declarations are written
inline with the code.

When declaring state we can optionally have an initializer expression, which is
written on the right following an `=` sign.

This expression is used if we don't already have a value for this variable (such as,
if this is the first time we've run this code, or if the state was deliberately reset).

    -- Keep track the time elapsed
    state time_started = current_time()
    time_elapsed = time_started - current_time()

So in this example we only assign `time_started` to `current_time()` once. From then
on, we remember the time_started value, and use it to compute the time elapsed.

At any time, the interpreter knows what the state of the whole system is, and we can
represent it as a dictionary with the variable names as keys.  In the above example, our
state might look like this:

    {'time_started': 1234567}

### Nested state

It would be hard to write a complex program if we could only declare state at the top level, so
we can also declare state inside functions or nested blocks.

    def elapsed() -> number
        state time_started = current_time()
        return time_started - current_time()

    time_elapsed = elapsed()

This does the same thing as our above example, but we've neatly encapsulated some code
into an `elapsed` function.

Now the system's state might look like this:

    {'time_elapsed': {'time_started': 1234567'}}

Each call to a stateful function creates a separate copy of local state. If we have this
code:

    def elapsed() -> number
        state time_started = current_time()
        return time_started - current_time()

    time_elapsed_1 = elapsed()
    time_elapsed_2 = elapsed()
    time_elapsed_3 = elapsed()

The system's state might look like:

    {'time_elapsed_1': {'time_started': 1234567'},
     'time_elapsed_2': {'time_started': 1234567'},
     'time_elapsed_3': {'time_started': 1234568'}}

If a function has state in it, we call it a stateful function. If function `f` has
state, and `g` has a call to `f`, then `g` becomes implicitly stateful, and so
on for any functions that call `g`.

    def f()
        state a = 1

    def g()
        f()

    def h()
        g()

    h()

The above code would result in the following state:

    {'h': {'g': {'f': {'a': 1}}}}

When a term doesn't have its own name, we'll use the function's name in the state
dictionary.

We can also nest state inside control-flow blocks for some useful effects.

    if condition()
        time_since_condition_was_true = elapsed()
    else
        time_since_condition_was_false = elapsed()

Results in this state:

    {'if_0': ['time_since_condition_was_true': {'time_started': 1234567'},
              'time_since_condition_was_false': {'time_started': 456789'}
             ]}

There's an open question: if one part of an if-block is not evaluated (when the condition
is not met), what should happen to any existing state in that block? Should that
state be destroyed or preserved? In practice I've found that it's most useful to
destroy the branch's state when the condition is not met. This lets us to nice things like:

    if button_pressed()
        print('The button has been held for: ', elapsed())

In that example, the `elapsed` count is reset every time `button_pressed()` returns
false.

Next we'll look at loops. In a for-loop, each iteration has its own copy of state:

    for i in 0..5
        elapsed()

    {'for_0': [{'elapsed': {'time_started': 1234567'}},
               {'elapsed': {'time_started': 1234567'}},
               {'elapsed': {'time_started': 1234567'}},
               {'elapsed': {'time_started': 1234567'}},
               {'elapsed': {'time_started': 1234567'}}]}

If the number of iterations changes, we'll create or destroy stateful containers as necessary.

A side note, it's definitely easy to abuse state in a for-loop: if we have lots of iterations then we
can end up with too much duplicated data. This is a problem that we'll tackle
with runtime warnings and possibly optimizations (the compiler has enough
knowledge that it can figure out when it can merge loop state).

### Preserving state

Anyway, back to our original problem, how to preserve state across an unstructured
change. We know that we can extract a program's entire state into a dictionary-like
structure, so the next thing to do is to insert this dictionary into a new piece
of code. If the variable names match up, we'll keep the old values. If we need to
create new values then we'll use the initialization expressions.

It's important to note that this is an imperfect process. When dealing with unstructured
changes, there will be some changes that the system has no hope of understanding. There
will also be changes that our current system doesn't understand, but maybe a more
clever system could. For example, our system only uses variable names, but we might
be able to also use typing or other context information.

Anyway, here are examples of successful state migration:

Examples:

    Old code                          New code
    --------                          --------
    state a = 0                       state a = 0
    a = 5                             a = 10
                        State:
                       {'a': 2}

*One line is changed but no state is changed. Existing state is preserved.*
<p style="height:40px"></p>

    Old code                          New code
    --------                          --------
    state a = 1           ->          state a = 1
    a += 1                            state b = 2
                                      a += 1
                        State:
                       {'a': 2}

*New code adds a stateful var 'b', which will be initialized to 2.*
<p style="height:40px"></p>

    Old code                          New code
    --------                          --------
    state a = 1           ->          state b = 2
    a += 1                            
                                      
                        State:
                       {'a': 2}

*Variable 'a' was removed, the existing state will be thrown out.*
<p style="height:40px"></p>

    Old code                          New code
    --------                          --------
    def f()               ->          def f()
        state a = 1                       state a = 1
                                          state b = 2
    f()                               f()
    f()                               f()

                        State:
            {'f_1': {'a': 1}, 'f_2': {'a': 1}}

*Added state inside 'f', the existing values for 'a' are preserved and values for 'b' are created.*
<p style="height:40px"></p>

    Old code                          New code
    --------                          --------
    for i in 0..3         ->          for i in 0..10
        state a = 1                       state a = 1

                        State:
          {'_for': [{'a': 1},{'a': 1},{'a': 1}]}

*For loop now has 10 iterations; the existing 3 iterations will use the existing state, and the rest will create new state.*
<p style="height:40px"></p>

    Old code                          New code
    --------                          --------
    if something()        ->          if something()
        state a = 1                       state a = 1
                                      else
                                          state b = 2

                        State:
                 {'_if': [{'a': 1}]}

*Added an 'else' condition. The first condition will use the existing state, and the 'else' condition will create new state.*
<p style="height:40px"></p>

Here are some examples of where state migration will fail:

    Old code                           New code
    --------                           --------
    state int a = some_expression()    state int b = some_expression()

                        State:
                      {'a': 123}

*The user probably just renamed 'a' to 'b', but we only consider names, so this state will be thrown out.*
<p style="height:40px"></p>

    Old code                           New code
    --------                           --------
    if true                            state a = something()
        state a = something()          if true
        blah()                             blah()

                        State:
                      {'_if': ['a': 123]}

*The user probably moved 'a' from inside the if-block to outside it, but we don't handle state moving across branches.*

### Last thoughts on state migration

On paper it looks like our strategy has some flaws, but in practice, the system works great.
Roughly 90% of our code changes are: appending new code to the bottom of the
file, appending new state variables, and modifying and tweaking existing code (without touching
state declarations). These changes can be handled by our system just fine.

For the occasions where the user does redefine state in a way that automatic migration will fail, we do have options:
 * Depending on what the state is for, it might be okay to throw it out
 * The user can restrict themselves to changes that they know will
   preserve state. (our name-based
   migration is very predictable in terms of when it will succeed or fail).
 * Finally, we can allow for structured modifications. The code change will be performed within the system, so the system will know how to migrate state.

### Faster coding with inlined state

We mentioned before that inlined state is more than just a solution to our migration problem,
it's also a really fast and fun way of creating code. Let's see it in action.

Let's say that we're coding a button. We want the button to change color when the user's
mouse is hovering over. Here's an initial example:

    def draw_button(Rect box)
        normal_color = #f00
        hover_color = #f88
        color = if mouse_over(box) { hover_color } else { normal_color }
        fill_rect(box, color)

With this code, the button will immediately switch between red and pink as the
mouse hovers over. But, we want things to look nice and smooth, so we want the color to smoothly
transition between red and pink.

First we'll change the code to use a scalar when switching between colors.

    def draw_button(Rect box)
        highlight = if mouse_over(box) { 1.0 } else { 0.0 }
        color = interp(highlight #f00 #f88)
        fill_rect(box, color)

There's no change in functionality here, we've just added some math. The `interp` function returns
the first argument (`#f00`) if `highlight` is 0.0 and it returns the second one (`#f88`)
if highlight is 1.0. The `interp` function also smoothly interpolates if the number is somewhere between 0.0 and 1.0, which is a feature we're about to use.

Now the magic happens, we'll add one more line to make it a smooth transition.

    def draw_button(Rect box)
        highlight = if mouse_over(box) { 1.0 } else { 0.0 }
        highlight = approach_over_time(highlight, 1.0)
        color = interp(highlight #f00 #f88)
        fill_rect(box, color)

The `approach_over_time` function is stateful (and now, `draw_button` is stateful too).
The `approach_over_time` function internally keeps a 'current' value,
and over time, its output value gradually approaches the target value. So, we start out
with `highlight` equal to 0.0. Then the mouse hovers over the box. Our "target" (the
value we pass to `approach_over_time` is now 1.0). But the return value won't be 1.0
yet, first it might return 0.1, then 0.2, etc. The end result is that the color slowly
fades in to what it should be - if the mouse is hovering then it fades to pink, if
the mouse is gone then it fades to red. If the user waves their mouse
over the button repeatedly, it handles that too.

If you're interested, here's what `approach_over_time` looks like:

    def approach_over_time(number target, number speed) -> number
        state current = target
        maximum_change = time_delta() * speed
        if target > current
            current += min(maximum_change, target - current)
        elif target < current
            current -= min(maximum_change, current - target)
        return current

But, the point is that we don't need to care what's inside `approach_over_time`.
We have *encapsulation*, where we don't need to care about the details of the
function, just the inputs and outputs.
The code that uses our `draw_button` function also doesn't need to care that
we have added state. We can call `draw_button` several times
and it will behave correctly.

With this tool in our arsenal, we can easily define all sorts of useful stateful
functions. Some examples:

    def once() -> bool
        "Returns true the first time it's called, and false thereafter"
        state bool s = true
        result = s
        s = false
        return result

    def toggle(bool tog) -> bool
        """
        Stateful function, returns a boolean status. Every frame the function is called
        with (true), the result flips. Starts out false.
        """
        state bool s = false
        if tog { s = not(s) }
        return s

    def seed() -> number
        "Returns a random number 0..1 which doesn't change after initialization"
        state number s = rand()
        return s

### Looking at inlined state as a code transformation

This article is getting long, so here's the last section.
One way to think of inlined state is to
look it as a code transformation. We can transform
a stateful function to a stateless function by adding an additional input and output
(this transformation is what Circa actually does internally).

Let's use our `draw_button` example. We'll change it to also return the color, just
because this example is more interesting if the function has an output. Here's our starting code:

    def draw_button(Rect box) -> Color
        highlight = if mouse_over(box) { 1.0 } else { 0.0 }
        highlight = approach_over_time(highlight, 1.0)
        color = interp(highlight, #f00, #f88)
        fill_rect(box, color)
        return color

Here it is as a stateless function, using multiple return values. The state variable
for the `approach_over_time` variable is unpacked (possibly created), then packed and
included in the return variable.

    def draw_button(Dict state, Rect box) -> (Color, Dict)
        highlight = if mouse_over(box) { 1.0 } else { 0.0 }

        if 'highlight' in state
            approach_state = state['highlight']
        else
            approach_state = {}

        highlight, approach_state = approach_over_time(approach_state, highlight, 1.0)
        state['highlight'] = approach_state

        color = interp(highlight #f00 #f88)
        fill_rect(box, color)
        return (color, state)
        
We can also think of it in terms of object-oriented programming, where each stateful function is
actually an object with one method. Here's what that might look like:

    class DrawButton {

        ApproachOverTime approach;

        public Color run(Rect box) {
            highlight = if mouse_over(box) { 1.0 } else { 0.0 }

            if (this.approach == null)
                this.approach = new ApproachOverTime

            highlight = this.approach.run(highlight, 1.0)

            color = interp(highlight #f00 #f88)
            fill_rect(box, color)
            return color
        }
    }

# References

<ul>
<li><a href="http://perldoc.perl.org/perlsub.html#Persistent-Private-Variables">Persistent private variables in Perl 5</a></li>
<li><a href="http://blog.lassus.se/files/liveprogramming.pdf"><i>Describing Live programming using program transformations and a callstack explicit interpreter</i> - Olov Johansson</a></li>
</ul>
