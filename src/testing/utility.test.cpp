#include "./test_lib.h"

void token_type_to_string()
{
    test_lib::REGISTER;

    test_lib::expect_equal_s(epicr::token_to_string(epicr::E_TT_PARENS_CLOSE), "E_TT_PARENS_CLOSE");
    test_lib::expect_equal_s(epicr::token_to_string(epicr::E_TT_COLON), "E_TT_COLON");
    test_lib::expect_equal_s(epicr::token_to_string(epicr::E_TT_ASTERIX), "E_TT_ASTERIX");
    test_lib::expect_equal_s(epicr::token_to_string(epicr::E_TT_WORD), "E_TT_WORD");
    test_lib::expect_equal_s(epicr::token_to_string(epicr::E_TT_NUMBER), "E_TT_NUMBER");
}

void to_lower()
{
    test_lib::REGISTER;
    test_lib::expect_equal_s(epicr::to_lower("hElLo WoRlD"), "hello world");
    test_lib::expect_equal_s(
        epicr::to_lower("ABCDEFGHIJKLMNOPQRSTUVXYZ"),
        "abcdefghijklmnopqrstuvxyz");
}

void strip_spaces_right()
{
    test_lib::REGISTER;

    test_lib::expect_equal_s(epicr::strip_spaces_right("hello!    "), "hello!");
}

void round_double_to_string()
{
    test_lib::REGISTER;

    test_lib::expect_equal_s(epicr::round_double_to_string(2.5f), "2.50");
}

void amount_to_string()
{
    test_lib::REGISTER;
    epicr::amount amnt = {10.22, false, "", "grams", false};
    test_lib::expect_equal_s(epicr::amount_to_string(amnt), "(10.22 grams)");
}

int main()
{
    token_type_to_string();
    to_lower();
    strip_spaces_right();
    round_double_to_string();
    amount_to_string();

    test_lib::print_recap();
    return test_lib::result();
}