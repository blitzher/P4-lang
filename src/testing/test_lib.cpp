#include "./test_lib.h"
#include <filesystem>

/* tests work by registering a test, and running the code,
 * and accepting/denying based on the results, which update
 * the state of the most recently registered test. */
std::map<std::string, test_lib::test_data> tests;
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
int failed = 0;
int accept_count = 0;
int deny_count = 0;

namespace test_lib
{
	void register_test(std::string func_name)
	{
		/* tests are by default unevaluated */
		tests[func_name] = { func_name, UNEVALUATED, "Call \"accept\" or \"deny\" to evaluate", 0, 0 };
		most_recent_test = &tests[func_name];
		/* epicr specific, to reset all recursion checking
		 * between unit tests */
		epicr::has_recurison_error = false;
		epicr::included_recipes.clear();
		accept_count = deny_count = 0;
	}

	void accept()
	{
		CHECK_TESTS_NON_EMPTY()
			if (most_recent_test->test_state == UNEVALUATED)
			{
				most_recent_test->test_state = ACCEPT;
				most_recent_test->err_message = "";
			}
		most_recent_test->accept_count++;
	}

	void deny(std::string err_message)
	{
		CHECK_TESTS_NON_EMPTY();
		most_recent_test->test_state = FAIL;
		most_recent_test->err_message = err_message;
		most_recent_test->deny_count++;
		failed = 1;
	}

	/* Helper function for asserting equality of two strings,
	 * generating an error message in case of inequality */
	void expect_equal_s(const std::string actual, const std::string expected)
	{
		CHECK_TESTS_NON_EMPTY();
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
		while (eline_v.size() < 200 && expected[i] != '\n' && expected[i] != '\0')
			eline_v.push_back(expected[i++]);
		i = temp;
		while (aline_v.size() < 200 && actual[i] != '\n' && actual[i] != '\0')
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
		dif_message[i + 14] = '\0';

		/* insert the expected line, actual line and
		 * difference message into the error message */
		char* err_message = (char*)malloc(512 * sizeof(char));
		sprintf(err_message, "\n%s\n%s\n%s", exp_message, act_message, dif_message);
		deny(err_message);

		/* release allocated mem of strings that aren't needed anymore */
		free(act_message);
		free(exp_message);
	}

	/* Helper function for asserting equality of two integers,
	 * generating an error message in case of inequality */
	void expect_equal_i(const int actual, const int expected)
	{
		CHECK_TESTS_NON_EMPTY()

			if (expected == actual)
				accept();
			else
			{
				char* err_message = (char*)malloc(100);
				sprintf(err_message, "\nExpected: %-5i\nActual: %-5i", expected, actual);
				deny(err_message);
			}
	}

	void expect_equal_d(const double actual, const double expected)
	{
		CHECK_TESTS_NON_EMPTY()

			double min = expected > actual ? actual : expected;
		double max = expected > actual ? expected : actual;

		double percent_diff = max / min;

		/* 1% wiggle room */
		if (percent_diff < 1.01)
			accept();
		else
		{
			char* err_message = (char*)malloc(100);
			sprintf(err_message, "\nExpected: %-5lf\nActual: %-5lf", expected, actual);
			deny(err_message);
		}
	}

	void expect_equal_b(const bool actual, const bool expected)
	{
		CHECK_TESTS_NON_EMPTY();

		if (expected == actual)
			accept();
		else
		{
			char* err_message = (char*)malloc(100);
			sprintf(err_message, "\nExpected: %-5s\nActual: %-5s", expected ? "true" : "false", actual ? "true" : "false");
			deny(err_message);
		}
	}

	void expect_exception(epicr::parse_ret parse_ret, std::string err_message)
	{
		if (!parse_ret.has_err)
			test_lib::deny("Parser did not throw an error");
		else
			test_lib::expect_equal_s(parse_ret.err, err_message);
	}

	void expect_warning(epicr::Parser parser, std::string err_message)
	{
		bool found_warning = false;
		for (const auto& warning : parser.warnings)
		{
			if (warning == err_message)
			{
				found_warning = true;
				break;
			}
		}
		if (found_warning)
		{
			accept();
		}
		else
		{
			deny("Did not find warning <" + err_message + "> in Parser!");
		}
	}

	void expect_exception(epicr::rcp_ret rcp_ret, std::string err_message)
	{
		if (!rcp_ret.has_err)
			test_lib::deny("Parser did not throw an error");
		else
			test_lib::expect_equal_s(rcp_ret.err, err_message);
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
					printf("\033[33munev\x1B[0m "); /* colour and status */
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
					printf("%s (%i/%i)\n", test.name.c_str(), test.accept_count, test.accept_count + test.deny_count);
				else
					printf("%s: %s (%i/%i)\n", test.name.c_str(), test.err_message.c_str(), test.accept_count, test.accept_count + test.deny_count);
			}
	}

	/* Get the success state of all tests run.
	 * Should be returned at the end of each test file */
	int result()
	{
		std::ifstream file;
		file.open("./bin/.tests", std::ios_base::in);
		file.seekg(0, std::ios_base::beg);

		/* read current results in the file */
		std::string results = "";
		char ch = file.get();

		while (ch != -1)
		{
			results += ch;
			ch = file.get();
		}

		results += failed ? '1' : '0';
		size_t results_size = results.size();

		/* count number of test files adjacent */
		const auto dir = std::filesystem::directory_iterator("./src/testing/");
		unsigned int test_file_count = 0;
		for (const auto& file : dir)
		{
			std::string fpath{ file.path().u8string() };
			if (ends_with(fpath, ".test.cpp"))
				test_file_count++;
		}

		/* append own result at the end, and return to base */
		std::ofstream o_file;
		o_file.open("./bin/.tests", std::ios_base::app);
		o_file << (failed ? '1' : '0');
		o_file.close();

		if (results_size == test_file_count)
			for (char c : results)
				if (c == '1')
					return 1;
		return 0;
	}
}

bool ends_with(std::string const& value, std::string const& ending)
{
	if (ending.size() > value.size())
		return false;
	return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}