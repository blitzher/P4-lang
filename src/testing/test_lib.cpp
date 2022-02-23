#include "./test_lib.h"

std::unordered_map<std::string, test_lib::test_data> tests;
test_lib::test_data *most_recent_test;

#define CHECK_TESTS_NON_EMPTY(return_val)                            \
	{                                                                \
		if (tests.size() == 0)                                       \
		{                                                            \
			std::cout << "No tests registered!\n";                   \
			std::cout << "Register with test_lib::REGISTER macro\n"; \
			return return_val;                                       \
		}                                                            \
	}

namespace test_lib
{
	void register_test(std::string func_name)
	{
		tests[func_name] = {.name = func_name,
							.accepted = false};
		most_recent_test = &tests[func_name];
	}

	void accept()
	{
		CHECK_TESTS_NON_EMPTY()
		most_recent_test->accepted = true;
		most_recent_test->err_message = "Passed";
	}

	void deny(std::string err_message)
	{
		CHECK_TESTS_NON_EMPTY()
		most_recent_test->accepted = false;
		most_recent_test->err_message = err_message;
	}

	void expect_equal_s(const std::string expected, const std::string actual)
	{
		CHECK_TESTS_NON_EMPTY()
		if (expected == actual)
		{
			accept();
		}
		/* TODO: Find where the discrepancy was */
		else
		{

			char *err_message = (char *)malloc(1000);
			sprintf(err_message, "Failed\nExpected: %s\nActual: %s", expected.c_str(), actual.c_str());
			deny(err_message);
		}
	}

	void expect_equal_i(const int expected, const int actual)
	{
		CHECK_TESTS_NON_EMPTY()

		if (expected == actual)
		{
			accept();
		}
		else
		{
			char *err_message = (char *)malloc(100);
			sprintf(err_message, "Failed\nExpected: %-5i\nActual: %-5i", expected, actual);
			deny(err_message);
		}
	}
	void print_recap()
	{
		CHECK_TESTS_NON_EMPTY()

		for (const auto &keyval_pair : tests)
		{
			const test_lib::test_data test = keyval_pair.second;

			if (test.accepted)
				std::cout << "\u001b[32m\u2713\u001b[0m ";
			else
				std::cout << "\u001b[31m\u2717\u001b[0m ";
			printf("%s: %s\n", test.name.c_str(), test.err_message.c_str());
		}
	}
}