#include "./test_lib.h"

void expect_token_type(epicr::epicr_token actual, epicr::epicr_token_type expected)
{
	test_lib::expect_equal_s(
		epicr::token_to_string(actual.type),
		epicr::token_to_string(expected));
}

void basic_test()
{
	test_lib::REGISTER;

	std::istringstream test_string("hello!");
	epicr::Lexer lexer(test_string);
	epicr::epicr_token tokens[]{lexer.next_token(), lexer.next_token()};
	expect_token_type(tokens[0], epicr::ETT_WORD);
	test_lib::expect_equal_s("hello!", tokens[0].word);
	expect_token_type(tokens[1], epicr::ETT_EOF);
}

void special_characters()
{
	test_lib::REGISTER;

	/* should split them up into 'hello' and a bunch of single length tokens */
	std::istringstream test_string("hello:?+*lg");
	epicr::Lexer lexer(test_string);
	std::vector<epicr::epicr_token> tokens = lexer.next_token(7);

	expect_token_type(tokens[0], epicr::ETT_WORD);
	expect_token_type(tokens[1], epicr::ETT_COLON);

	expect_token_type(tokens[2], epicr::ETT_SPECIAL_OPR_Q);
	expect_token_type(tokens[3], epicr::ETT_SPECIAL_OPR_P);
	expect_token_type(tokens[4], epicr::ETT_SPECIAL_OPR_A);

	expect_token_type(tokens[5], epicr::ETT_WORD);
	expect_token_type(tokens[6], epicr::ETT_EOF);
}

void blank_runs()
{
	test_lib::REGISTER;

	/* should split them up into 'hello' and a bunch of single length tokens */
	std::istringstream test_string("  \n\nhello!\n\n\n");
	epicr::Lexer lexer(test_string);
	std::vector<epicr::epicr_token> tokens = lexer.next_token(5);
	expect_token_type(tokens[0], epicr::ETT_BLANK);
	expect_token_type(tokens[1], epicr::ETT_NEWLINE);
	expect_token_type(tokens[2], epicr::ETT_WORD);
	expect_token_type(tokens[3], epicr::ETT_NEWLINE);
	expect_token_type(tokens[4], epicr::ETT_EOF);
}

int main(void)
{
	basic_test();
	special_characters();
	blank_runs();
	test_lib::print_recap();
	return test_lib::was_success();
}
