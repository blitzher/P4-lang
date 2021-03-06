#include "./test_lib.h"

void expect_token_type(epicr::epicr_token actual, epicr::epicr_token_type expected)
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

void basic_test()
{
	test_lib::REGISTER;

	std::istringstream test_string("hello!");
	epicr::Lexer lexer(test_string);
	epicr::epicr_token tokens[]{lexer.next_token(), lexer.next_token()};
	expect_token_type(tokens[0], epicr::E_TT_WORD);
	test_lib::expect_equal_s("hello!", tokens[0].word);
	expect_token_type(tokens[1], epicr::E_TT_EOF);
}

void sentence_test()
{
	test_lib::REGISTER;

	std::istringstream test_string("hello there, this is a sentence!");
	epicr::Lexer lexer(test_string);
	std::vector<epicr::epicr_token> tokens = lexer.next_token(13);
	test_lib::expect_equal_s("hello", tokens[0].word);
	expect_token_type(tokens[1], epicr::E_TT_BLANK);
	test_lib::expect_equal_s("there", tokens[2].word);
	expect_token_type(tokens[3], epicr::E_TT_COMMA);
	expect_token_type(tokens[4], epicr::E_TT_BLANK);
	test_lib::expect_equal_s("this", tokens[5].word);
	expect_token_type(tokens[6], epicr::E_TT_BLANK);
	test_lib::expect_equal_s("is", tokens[7].word);
	expect_token_type(tokens[8], epicr::E_TT_BLANK);
	test_lib::expect_equal_s("a", tokens[9].word);
	expect_token_type(tokens[10], epicr::E_TT_BLANK);
	test_lib::expect_equal_s("sentence!", tokens[11].word);
	expect_token_type(tokens[12], epicr::E_TT_EOF);
}

void dash_word_test()
{
	test_lib::REGISTER;

	/* should split them up into 'hello' and a bunch of single length tokens */
	std::istringstream test_string("hello-there");
	epicr::Lexer lexer(test_string);
	epicr::epicr_token tok = lexer.next_token();
	expect_token_type(tok, epicr::E_TT_WORD);
	test_lib::expect_equal_s(tok.word, "hello-there");
}

void special_characters_token_breakers()
{
	test_lib::REGISTER;

	/* should split them up into 'hello' and a bunch of single length tokens */
	std::istringstream test_string("hello:?+*lg");

	epicr::Lexer lexer(test_string);
	std::vector<epicr::epicr_token> tokens = lexer.next_token(7);

	expect_token_type(tokens[0], epicr::E_TT_WORD);
	expect_token_type(tokens[1], epicr::E_TT_COLON);

	expect_token_type(tokens[2], epicr::E_TT_QUESTION_MARK);
	expect_token_type(tokens[3], epicr::E_TT_PLUS);
	expect_token_type(tokens[4], epicr::E_TT_ASTERIX);

	expect_token_type(tokens[5], epicr::E_TT_WORD);
	expect_token_type(tokens[6], epicr::E_TT_EOF);
}

void special_characters_non_token_breakers()
{
	test_lib::REGISTER;

	std::istringstream test_string("[]@ ??????,??\033\x1b");

	epicr::Lexer lexer(test_string);
	std::vector<epicr::epicr_token> tokens = lexer.next_token(8);

	expect_token_type(tokens[0], epicr::E_TT_BRACKET_OPEN);
	expect_token_type(tokens[1], epicr::E_TT_BRACKET_CLOSE);

	expect_token_type(tokens[2], epicr::E_TT_WORD); /* @ */
	expect_token_type(tokens[3], epicr::E_TT_BLANK);

	expect_token_type(tokens[4], epicr::E_TT_WORD);	 /* ?????? */
	expect_token_type(tokens[5], epicr::E_TT_COMMA); /* , */

	expect_token_type(tokens[6], epicr::E_TT_WORD); /* ?? */

	expect_token_type(tokens[7], epicr::E_TT_EOF);
}

/* carriage return means that it moves the cursor to the left.
It differs from windows to unix because of line endings.
Line endings on unix is LF and on windows it is CRLF */
void carriage_return()
{
	test_lib::REGISTER;

	std::istringstream test_string("\r\n\r\n");

	epicr::Lexer lexer(test_string);
	std::vector<epicr::epicr_token> tokens = lexer.next_token(3);

	expect_token_type(tokens[0], epicr::E_TT_NEWLINE);
	test_lib::expect_equal_i(tokens[0].word.size(), 2);
	expect_token_type(tokens[1], epicr::E_TT_EOF);
}

void blank_runs()
{
	test_lib::REGISTER;

	/* should split them up into 'hello' and a bunch of single length tokens */
	std::istringstream test_string("  \n\nhello!\n\n\n");
	epicr::Lexer lexer(test_string);
	std::vector<epicr::epicr_token> tokens = lexer.next_token(5);
	expect_token_type(tokens[0], epicr::E_TT_BLANK);
	expect_token_type(tokens[1], epicr::E_TT_NEWLINE);
	expect_token_type(tokens[2], epicr::E_TT_WORD);
	expect_token_type(tokens[3], epicr::E_TT_NEWLINE);
	expect_token_type(tokens[4], epicr::E_TT_EOF);
}

void valid_and_invalid_numbers()
{
	test_lib::REGISTER;

	std::istringstream test_string("0\n0.0\n0.\n0.0.0\n.0");
	epicr::Lexer lexer(test_string);
	std::vector<epicr::epicr_token> tokens = lexer.next_token(10);
	expect_token_type(tokens[0], epicr::E_TT_NUMBER);
	expect_token_type(tokens[2], epicr::E_TT_NUMBER);
	expect_token_type(tokens[4], epicr::E_TT_NUMBER);
	expect_token_type(tokens[6], epicr::E_TT_WORD);
	expect_token_type(tokens[8], epicr::E_TT_NUMBER);
}

int main(void)
{
	basic_test();
	dash_word_test();
	sentence_test();
	special_characters_token_breakers();
	special_characters_non_token_breakers();
	carriage_return();
	blank_runs();
	valid_and_invalid_numbers();
	test_lib::print_recap();
	return test_lib::result();
}
