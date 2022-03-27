#include "./test_lib.h"

/* tests work by registering a test, and running the code,
 * and accepting/denying based on the results, which update
 * the state of the most recently registered test. */
std::unordered_map<std::string, test_lib::test_data> tests;
test_lib::test_data* most_recent_test;

#define CHECK_TESTS_NON_EMPTY(return_val)                            \
	{                                                                \
		if (tests.size() == 0)                                       \
		{                                                            \
			std::cout << "No tests registered!\n";                   \
			std::cout << "Register with test_lib::REGISTER macro\n"; \
			return return_val;                                       \
		}                                                            \
	}
size_t c_str_size(char* c_str)
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
		/* tests are by default unevaluated */
		tests[func_name] = { func_name, UNEVALUATED, "Call \"accept\" or \"deny\" to evaluate" };
		most_recent_test = &tests[func_name];
	}

	void accept()
	{
		CHECK_TESTS_NON_EMPTY()
			if (most_recent_test->test_state == UNEVALUATED)
			{
				most_recent_test->test_state = ACCEPT;
				most_recent_test->err_message = "";
			}
	}

	void deny(std::string err_message)
	{
		CHECK_TESTS_NON_EMPTY()
			most_recent_test->test_state = FAIL;
		most_recent_test->err_message = err_message;
		failed = 1; /* fail */
	}

	/* Helper function for asserting equality of two strings,
	 * generating an error message in case of inequality */
	void expect_equal_s(const std::string expected, const std::string actual)
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

		/* find the line which contains the discrepancy */
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

		/* copy the rest of the line into string */
		int temp = i;
		while (eline_v.size() < 80 && eline_v[i] != '\n')
			eline_v.push_back(expected[i++]);
		i = temp;
		while (aline_v.size() < 80 && aline_v[i] != '\n')
			aline_v.push_back(actual[i++]);

		/* copy line into strings */
		std::string aline{ aline_v.begin(), aline_v.end() };
		std::string eline{ eline_v.begin(), eline_v.end() };

		char* exp_message = (char*)malloc(100);
		char* act_message = (char*)malloc(100);
		char* dif_message = (char*)malloc(100);
		sprintf(exp_message, "Expected: %3i %s", line_num, eline.c_str());
		sprintf(act_message, "Actual  : %3i %s", line_num, aline.c_str());

		size_t exp_message_size = c_str_size(exp_message);
		size_t act_message_size = c_str_size(act_message);

		/* find longest message for writing the ^~~ string */
		size_t longest_msg = (exp_message_size > act_message_size) ? exp_message_size : act_message_size;

		for (uint i = 0; i < line_index + 14; i++)
			dif_message[i] = ' ';
		for (uint i = line_index + 15; i < longest_msg; i++)
			dif_message[i] = '~';

		dif_message[line_index + 14] = '^';

		/* insert the expected line, actual line and
		 * difference message into the error message */
		char* err_message = (char*)malloc(512);
		sprintf(err_message, "\n%s\n%s\n%s", exp_message, act_message, dif_message);
		deny(err_message);

		/* release allocated mem of strings that aren't needed anymore */
		free(act_message);
		free(exp_message);
	}

	/* Helper function for asserting equality of two integers,
	 * generating an error message in case of inequality */
	void expect_equal_i(const int expected, const int actual)
	{
		CHECK_TESTS_NON_EMPTY()

			if (expected == actual)
			{
				accept();
			}
			else
			{
				char* err_message = (char*)malloc(100);
				sprintf(err_message, "\nExpected: %-5i\nActual: %-5i", expected, actual);
				deny(err_message);
			}
	}

	/* Print the results of the tests */
	void print_recap()
	{
		CHECK_TESTS_NON_EMPTY()

			for (const auto& keyval_pair : tests)
			{
				const test_lib::test_data test = keyval_pair.second;

				printf(" - "); /* test result prefix */
				switch (test.test_state)
				{
				case UNEVALUATED:
					printf("\033[33munev\x1B[0m ");	/* colour and status */
					break;
				case ACCEPT:
					std::cout << "\033[32mpass\x1B[0m "; /* colour and status */
					break;
				case FAIL:
					std::cout << "\033[31mfail\x1B[0m "; /* colour and status */
					break;
				default:
					break;
				}

				if (test.test_state == ACCEPT)
					printf("%s\n", test.name.c_str());
				else
					printf("%s: %s\n", test.name.c_str(), test.err_message.c_str());
			}
	}

	/* Get the success state of all tests run.
	 * Should be returned at the end of each test file */
	int was_success()
	{
		return failed;
	}
}