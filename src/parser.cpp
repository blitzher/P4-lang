#include "./epicr.h"

#define ERR(msg, token)      \
	{                        \
		error = true;        \
		error_message = msg; \
		error_token = token; \
		return;              \
	}
#define ADV_NON_BLANK(count)                        \
	{                                               \
		for (int __i = 0; __i < count; __i++)       \
		{                                           \
			ctoken = lexer->next_non_blank_token(); \
			utoken = lexer->peek_non_blank_token(); \
		}                                           \
	}
#define ADV(count)                            \
	{                                         \
		for (int __i = 0; __i < count; __i++) \
		{                                     \
			ctoken = lexer->next_token();     \
			utoken = lexer->peek_token();     \
		}                                     \
	}

std::vector<std::string> optional_fields = {

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
		recipe *rcp = (recipe *)calloc(1, sizeof(recipe));
		ctoken = lexer->next_non_blank_token();
		utoken = lexer->peek_non_blank_token();

		ParseTitle(rcp);
		/* Parse all optional fields */
		while (to_lower(ctoken.word) != "ingredients" && ctoken.type != ETT_EOF)
		{
			if (to_lower(ctoken.word) == "description")
				ParseDescription(rcp);
			else if (to_lower(ctoken.word) == "amount")
				ParseAmount(rcp);
			else if (to_lower(ctoken.word) == "nutrients")
				ParseNutrients(rcp);
			else if (to_lower(ctoken.word) == "kitchenware")
				ParseKitchenware(rcp);
			else if (to_lower(ctoken.word) == "tags")
				ParseTags(rcp);
			else if (to_lower(ctoken.word) == "time")
				ParseTime(rcp);

			print_token(ctoken);
			ADV(1);
		}
		return *rcp;
	}

	void Parser::ParseTitle(recipe *rcp)
	{
		if (to_lower(ctoken.word) != "title" || utoken.type != ETT_COLON)
			ERR("No 'title:' found at the start of the file!", ctoken);

		ADV_NON_BLANK(2);

		/* Read all words and spaces in title */
		while (utoken.type != ETT_COLON)
		{
			rcp->title += ctoken.word;
			std::cout << "Reading title" << std::endl;
			ADV(1);
		}
	}
	void Parser::ParseDescription(recipe *rcp)
	{
		ADV_NON_BLANK(2);

		/* Read all words and spaces in description */
		while (utoken.type != ETT_COLON)
		{
			rcp->description += ctoken.word;
			std::cout << "Reading description" << std::endl;
			ADV(1);
		}
	}
	void Parser::ParseAmount(recipe *rcp) {}
	void Parser::ParseNutrients(recipe *rcp) {}
	void Parser::ParseKitchenware(recipe *rcp) {}
	void Parser::ParseTags(recipe *rcp) {}
	void Parser::ParseTime(recipe *rcp) {}
	void Parser::ParseIngredients(recipe *rcp) {}
	void Parser::ParseProcedure(recipe *rcp) {}

	Parser::~Parser()
	{
	}
#pragma endregion
}