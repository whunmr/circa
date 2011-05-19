// Copyright (c) Paul Hodge. See LICENSE file for license terms.

#include <circa.h>

namespace circa {
namespace list_tests {

void test_tagged_value()
{
    Branch branch;

    branch.compile("type MyType { string s, int i }");
    Value* val = branch.eval("MyType()");

    test_assert(is_string(val->getIndex(0)));
    test_assert(is_int(val->getIndex(1)));

    test_assert(is_string(val->getField("s")));
    test_assert(is_int(val->getField("i")));
    test_assert(val->getField("x") == NULL);
}

void test_cast()
{
    Branch branch;

    Value a;
    set_list(&a);

    Value b;
    set_list(&b, 2);
    set_int(b.getIndex(0), 1);
    set_int(b.getIndex(1), 2);
    test_equals(b.toString(), "[1, 2]");

    Value c;
    set_list(&c, 2);
    set_int(c.getIndex(0), 1);
    set_string(c.getIndex(1), "hi");
    test_equals(c.toString(), "[1, 'hi']");

    Value d;
    set_list(&d, 1);
    set_float(d.getIndex(0), 1);
    test_equals(d.toString(), "[1.0]");

    Value x;

    test_assert(!cast_possible(&a, &INT_T));
    test_assert(cast_possible(&a, &LIST_T));

    test_assert(cast_possible(&b, &LIST_T));
    test_assert(cast(&b, &LIST_T, &x));
    test_equals(x.toString(), "[1, 2]");

    Term* t_term = branch.compile("type T { int i, number f }");
    Type* t = unbox_type(t_term);

    test_assert(!cast_possible(&a, t));
    test_assert(cast_possible(&b, t));
    test_assert(!cast_possible(&c, t));
    test_assert(!cast_possible(&d, t));

    test_assert(cast(&b, t, &x));
    test_equals(x.toString(), "[1, 2.0]");
}

void test_remove_nulls()
{
    Value v;
    List list;
    test_equals(list.toString(), "[]");

    for (int i=0; i < 6; i++) {
        set_int(&v, i);
        list.append(&v);
    }

    test_equals(list.toString(), "[0, 1, 2, 3, 4, 5]");

    list.removeNulls();
    test_equals(list.toString(), "[0, 1, 2, 3, 4, 5]");

    set_null(list[3]);
    test_equals(list.toString(), "[0, 1, 2, null, 4, 5]");

    list.removeNulls();
    test_equals(list.toString(), "[0, 1, 2, 4, 5]");

    set_null(list[4]);
    list.removeNulls();
    test_equals(list.toString(), "[0, 1, 2, 4]");

    set_null(list[0]);
    set_null(list[1]);
    set_null(list[3]);
    list.removeNulls();
    test_equals(list.toString(), "[2]");

    set_null(list[0]);
    list.removeNulls();
    test_equals(list.toString(), "[]");
}

void test_remove_index()
{
    List list;
    set_int(list.append(), 0);
    set_int(list.append(), 1);
    set_int(list.append(), 2);
    set_int(list.append(), 3);

    test_equals(&list, "[0, 1, 2, 3]");
    list.remove(2);
    test_equals(&list, "[0, 1, 3]");
    list.remove(0);
    test_equals(&list, "[1, 3]");
    list.remove(1);
    test_equals(&list, "[1]");
    list.remove(0);
    test_equals(&list, "[]");
}

void register_tests()
{
    REGISTER_TEST_CASE(list_tests::test_tagged_value);
    REGISTER_TEST_CASE(list_tests::test_cast);
    REGISTER_TEST_CASE(list_tests::test_remove_nulls);
    REGISTER_TEST_CASE(list_tests::test_remove_index);
}

}
}
