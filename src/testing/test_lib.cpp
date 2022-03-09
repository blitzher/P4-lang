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
size_t c_str_size(char *c_str)
{
	size_t i = 0;
	while (c_str[i] != '\0')
		i++;
	return i;
}

/* by default 0, set to 1 if any tests fail */
int failed;

namespace test_lib
{
	void register_test(std::string func_name)
	{
		tests[func_name] = {func_name, UNEVALUATED, "Unevaluated\nCall `accept` or `deny` to evaluate"};
		most_recent_test = &tests[func_name];
	}

	void accept()
	{
		CHECK_TESTS_NON_EMPTY()
		if (most_recent_test->test_state == UNEVALUATED)
		{
			most_recent_test->test_state = ACCEPT;
			most_recent_test->err_message = "Passed";
		}
	}

	void deny(std::string err_message)
	{
		CHECK_TESTS_NON_EMPTY()
		most_recent_test->test_state = FAIL;
		most_recent_test->err_message = err_message;
		failed = 1; /* fail */
	}

	void expect_equal_s(const std::string actual, const std::string expected)
	{
		CHECK_TESTS_NON_EMPTY()
		if (expected == actual)
		{
			accept();
			return;
		}

		size_t expected_size = expected.size();
		size_t actual_size = actual.size();
		size_t len_shortest = (expected_size > actual_size) ? actual_size : expected_size;

		uint line_num = 1;
		uint line_index = 0;
		std::vector<char> aline_v;
		std::vector<char> eline_v;
		size_t i = 0;
		for (i = 0; i < len_shortest; i++)
		{
			if (actual[i] != expected[i])
				break;
			if (actual[i] == '\n')
			{
				line_num++;
				line_index = 0;
				aline_v.clear();
				eline_v.clear();
			}
			else
			{
				line_index++;
				aline_v.push_back(actual[i]);
				eline_v.push_back(expected[i]);
			}
		}

		int temp = i;
		while (eline_v.size() < 80 && eline_v[i] != '\n')
			eline_v.push_back(expected[i++]);
		i = temp;
		while (aline_v.size() < 80 && aline_v[i] != '\n')
			aline_v.push_back(actual[i++]);

		std::string aline{aline_v.begin(), aline_v.end()};
		std::string eline{eline_v.begin(), eline_v.end()};

		char *exp_message = (char *)malloc(100);
		char *act_message = (char *)malloc(100);
		char *dif_message = (char *)malloc(100);
		sprintf(exp_message, "Expected: %3i %s", line_num, eline.c_str());
		sprintf(act_message, "Actual  : %3i %s", line_num, aline.c_str());

		size_t exp_message_size = c_str_size(exp_message);
		size_t act_message_size = c_str_size(act_message);

		size_t longest_msg = (exp_message_size > act_message_size) ? exp_message_size : act_message_size;

		for (uint i = 0; i < line_index + 14; i++)
			dif_message[i] = ' ';
		for (uint i = line_index + 15; i < longest_msg; i++)
			dif_message[i] = '~';

		dif_message[line_index + 14] = '^';

		char *err_message = (char *)malloc(300);
		sprintf(err_message, "\n%s\n%s\n%s", exp_message, act_message, dif_message);
		deny(err_message);

		free(act_message);
		free(exp_message);
	}

	void expect_equal_i(const int actual, const int expected)
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

			switch (test.test_state)
			{
			case UNEVALUATED:
				std::cout << "\x1b[33m\u2B55\x1b[0m ";
				break;
			case ACCEPT:
				std::cout << "\x1B[32m\u2714\x1B[0m ";
				break;
			case FAIL:
				std::cout << "\x1B[31m\u2716\x1B[0m ";
				break;
			default:
				break;
			}

			printf("%s: %s\n", test.name.c_str(), test.err_message.c_str());
		}
	}

	int was_success()
	{
		return failed;
	}
}