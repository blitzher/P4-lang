#include "./epicr.h"

#define ERR(msg, token)      \
	{                        \
		error = true;        \
		error_message = msg; \
		error_token = token; \
	}
#define ADV(count)                                  \
	{                                               \
		for (int __i = 0; __i < count; __i++)       \
		{                                           \
			ctoken = lexer->next_non_blank_token(); \
			utoken = lexer->peek_non_blank_token(); \
		}                                           \
	}

std::string opt_fields[] = {

	"description",
	"amount",
	"nutrients",
	"kitchenware",
	"tags",
	"time",
};

namespace epicr
{

#pragma region Parser implementation
	Parser::Parser(Lexer *lexer_r)
	{
		lexer = lexer_r;
	}

	recipe Parser::Parse()
	{
		error = 0;
		error_message = "No error";
		recipe rcp;
		epicr_token ctoken = lexer->next_non_blank_token();
		epicr_token utoken = lexer->peek_non_blank_token();

		if (ctoken.word != "title" || utoken.type != ETT_COLON)
			ERR("No 'title:' found at the start of the file!", ctoken);

		ADV(1);
		print_token(ctoken);
		ADV(1);
		print_token(ctoken);

		while (utoken.type != ETT_COLON)
		{

			rcp.title += ctoken.word + " ";
			ADV(1);
		}

		return rcp;
	}

	Parser::~Parser()
	{
	}
#pragma endregion
}