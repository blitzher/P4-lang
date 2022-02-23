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

int main(void)
{
	acceptance_test();
	denial_test();
	test_lib::print_recap();
	return 0;
}
