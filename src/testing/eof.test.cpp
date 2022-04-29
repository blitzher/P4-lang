#include "./test_lib.h"

void stolen_expect_token_type(epicr::epicr_token actual, epicr::epicr_token_type expected)
{
	if (actual.type != expected)
	{
		char *err_msg = (char *)malloc(100);
		sprintf(err_msg, "\nActual  : %s (%s)\nExpected: %s",
				epicr::token_to_string(actual.type).c_str(), actual.word.c_str(),
				epicr::token_to_string(expected).c_str());
		test_lib::deny(err_msg);
	}
	test_lib::accept();
}

void stolen_basic_test()
{
	test_lib::REGISTER;

	std::istringstream test_string("hello!");
	epicr::Lexer lexer(test_string);
	epicr::epicr_token tokens[]{lexer.next_token(), lexer.next_token()};
	stolen_expect_token_type(tokens[0], epicr::E_TT_WORD);
	test_lib::expect_equal_s("hello!", tokens[0].word);
	stolen_expect_token_type(tokens[1], epicr::E_TT_EOF);
}

void stolen_sentence_test()
{
	test_lib::REGISTER;

	std::istringstream test_string("hello there, this is a sentence!");
	epicr::Lexer lexer(test_string);
	std::vector<epicr::epicr_token> tokens = lexer.next_token(13);
	test_lib::expect_equal_s("hello", tokens[0].word);
	stolen_expect_token_type(tokens[1], epicr::E_TT_BLANK);
	test_lib::expect_equal_s("there", tokens[2].word);
	stolen_expect_token_type(tokens[3], epicr::E_TT_COMMA);
	stolen_expect_token_type(tokens[4], epicr::E_TT_BLANK);
	test_lib::expect_equal_s("this", tokens[5].word);
	stolen_expect_token_type(tokens[6], epicr::E_TT_BLANK);
	test_lib::expect_equal_s("is", tokens[7].word);
	stolen_expect_token_type(tokens[8], epicr::E_TT_BLANK);
	test_lib::expect_equal_s("a", tokens[9].word);
	stolen_expect_token_type(tokens[10], epicr::E_TT_BLANK);
	test_lib::expect_equal_s("sentence!", tokens[11].word);
	stolen_expect_token_type(tokens[12], epicr::E_TT_EOF);
}

int main(void)
{
	stolen_basic_test();
    stolen_sentence_test();
    
	test_lib::print_recap();
	return test_lib::result();
}
