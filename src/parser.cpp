#include "./epicr.h"

#pragma region Helper macros
#define ERR(msg, token)                                \
	{                                                  \
		error = true;                                  \
		error_message = msg;                           \
		error_token = token;                           \
		std::cout << "ERROR ON LINE " << __LINE__      \
				  << " (" << __FILE__ << ":"           \
				  << __FUNCTION__ << ")" << std::endl; \
		std::cout << msg << std::endl;                 \
		print_token(token);                            \
		std::cout << std::endl;                        \
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

	/*
	class Bla {


	}

	 */

#pragma region Parser implementation
	Parser::Parser(Lexer *lexer_r)
	{
		lexer = lexer_r;
	}

	recipe Parser::Parse()
	{
		error = 0;
		error_message = "No error";
		recipe *rcp = new recipe;
		ctoken = lexer->next_non_blank_token();
		utoken = lexer->peek_non_blank_token();

		ParseTitle(rcp);
		if (error)
			return *rcp;
		/* Parse all optional fields */
		while (to_lower(ctoken.word) != "ingredients" && ctoken.type != ETT_EOF)
		{
			/* If an error occured during parsing,
			 * return what was parsed so far */
			if (error)
				return *rcp;
			/* TODO: refactor x */
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
			else if (to_lower(ctoken.word) == "cook-time")
				ParseTime(rcp);
			else
			{

				ADV(1);
			}
		}
		// ParseIngredients(rcp);
		// for testing - until we get out of the infinite loop in reading ingredient:
		while (ctoken.word != "instructions" && ctoken.type != ETT_EOF)
		{
			ADV_NON_BLANK(1)
		}
		if (to_lower(ctoken.word) != "instructions")
		{
			ERR("expected instructions", ctoken);
			return *rcp;
		}
		ParseInstructions(rcp);

			return *rcp;
	}

	void Parser::ParseTitle(recipe *rcp)
	{
		if (to_lower(ctoken.word) != "title" || utoken.type != ETT_COLON)
		{
			ERR_VOID("No 'title:' found at the start of the file!", ctoken);
		}

		ADV_NON_BLANK(2);

		/* Read all words and spaces in title */
		while (utoken.type != ETT_COLON)
		{
			rcp->title += ctoken.word;
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
			ADV(1);
		}
	}
	void Parser::ParseAmount(recipe *rcp)
	{
		ADV_NON_BLANK(2);
		/*Read the first word and store in amount.count as a number is expected as the first word otherwise throw an error */
		if (ctoken.type != ETT_NUMBER || utoken.type == ETT_EOF)
		{
			ERR_VOID("No correct desciption for amount has been found!", ctoken);
		}
		rcp->amount.count += stoi(ctoken.word);
		//
		// todo: tilføj så man kan skrive 2-4 (altså 2 til 4)
		//
		/*
		 * Advance 1 so it goes to the next word,
		 * which is probably not a blank.
		 * In fact, it is certainly not a blank,
		 * since `ADV_NON_BLANK` never leaves the
		 * current token on a blank, and rather
		 * skips over them until it reaches a non
		 * blank token.
		 * */
		ADV_NON_BLANK(1);
		/* Read all words and spaces in amount */
		while (utoken.type != ETT_COLON && ctoken.type != ETT_EOF)
		{
			rcp->amount.descriptor += ctoken.word;
			ADV(1);
		}
	}

	void Parser::ParseNutrients(recipe *rcp)
	{
		ADV_NON_BLANK(2);

		std::vector<ingredient> nutrients;

		while (utoken.type != ETT_COLON)
		{
			ingredient nutrient = ReadIngredient(0);
			if (nutrient.amount.unit != "kcal" && nutrient.amount.unit != "cal" && nutrient.amount.unit != "g")
			{

				ERR_VOID("Invalid unit after nutrient", ctoken);
			}

			nutrients.push_back(nutrient);

			if (ctoken.type == ETT_COMMA)
			{
				ADV_NON_BLANK(1);
			}
		}

		rcp->nutrients = nutrients;
	}

	void Parser::ParseKitchenware(recipe *rcp)
	{

		ADV_NON_BLANK(2);
		while (utoken.type != ETT_COLON && utoken.type != ETT_EOF)
		{
			if (ctoken.type == ETT_EOF)
			{
				ERR_VOID("@kitchenware, End of file reached:", ctoken);
			}
			else if (ctoken.type == ETT_COMMA)
			{
				ADV_NON_BLANK(1);
			}
			rcp->kitchenware.push_back(ctoken.word);
			ADV_NON_BLANK(1);
		}
	}

	void Parser::ParseTags(recipe *rcp)
	{
		ADV_NON_BLANK(2);

		if (utoken.type == ETT_COLON || utoken.type == ETT_EOF)
		{
			ERR_VOID("No tags where found!", ctoken);
		}

		while (utoken.type != ETT_COLON && ctoken.type != ETT_EOF)
		{
			if (ctoken.type == ETT_COMMA)
			{
				ADV_NON_BLANK(1);
			}
			rcp->tags.push_back(ctoken.word);
			ADV_NON_BLANK(1);
		}
	}

	void Parser::ParseTime(recipe *rcp)
	{
		ADV_NON_BLANK(2);

		if (ctoken.type != ETT_NUMBER || utoken.type == ETT_EOF)
		{
			ERR_VOID("No amount was found!", ctoken);
		}
		while (utoken.type != ETT_COLON && ctoken.type != ETT_EOF)
		{
			rcp->time += ctoken.word;
			ADV(1);
		}
	}

	void Parser::ParseIngredients(recipe *rcp)
	{
		ADV_NON_BLANK(1);
		do
		{
			ADV_NON_BLANK(1);
			ingredient ingr = ReadIngredient(HAS_PLUS | HAS_ASTERIX | HAS_QMARK);
			rcp->ingredients.push_back(ingr);
		} while (ctoken.type != ETT_COMMA);
	}

	void Parser::ParseInstructions(recipe *rcp)
	{
		ADV_NON_BLANK(2);
		std::vector<instruction> instructions;
		while (utoken.type != ETT_EOF)
		{
			if (to_lower(ctoken.word) != "with" && to_lower(ctoken.word) != "using")
			{
				ERR_VOID("expected instruction header, either 'with' or 'using'", ctoken);
			}
			instruction singleInstruction;
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
			if (ctoken.type != ETT_COLON)
			{
				ERR_VOID("missing ':' after instruction header", ctoken);
			}
			ADV_NON_BLANK(1);
			ParseInstructionBody(&singleInstruction);
			if (to_lower(ctoken.word) == "yield" && utoken.type == ETT_COLON)
			{
				ADV_NON_BLANK(2);
				ParseInstructionYield(&singleInstruction);
			}

			rcp->instructions.push_back(singleInstruction); // something doesnt work here
		}
	}
	void Parser::ParseInstructionHeaderWith(instruction *singleInstruction)
	{
		if (ctoken.type != ETT_PARENS_OPEN)
		{
			ERR_VOID("expected open bracket with 'with' ", ctoken);
		}
		while (ctoken.type != ETT_PARENS_CLOSE)
		{
			ADV_NON_BLANK(1);
			ingredient currentIngredient = ReadIngredient(0);
			if (ctoken.type != ETT_COMMA && ctoken.type != ETT_PARENS_CLOSE)
			{
				ERR_VOID("Expected a ',' as seperator between ingredient or a closing parenthesis for the 'with'", ctoken);
			}
			singleInstruction->ingredients.push_back(currentIngredient);
		}
		ADV_NON_BLANK(1)
	}
	void Parser::ParseInstructionHeaderUsing(instruction *singleInstruction)
	{
		if (ctoken.type != ETT_PARENS_OPEN)
		{
			ERR_VOID("expected open bracket with 'using' ", ctoken);
		}

		while (ctoken.type != ETT_PARENS_CLOSE)
		{
			ADV_NON_BLANK(1);
			if (ctoken.type != ETT_WORD)
			{
				ERR_VOID("expected a kitchenware", ctoken);
			}
			std::string currentKitchenware;
			currentKitchenware = ReadWords();

			if (ctoken.type != ETT_COMMA && ctoken.type != ETT_PARENS_CLOSE)
			{
				ERR_VOID("Expected a ',' as seperator between kitchenware or a closing bracket for the 'using'", ctoken);
			}
			singleInstruction->kitchenware.push_back(currentKitchenware);
		}
		ADV_NON_BLANK(1);
	}
	void Parser::ParseInstructionBody(instruction *currentInstruction)
	{
		std::vector<instruction_word> Body;

		while (ctoken.type != ETT_EOF)
		{
			// has yield or update
			if (ctoken.word == "yield" && utoken.type == ETT_COLON)
			{
				break;
			}
			// has not yield or update
			if ((ctoken.word == "with" || ctoken.word == "using") && utoken.type == ETT_PARENS_OPEN)
			{
				break;
			}

			instruction_word iword;

			if (ctoken.type == ETT_BRACKET_OPEN)
			{
				amount amnt = ReadAmount(0);
				iword.is_amount = true;
				iword.value = amnt;
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
		while (to_lower(ctoken.word) != "with" && to_lower(ctoken.word) != "using" && ctoken.type != ETT_EOF)
		{
			ingredient currentYield;
			if (ctoken.type != ETT_WORD)
				ERR_VOID("Expected an ingredient", ctoken);
			currentYield = ReadIngredient(HAS_PLUS);
			singleInstruction->yields.push_back(currentYield);

			if (ctoken.type == ETT_COMMA)
				ADV_NON_BLANK(1)
		}
	}

	ingredient Parser::ReadIngredient(ingredient_arg arg)
	{
		bool canHaveAsterix = (arg & HAS_ASTERIX) >> 1;
		bool canHaveQmark = (arg & HAS_QMARK) >> 2;

		ingredient currentIngredient;
		amount ingredientAmount;
		if (ctoken.type != ETT_WORD)
		{
			ERR("Expected ingredient name", ctoken);
		}
		currentIngredient.name = ReadWords();

		while (ctoken.type == ETT_ASTERIX || ctoken.type == ETT_QUESTION_MARK)
		{
			if (ctoken.type == ETT_ASTERIX)
			{
				if (!canHaveAsterix)
				{
					ERR("An asterix is not valid in the given context", ctoken);
					return currentIngredient;
				}
				if (currentIngredient.isIngredientRef)
					ERR("Duplicate asterix", ctoken); // should be a warning
				currentIngredient.isIngredientRef = true;
			}
			if (ctoken.type == ETT_QUESTION_MARK)
			{
				if (!canHaveQmark)
				{
					ERR("A question mark is not valid in the given context", ctoken);
					return currentIngredient;
				}
				if (currentIngredient.isOptional)
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
		amount amnt;

		if (ctoken.type == ETT_PLUS)
		{
			if (!canHavePlus)
			{
				ERR("Disallowed plus in amount", ctoken);
				return amnt;
			}
			amnt.isUncountable = true;
			ADV_NON_BLANK(1);
			return amnt;
		}

		if (ctoken.type != ETT_BRACKET_OPEN)
		{
			amnt.amount = 1;
			amnt.unit = "";
			return amnt;
		}

		ADV_NON_BLANK(1);

		if (ctoken.type == ETT_NUMBER)
		{
			amnt.amount = std::stod(ctoken.word);
			ADV_NON_BLANK(1);
			amnt.unit = ReadWords();
		}
		else if (ctoken.type == ETT_WORD)
		{
			amnt.isRelativeAmount = true;
			amnt.relativeAmount = ReadWords();

			std::string validRelatives[]{"rest", "quarter", "half", "all"};

			bool isValid = false;
			for (size_t i = 0; i < 4; i++)
			{
				if (validRelatives[i] == amnt.relativeAmount)
					isValid = true;
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

	std::string Parser::ReadWords()
	{
		std::string finalWord = "";
		while (ctoken.type == ETT_WORD || ctoken.type == ETT_BLANK)
		{
			finalWord += ctoken.word;
			ADV(1);
		}

		if (ctoken.type == ETT_NEWLINE)
			ADV_NON_BLANK(1);

		return finalWord;
	}

	Parser::~Parser()
	{
	}
#pragma endregion
}