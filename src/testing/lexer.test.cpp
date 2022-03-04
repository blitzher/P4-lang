#include "./test_lib.h"

void acceptance_test()
{
	test_lib::REGISTER;
	test_lib::accept();
}

void denial_test()
{
	test_lib::REGISTER;
	test_lib::deny("Deny test");
}

void str_cmp_invalid_test()
{
	test_lib::REGISTER;
	test_lib::expect_equal_s(
		"Hello world!\nThis is some message which aren't equal",
		"Hello world!\nThis is some message which arent equal");
}

void str_cmp_valid_test()
{
	test_lib::REGISTER;
	test_lib::expect_equal_s(
		"Hello world!\nThis is some message which are equal",
		"Hello world!\nThis is some message which are equal");
}

int main(void)
{
	acceptance_test();
	denial_test();
	str_cmp_invalid_test();
	str_cmp_valid_test();
	test_lib::print_recap();
	return test_lib::was_success();
}
