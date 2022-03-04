#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

typedef unsigned int uint;

namespace test_lib
{
#define REGISTER register_test(__FUNCTION__)
	typedef struct test_data_s
	{
		std::string name;
		bool accepted;
		std::string err_message;

	} test_data;

	/* Signal to the testing lib */
	void register_test(const std::string test_name);
	void accept();
	void deny(std::string err_message);
	void expect_equal_s(const std::string expected, const std::string actual);
	void expect_equal_i(const int expected, const int actual);
	void print_recap();
	// ---^^^---vvvv---:>~

	int was_success();
}