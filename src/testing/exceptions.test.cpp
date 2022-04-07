#include "./test_lib.h"

void parser_double_asterix_exception()
{
    test_lib::REGISTER;

    auto rcp = epicr::parse_recipe_silent("src/test-recipes/PastaErr.rcp");
    test_lib::expect_exception(rcp, "Duplicate asterix");
}

int main()
{
    parser_double_asterix_exception();
    test_lib::print_recap();
    return test_lib::was_success();
}