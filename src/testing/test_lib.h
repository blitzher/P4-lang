#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include "../epicr.h"

typedef unsigned int uint;

namespace test_lib
{
#define REGISTER register_test(__func__)

	enum state
	{
		UNEVALUATED = -1,
		FAIL,
		ACCEPT,
	};

	typedef struct test_data_s
	{
		std::string name;
		state test_state;
		std::string err_message;

	} test_data;

	/* Signals to the testing lib */
	void register_test(const std::string test_name);
	void accept();
	void deny(std::string err_message);

	/* Helper functions for asserting values */
	void expect_equal_s(const std::string actual, const std::string expected);
	void expect_equal_i(const int actual, const int expected);
	void expect_equal_d(const double actual, const double expected);

	/* Final call in main, after tests have been run */
	void print_recap();
	// ---^^^---vvvv---:>~

	int was_success();
}