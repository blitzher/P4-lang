#include "./epicr.h"

#pragma region Helper macros

#define ERR(msg, token)                                    \
	{                                                      \
		error = true;                                      \
		error_message = msg;                               \
		error_token = token;                               \
		if (!silent)                                       \
		{                                                  \
			std::cout << "ERROR ON LINE " << __LINE__      \
					  << " (" << __FILE__ << ":"           \
					  << __FUNCTION__ << ")" << std::endl; \
			std::cout << msg << std::endl;                 \
			print_token(token);                            \
			std::cout << std::endl;                        \
		}                                                  \
	}

#define ERR_VOID(msg, token) \
	{                        \
		ERR(msg, token);     \
		return;              \
	}

#define ADV_NON_BLANK(count)                        \
	{                                               \
		for (int __i = 0; __i < count; __i++)       \
		{                                           \
			ctoken = lexer->next_non_blank_token(); \
		}                                           \
		utoken = lexer->peek_non_blank_token();     \
	}
#define ADV(count)                            \
	{                                         \
		for (int __i = 0; __i < count; __i++) \
		{                                     \
			ctoken = lexer->next_token();     \
		}                                     \
		utoken = lexer->peek_token();         \
	}
#pragma endregion

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
		recipe rcp = recipe();
		ctoken = lexer->next_non_blank_token();
		utoken = lexer->peek_non_blank_token();

		/* Parse all fields */
		while (ctoken.type != E_TT_EOF)
		{

			/* If an error occured during parsing,
			 * return what was parsed so far */
			if (error)
				return rcp;
			/* TODO: refactor x */
			if (to_lower(ctoken.word) == "title")
				ParseTitle(&rcp);
			else if (to_lower(ctoken.word) == "description")
				ParseDescription(&rcp);
			else if (to_lower(ctoken.word) == "servings")
				ParseServings(&rcp);
			else if (to_lower(ctoken.word) == "nutrients")
				ParseNutrients(&rcp);
			else if (to_lower(ctoken.word) == "kitchenware")
				ParseKitchenware(&rcp);
			else if (to_lower(ctoken.word) == "tags")
				ParseTags(&rcp);
			else if (to_lower(ctoken.word) == "prep-time" ||
					 to_lower(ctoken.word) == "cook-time" ||
					 to_lower(ctoken.word) == "total-time")
				ParseTime(&rcp);
			else if (to_lower(ctoken.word) == "ingredients")
				ParseIngredients(&rcp);
			else if (to_lower(ctoken.word) == "instructions")
				ParseInstructions(&rcp);
			/*if there is a colon as the utoken, and it didn't match any of the other statements,
			then it must be an invalid field */
			else if (utoken.type == E_TT_COLON)
			{
				ERR("invalid field: No field with this name", ctoken);
				return rcp;
			}
			else
			{
				ADV(1);
			}
		}
		return rcp;
	}

	void Parser::ParseTitle(recipe *rcp)
	{
		ADV_NON_BLANK(2);

		/* Read all words and spaces in title */
		while (utoken.type != E_TT_COLON && ctoken.type != E_TT_EOF)
		{
			rcp->title = ReadWords(true, false);
			if (utoken.type != E_TT_COLON)
				ADV_NON_BLANK(1);
		}

		rcp->title = strip_spaces_right(rcp->title);
	}
	void Parser::ParseDescription(recipe *rcp)
	{
		ADV_NON_BLANK(2);
		/* Read all words and spaces in description */
		while (utoken.type != E_TT_COLON)
		{
			rcp->description += ctoken.word;
			ADV(1);
		}
		rcp->description = strip_spaces_right(rcp->description);
	}
	void Parser::ParseServings(recipe *rcp)
	{
		ADV_NON_BLANK(2);
		if (ctoken.type != E_TT_NUMBER || utoken.type == E_TT_EOF)
		{
			ERR_VOID("No correct description for amount has been found!", ctoken);
		}
		rcp->servings.count += stoi(ctoken.word);

		ADV_NON_BLANK(1);
		rcp->servings.descriptor = ReadWords(false, false);
	}

	void Parser::ParseNutrients(recipe *rcp)
	{
		ADV_NON_BLANK(2);

		std::vector<ingredient> nutrients;

		while (utoken.type != E_TT_COLON)
		{
			ingredient nutrient = ReadIngredient(0);

			if (nutrient.amount.unit != "kcal" && nutrient.amount.unit != "cal" && nutrient.amount.unit != "g")
				ERR_VOID("Invalid unit after nutrient", ctoken);

			nutrients.push_back(nutrient);
			if (ReadSeperatorOrWaitAtNextField())
				ERR_VOID("expected a comma as a seperator between nutrients.", ctoken);
		}

		rcp->nutrients = nutrients;
	}

	void Parser::ParseKitchenware(recipe *rcp)
	{
		ADV_NON_BLANK(2);
		while (utoken.type != E_TT_COLON && ctoken.type != E_TT_EOF)
		{
			std::string kitchenware = ReadWords(true, false);
			rcp->kitchenware.push_back(kitchenware);

			if (ReadSeperatorOrWaitAtNextField())
				ERR_VOID("expected a comma as a seperator between kitchenware.", ctoken);
		}
	}

	void Parser::ParseTags(recipe *rcp)
	{
		ADV_NON_BLANK(2);

		if (utoken.type == E_TT_COLON || utoken.type == E_TT_EOF)
		{
			ERR_VOID("No tags where found!", ctoken);
		}

		while (utoken.type != E_TT_COLON && ctoken.type != E_TT_EOF)
		{
			std::string tag = ReadWords(true, true);
			rcp->tags.push_back(tag);
			if (ReadSeperatorOrWaitAtNextField())
				ERR_VOID("expected a comma as a seperator between tags.", ctoken);
		}
	}

	void Parser::ParseTime(recipe *rcp)
	{
		/*saves which kind of time it is: */
		std::string timeType = to_lower(ctoken.word);
		ADV_NON_BLANK(2);
		std::string time = "";
		while (utoken.type != E_TT_COLON && ctoken.type != E_TT_EOF)
		{
			time += ctoken.word;
			ADV(1);
		}
		time = strip_spaces_right(time);
		if (timeType == "prep-time")
			rcp->time.prep_time = time;
		else if (timeType == "cook-time")
			rcp->time.cook_time = time;
		else /*total-time*/
			rcp->time.total_time = time;
	}

	void Parser::ParseIngredients(recipe *rcp)
	{
		ADV_NON_BLANK(2);
		while (utoken.type != E_TT_COLON && ctoken.type != E_TT_EOF)
		{
			// ADV_NON_BLANK(1);
			// ingredient ingr = ReadIngredient(HAS_PLUS | HAS_ASTERIX | HAS_QMARK | ASSUME_1_NUM);
			ingredient ingr = ReadIngredient(HAS_PLUS | HAS_ASTERIX | HAS_QMARK | ASSUME_1_NUM);
			rcp->ingredients.push_back(ingr);
			if (ReadSeperatorOrWaitAtNextField())
				ERR_VOID("expected a comma as a seperator between ingredients.", ctoken);
		}
	}

	void Parser::ParseInstructions(recipe *rcp)
	{
		ADV_NON_BLANK(2);

		while (utoken.type != E_TT_COLON && utoken.type != E_TT_EOF)
		{
			instruction singleInstruction = instruction();
			if (to_lower(ctoken.word) == "with")
			{
				ADV_NON_BLANK(1)
				ParseInstructionHeaderWith(&singleInstruction);
			}
			if (to_lower(ctoken.word) == "using")
			{
				ADV_NON_BLANK(1)
				ParseInstructionHeaderUsing(&singleInstruction);
			}
			if (ctoken.type != E_TT_COLON)
			{
				ERR_VOID("missing ':' after instruction header", ctoken);
			}
			ADV_NON_BLANK(1);
			ParseInstructionBody(&singleInstruction);
			if (to_lower(ctoken.word) == "yield" && utoken.type == E_TT_COLON)
			{
				ADV_NON_BLANK(2);
				ParseInstructionYield(&singleInstruction);
			}

			rcp->instructions.push_back(singleInstruction); // something doesnt work here
		}
	}
	void Parser::ParseInstructionHeaderWith(instruction *singleInstruction)
	{
		if (ctoken.type != E_TT_PARENS_OPEN)
		{
			ERR_VOID("expected open bracket with 'with' ", ctoken);
		}
		while (ctoken.type != E_TT_PARENS_CLOSE) /*reads every ingredient in the "with"*/
		{
			ADV_NON_BLANK(1);
			ingredient currentIngredient = ReadIngredient(ASSUME_REST);
			if (ctoken.type != E_TT_COMMA && ctoken.type != E_TT_PARENS_CLOSE)
			{
				ERR_VOID("Expected a ',' as seperator between ingredient or a closing parenthesis for the 'with'", ctoken);
			}
			singleInstruction->ingredients.push_back(currentIngredient);
		}
		ADV_NON_BLANK(1) /*reads through the end parenthesis*/
	}
	void Parser::ParseInstructionHeaderUsing(instruction *singleInstruction)
	{
		if (ctoken.type != E_TT_PARENS_OPEN)
			ERR_VOID("expected open bracket with 'using' ", ctoken);

		while (ctoken.type != E_TT_PARENS_CLOSE) /*reads every kitchenware in the "using"*/
		{
			ADV_NON_BLANK(1);
			if (ctoken.type != E_TT_WORD && ctoken.type != E_TT_NUMBER)
				ERR_VOID("expected a kitchenware", ctoken);

			std::string currentKitchenware = ReadWords(true, false);
			if (ctoken.type != E_TT_COMMA && ctoken.type != E_TT_PARENS_CLOSE)
			{
				ERR_VOID("Expected a ',' as seperator between kitchenware or a closing bracket for the 'using'", ctoken);
			}
			singleInstruction->kitchenware.push_back(currentKitchenware);
		}
		ADV_NON_BLANK(1); /*reads through the end parenthesis*/
	}
	void Parser::ParseInstructionBody(instruction *currentInstruction)
	{
		std::vector<instruction_word> Body;

		while (utoken.type != E_TT_COLON && utoken.type != E_TT_EOF)
		{
			// has yield or update
			if (ctoken.word == "yield" && utoken.type == E_TT_COLON)
			{
				break;
			}
			// has not yield or update
			if ((ctoken.word == "with" || ctoken.word == "using") && utoken.type == E_TT_PARENS_OPEN)
			{
				break;
			}

			instruction_word iword = instruction_word();

			if (ctoken.type == E_TT_BRACKET_OPEN)
			{
				amount amnt = ReadAmount(0);
				iword.is_amount = true;
				iword.value = amnt;
			}
			else if (ctoken.type == E_TT_NEWLINE)
			{
				iword.spelling = ctoken.word;
				ADV_NON_BLANK(1);
			}
			else
			{
				iword.spelling = ctoken.word;
				ADV(1);
			}
			Body.push_back(iword);
		}
		currentInstruction->body = Body;
	}

	void Parser::ParseInstructionYield(instruction *singleInstruction)
	{
		do
		{
			if (ctoken.type == E_TT_COMMA)
				ADV_NON_BLANK(1);
			if (ctoken.type == E_TT_EOF)
				break;
			ingredient currentYield = ReadIngredient(HAS_PLUS);
			singleInstruction->yields.push_back(currentYield);
		} while (ctoken.type == E_TT_COMMA);
	}
	ingredient Parser::ReadIngredient(ingredient_arg arg)
	{
		bool canHaveAsterix = (arg & HAS_ASTERIX) >> 1;
		bool canHaveQmark = (arg & HAS_QMARK) >> 2;
		ingredient currentIngredient = ingredient();

		amount ingredientAmount = amount();
		if (ctoken.type != E_TT_WORD && ctoken.type != E_TT_NUMBER)
		{
			ERR("Expected ingredient name", ctoken);
		}
		currentIngredient.name = ReadWords(true, false);

		while (ctoken.type == E_TT_ASTERIX || ctoken.type == E_TT_QUESTION_MARK)
		{
			if (ctoken.type == E_TT_ASTERIX)
			{
				if (!canHaveAsterix)
				{
					ERR("An asterix is not valid in the given context", ctoken);
					return currentIngredient;
				}
				if (currentIngredient.isIngredientRef)
				{
					ERR("Duplicate asterix", ctoken);
				} // should be a warning
				currentIngredient.isIngredientRef = true;
			}
			if (ctoken.type == E_TT_QUESTION_MARK)
			{
				if (!canHaveQmark)
				{
					ERR("A question mark is not valid in the given context", ctoken);
					return currentIngredient;
				}
				else if (currentIngredient.isOptional)
				{
					ERR("Duplicate question mark", ctoken); // should be a warning
				}
				currentIngredient.isOptional = true;
			}
			ADV_NON_BLANK(1);
		}

		ingredientAmount = ReadAmount(arg);
		currentIngredient.amount = ingredientAmount;
		return currentIngredient;
	}

	amount Parser::ReadAmount(ingredient_arg arg)
	{
		bool canHavePlus = (arg & HAS_PLUS);
		bool assume_1_num = (arg & ASSUME_1_NUM) >> 3;
		bool assume_rest = (arg & ASSUME_REST) >> 4;
		amount amnt = amount();

		if (ctoken.type == E_TT_PLUS)
		{
			if (!canHavePlus)
			{
				ERR("Disallowed plus in amount", ctoken);
				return amnt;
			}
			amnt.isUncountable = true;
			amnt.number = std::numeric_limits<double>::infinity();
			ADV_NON_BLANK(1);
			return amnt;
		}

		if (ctoken.type != E_TT_BRACKET_OPEN)
		{
			if (assume_1_num)
			{
				amnt.number = 1;
				amnt.unit = "";
			}
			else if (assume_rest)
			{
				amnt.isRelativeAmount = true;
				amnt.relativeAmount = "rest";
			}
			return amnt;
		}

		ADV_NON_BLANK(1);

		if (ctoken.type == E_TT_NUMBER)
		{
			amnt.number = std::stod(ctoken.word);
			ADV_NON_BLANK(1);
			amnt.unit = ReadWords(false, false);
		}
		else if (ctoken.type == E_TT_WORD)
		{
			amnt.isRelativeAmount = true;
			amnt.relativeAmount = ReadWords(false, false);

			std::string validRelatives[]{"rest", "quarter", "half", "all"};

			bool isValid = false;
			size_t i;
			for (i = 0; i < 4; i++)
			{
				if (validRelatives[i] == amnt.relativeAmount)
				{
					isValid = true;
					break;
				}
			}

			if (!isValid)
			{
				ERR("Invalid relative amount", ctoken);
				return amnt;
			}
		}
		else
		{
			ERR("Did not find number or word in amount", ctoken);
			return amnt;
		}

		ADV_NON_BLANK(1);

		return amnt;
	}

	std::string Parser::ReadWords(bool canReadNumbers, bool canReadParenthesis)
	{
		std::string finalWord = "";

		while (ReadWordsPredicate(ctoken.type, canReadNumbers, canReadParenthesis))
		{
			finalWord += ctoken.word;
			ADV(1);
		}

		if (ctoken.type == E_TT_NEWLINE)
			ADV_NON_BLANK(1);

		return strip_spaces_right(finalWord);
	}

	bool Parser::ReadWordsPredicate(epicr_token_type ctokenType, bool canReadNumbers, bool canReadParenthesis)
	{
		switch (ctokenType)
		{
		case E_TT_WORD:
		case E_TT_BLANK:
			return true;
		case E_TT_NUMBER:
			if (canReadNumbers)
				return true;
			return false;
		case E_TT_PARENS_OPEN:
		case E_TT_PARENS_CLOSE:
			if (canReadParenthesis)
				return true;
			return false;
		default:
			return false;
		}
	}

	int Parser::ReadSeperatorOrWaitAtNextField()
	{
		if (ctoken.type == E_TT_COMMA)
		{
			ADV_NON_BLANK(1);
			return 0;
		}
		if (utoken.type != E_TT_COLON && ctoken.type != E_TT_EOF)
		{
			return 1;
		}
		return 0;
	}

	void Parser::silence(bool val)
	{
		silent = val;
	}

	Parser::~Parser()
	{
	}
#pragma endregion
}
