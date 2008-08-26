
#include "common_headers.h"

#include "circa.h"
#include "tests/common.h"

namespace circa {
namespace list_tests {

void range()
{
    Branch* branch = new Branch();

    Term* range_zero_to_ten = parser::quick_exec_statement(branch, "range(10)");

    test_assert(as_int(as_list(range_zero_to_ten)->get(0)) == 0);
    test_assert(as_int(as_list(range_zero_to_ten)->get(9)) == 9);
}

void list_apply()
{
    Branch* branch = new Branch();

    Term* result = parser::quick_exec_statement(branch, "list-apply(to-string, range(5))");
    
    test_assert(as_string(as_list(result)->get(0)) == "0");
    test_assert(as_string(as_list(result)->get(4)) == "4");
}

} // namespace list_tests

void register_list_tests()
{
    REGISTER_TEST_CASE(list_tests::range);
    REGISTER_TEST_CASE(list_tests::list_apply);
}

} // namespace circa
