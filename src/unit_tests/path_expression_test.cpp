// Copyright (c) Andrew Fischer. See LICENSE file for license terms.

#include "unit_test_common.h"

#include "inspection.h"
#include "kernel.h"

namespace path_expression_tests {

void simple_name_match()
{
    Block block;
    block.compile("a = 1; b = 2; c = 3");

    Term* a = find_term_from_path_expression(&block, "a");
    Term* b = find_term_from_path_expression(&block, "b");
    Term* c = find_term_from_path_expression(&block, "c");
    test_assert(a != NULL);
    test_assert(b != NULL);
    test_assert(c != NULL);
    test_equals(term_value(a), "1");
    test_equals(term_value(b), "2");
    test_equals(term_value(c), "3");
}

void nested_name_match()
{
    Block block;
    Term* a = block.compile("a = { b = { c = 4 }}");
    block.compile("a2 = { b2 = 5 }");

    test_assert(a == find_term_from_path_expression(&block, "a"));
    Term* c = find_term_from_path_expression(&block, "a/b/c");
    Term* b2 = find_term_from_path_expression(&block, "a2/b2");

    test_assert(c != NULL);
    test_assert(b2 != NULL);
    test_equals(term_value(c), "4");
    test_equals(term_value(b2), "5");
}

void wildcard_nested_match()
{
    Block block;
    block.compile("a = { b = { c = 5 } }");
    block.compile("a = { b2 = { d = 6 } }");

    Term* d = find_term_from_path_expression(&block, "a/*/d");
    test_assert(d != NULL);
    test_equals(term_value(d), "6");
}

void recursive_wildcard_match()
{
    Block block;
    block.compile("a = 1");
    block.compile("b = { c = 2 }");
    block.compile("d = { e = { f = 3 } }");

    Term* a = find_term_from_path_expression(&block, "**/a");
    Term* c = find_term_from_path_expression(&block, "**/c");
    Term* f = find_term_from_path_expression(&block, "** / f");
    test_assert(a != NULL);
    test_assert(c != NULL);
    test_assert(f != NULL);
    test_equals(term_value(a), "1");
    test_equals(term_value(c), "2");
    test_equals(term_value(f), "3");
}

void function_match()
{
    Block block;
    block.compile("a = add(1 2)");
    block.compile("b = mult(3 4)");

    Term* a = find_term_from_path_expression(&block, "function=add");
    Term* b = find_term_from_path_expression(&block, "function=mult");
    test_assert(a != NULL);
    test_assert(b != NULL);
    test_assert(a->function == FUNCS.add);
    test_assert(b->function == FUNCS.mult);
}

void recursive_function_match()
{
    Block block;
    block.compile("def f() { for i in [1] { if i == 2 { return 3 } } }");

    Term* returnCall = find_term_from_path_expression(&block, "** / function=return");
    test_assert(returnCall != NULL);
    test_assert(returnCall->function == FUNCS.return_func);
}

void register_tests()
{
    REGISTER_TEST_CASE(path_expression_tests::simple_name_match);
    REGISTER_TEST_CASE(path_expression_tests::nested_name_match);
    REGISTER_TEST_CASE(path_expression_tests::wildcard_nested_match);
    REGISTER_TEST_CASE(path_expression_tests::recursive_wildcard_match);
    REGISTER_TEST_CASE(path_expression_tests::function_match);
    REGISTER_TEST_CASE(path_expression_tests::recursive_function_match);
}

}
