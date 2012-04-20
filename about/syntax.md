---
title: "Quick Syntax Guide"
description: ""
layout: about
author: Andrew Fischer
date: 4-13-2012
---

A quick tour of Circa's syntax.

## Expressions

Simple values:

    3
    5.0
    "hello"
    true
    :some_name

Infix expressions are supported. with C-like precedence:

    1 + 2 * 3 / 4

## Comments

Single-line comments start with `--`.

    -- this is a comment

Multiline comments start with `{-` and end with `-}`. These can be nested.

    {-
    this is a
    multiline comment
    {- with nested brackets -}
    -}

## Color literals

Circa is often used for drawing, so the language supports color literals, with the
same syntax as CSS.

    red = #f00
    transparent_blue = #00f8
    international_orange = #ff4f00

## Function calls

Function calls are written with the function name followed by a parenthesized list.
Commas are optional when seperating arguments.

    func()
    print(msg)
    another_function(arg1 arg2)
    a_third_function(arg1, arg2, arg3)


## Right-apply

The -> syntax is an alternative way to make a function call, the inputs are on the left
and the function is on the right. 

    msg -> print

    -- does the same thing as:
    print(msg)


## Name bindings

    a = 1
    name = "Harry"
    harrys_age = get_person_age(name)

A name binding may precede any expression. (Future: we might require `var` or `val`
keyword in front of new names, to avoid problems caused by misspellings)

## Rebinding operators

Some infix operators will rebind the left-hand-side name, so that they behave like they would in C. Example:

    -- this expression:
    a += 2

    -- does the same thing as:
    a = a + 2

The rebinding operators are `+=`, `-=`, `*=`, `/=`.

## Name rebinding operator

The `@` operator can be written in front of a name, this rebinds the name to the
result of the overall statement.

    add(@x, 1.0)

    -- does the same thing as:
    x = add(x, 1.0)

The `@` symbol shows up in a few different ways in the language (see for-loops below).
It always means that the given name is being rebound to a new value.

## Lists

Literal lists are written with brackets. Commas are optional when separating elements
inside a literal list.

    ['Apple', 'Banana', 'Coffee']
    numbers = [1 2 3 4]
    rotated_point = [cos(r) sin(r)] * p

Accessing the elements of the list uses the traditional `[]` syntax. Indices are 0 based.

    first_element = list[0]
    list[1] = 2

## Function definitions

Functions are defined with the `def` keyword. Function names are lower-case by convention.

    def sum(int a, int b, int c) -> int
        return a + b + c

Each argument must have a declared type. The declared type can be `any`, for
runtime typing. The function's return type is after the `->`.

The function's output
is given with the `return` keyword. (Future: we might just use the function's final
expression as the return value, as in Ruby).

Significant indentation is used to indicate the function's contents.
Alternatively, instead of significant indentation, you can put braces around the function's contents.
Brace syntax is good for writing a one-liner,
or if you don't like significant indentation.

    def sum(int a, int b) -> int { return a + b }

## Conditionals

    if should_print_hi()
        print('hi')

This will print "hi" if `should_print_hi()` returns true.

Conditionals require an input with type boolean, we don't have truthy and falsey values,
just `true` and `false`.

The `else` and `elif` keywords are supported.

    if one_thing()
        print('one thing')
    elif another_thing()
        print('another thing')
    else
        print('last thing')

Like other blocks, the contents can use significant indentation or braces.

    if should_print_hi() { print('hi') }

Also, the entire block can be used as an expression. The last expression inside the
condition block is used as the overall result.

    max = if a > b { a } else { b }

## Loops

    for x in [1 2 3]
        print(x)

This will print 1, 2, and 3 in order.

Also, the entire for loop can return a value:

    squares = for x in [1 2 3] { x * x }

    -- 'squares' now equals [1, 4, 9]

The keywords `break` and `continue` can be used for flow control.

    -- This prints '1' and '3':
    for x in [1, 2, 3, 4, 5]
        if x == 2
            continue
        if x == 4
            break

        print(x)

The loop can also be used to rewrite the input list, if the `@` symbol
is added. Example:

    list = [1 2 3]
    for i in @list
        i += 2

    -- 'list' now equals [3, 4, 5]

When we're rewriting the input list, we can also use the `discard` statement.
When called, `discard` will `continue` and remove the current element from the output.

    list = [1 2 3]
    for i in @list
        if i == 2
            discard

    -- 'list' now equals [1, 3]


## Typecasting

    int(1.0)
    1 -> number

A type's name can be used to do a cast, like a function call.

## Methods

    [1 2 3].length()
    -- results in 3

Circa supports methods, where the function call is made by writing a dot on
the right side of a value.

Methods are just functions with slightly different names. Just like regular function
calls, the call is resolved statically. There is no inheritance or polymorphism (for now).

Some methods will rebind the left-hand-side name with a new value.

    a = [1 2 3]
    a.append(4)
    -- 'a' now equals [1, 2, 3, 4]

A method can be declared anywhere; one can even add local methods to someone else's type.
Here we declare a method called `last_element` that works on Lists.

    def List.last_element(self) -> any
        return self[self.length() - 1]

    three = [1 2 3].last_element()

We don't write a type in front of the function's first argument (`self`),
because the type is already given by the method's name.

This method is usable anywhere that a declared function would normally be visible.
So, declaring a new method doesn't necessary affect every user of the
List type.

When calling a method that takes no inputs, the parenthesis are optional.

    -- this:
    [1 2 3].length

    -- is equivalent to this:
    [1 2 3].length()

## Compound types

A new compound type can be declared with the `type` keyword:

    type MyType {
        int a
        number b
        string c
    }

Type names are UpperCase by convention.

We can create an empty instance of our new type, or we can cast
an untyped list.

    blank_value = create(MyType)
    filled_in_value = [1, 1.0, 'hello'] -> MyType

We can also declare methods on our new type.

    def MyType.a_times_b(self) -> number
        return self.a * self.b

## State

The `state` keyword declares a value that should be preserved across evaluations.

    state int a = 1

Declaring the type is optional:

    state a = 1

Declaring an initial value is also optional:

    state a

The right hand side may be an initialization expression:

    state a = one_time_expression()

## Namespaces

A namespace is declared with the `namespace` keyword.

    namespace myNamespace
      def myFunc() -> int
        return 1

Namespaced names can be accessed with a colon-separated identifier.

    myNamespace:myFunc()

    
