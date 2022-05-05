#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include "../epicr.h"
#include <filesystem>

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
		int accept_count;
		int deny_count;

	} test_data;

	/* Signals to the testing lib */
	/**
	 * @brief Function to register a test
	 *
	 * @param test_name input test name
	 */
	void register_test(const std::string test_name);
	/**
	 * @brief Function that accepts the test if it went as expected
	 *
	 */
	void accept();
	/**
	 * @brief Function the denys the test if it didnt go as expected
	 *
	 * @param err_message, error message
	 */
	void deny(std::string err_message);

	/* Helper functions for asserting values */
	/**
	 * @brief Function that compares 2 strings
	 * If they match it accepts the test,
	 * if they dont it gets denied
	 *
	 * @param actual input string
	 * @param expected input string
	 */
	void expect_equal_s(const std::string actual, const std::string expected);
	/**
	 * @brief Function that compares 2 integers
	 * If they match it accepts the test,
	 * if they dont it gets denied
	 *
	 * @param actual input int
	 * @param expected input int
	 */
	void expect_equal_i(const int actual, const int expected);
	/**
	 * @brief Function that compares 2 doubles
	 * If they match it accepts the test,
	 * if they dont it gets denied
	 *
	 * @param actual input double
	 * @param expected input double
	 */
	void expect_equal_d(const double actual, const double expected);
	/**
	 * @brief Function that compares 2 booleans
	 * If they match it accepts the test,
	 * if they dont it gets denied
	 *
	 * @param actual input bool
	 * @param expected input bool
	 */
	void expect_equal_b(const bool actual, const bool expected);
	/**
	 * @brief Function expects an exception,
	 * if the error message matches the expected error message
	 * it is accepted, if not it is denied
	 *
	 * @param parse_ret input parser return object
	 * @param err_message input error message
	 */
	void expect_exception(epicr::parse_ret parse_ret, std::string err_message);
	/**
	 * @brief Function expects an exception,
	 * if the error message matches the expected error message
	 * it is accepted, if not it is denied
	 *
	 * @param rcp_ret input recipe return object
	 * @param err_message input error message
	 */
	void expect_exception(epicr::rcp_ret rcp_ret, std::string err_message);
	/**
	 * @brief Function expects a warning,
	 * if the error message matches the expected error message
	 * it is accepted, if not it is denied
	 *
	 * @param parser input parser object
	 * @param err_message input error message (warning)
	 */
	void expect_warning(epicr::Parser parser, std::string err_message);
	/* Final call in main, after tests have been run */
	/**
	 * @brief Prints the recap of the tests
	 *
	 */
	void print_recap();
	// ---^^^---vvvv---:>~
	/**
	 * @brief Function that counts the amount of successes in each test
	 *
	 * @return int, the amount passed
	 */
	int result();
}
/**
 * @brief returns whether or not the string end with a substring.
 *
 * @param value the string to check on.
 * @param ending the substring to check with.
 * @return true, if the string ends with the substring.
 * @return false if the string does not end with the substring.
 */
bool ends_with(std::string const &value, std::string const &ending);