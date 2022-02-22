#include "./epicr.h"

namespace epicr
{

#pragma region Parser implementation
	Parser::Parser(Lexer *lexer_r)
	{
		lexer = lexer_r;
	}

	Parser::~Parser()
	{
	}
#pragma endregion
}