#include "./epicr.h"
#include <string>

#pragma region Helper macros
#define ERR(msg, token)                                         \
	{                                                           \
		error = true;                                           \
		error_message = msg;                                    \
		error_token = token;                                    \
		std::cout << "ERROR ON LINE " << __LINE__ << std::endl; \
		print_token(token);                                     \
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
				print_token(ctoken);
				ADV(1);
			}
		}
		//ParseIngredients(rcp);
		//for testing - until we get out of the infinite loop in reading ingredient:
		while (ctoken.word != "instructions" && ctoken.type != ETT_EOF)
		{
			ADV_NON_BLANK(1)
		}
		if (to_lower(ctoken.word) != "instructions")
		{
			ERR("expected instructions",ctoken);
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
	void Parser::ParseAmount(recipe *rcp)
	{
		ADV_NON_BLANK(2);
		/*Read the first word and store in amount.count as a number is expected as the first word otherwise throw an error */
		if (ctoken.type != ETT_NUMBER || utoken.type == ETT_EOF)
		{
			ERR_VOID("No correct desciption for amount has been found!", ctoken);
		}
		rcp->amount.count += stoi(ctoken.word);
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
			std::cout << "Reading amount" << std::endl;
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
			std::cout << "Reading nutrients" << std::endl;
			ingredient nutrient;
			if (ctoken.type != ETT_WORD)
				ERR_VOID("Not a word found for nutrient!", ctoken);
			nutrient.name = ctoken.word;

			/* Find open bracket for unit */
			ADV_NON_BLANK(1);
			if (ctoken.type != ETT_BRACKET_OPEN)
			{
				ERR_VOID("Nutrient must be followed by an amount!", ctoken)
			}

			/* Find number */
			ADV_NON_BLANK(1);
			if (ctoken.type != ETT_NUMBER)
				ERR_VOID("Amount must start with a number", ctoken)
			double count = std::stod(ctoken.word);
			nutrient.amount = count;

			/* Find unit */
			ADV_NON_BLANK(1);
			if (ctoken.type != ETT_WORD || utoken.type == ETT_COLON)
			{

				if (ctoken.word != "kcal" && ctoken.word != "cal" && ctoken.word != "g")
				{
					ERR_VOID("Invalid unit after nutrient", ctoken);
				}
			}

			/* Find closing bracket */
			/* TODO: Consider refactoring brackets, */
			/* 		 I vores sprog bliver hver bracket,*/
			/* 		 kun brugt til et formaal x) saa*/
			ADV_NON_BLANK(1);
			if (ctoken.type != ETT_BRACKET_CLOSE)
				ERR_VOID("Unclosed amount", ctoken);

			nutrients.push_back(nutrient);

			ADV_NON_BLANK(1);
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
			std::cout << "Reading kitchenware x)" << std::endl;
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
			std::cout << "Reading tags" << std::endl;
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
			std::cout << "Reading time" << std::endl;
			rcp->time += ctoken.word;
			ADV(1);
		}
	}

	void Parser::ParseIngredients(recipe *rcp)
	{
		/*  Ingredients should work :) */
		ADV_NON_BLANK(2);

		if (ctoken.type != ETT_WORD || utoken.type == ETT_EOF)
		{
			ERR_VOID("No ingredients was found! x(", ctoken);
		}

		while (utoken.type != ETT_COLON && utoken.type != ETT_EOF)
		{
			std::cout << "Reading ingreditents x)" << std::endl;
			ingredient ingredient;

			if (ctoken.type == ETT_EOF)
			{
				ERR_VOID("@Ingredients, End of file reached x(( :", ctoken);
			}
			else if (ctoken.type == ETT_COMMA)
			{
				ADV_NON_BLANK(1);
			}

			// gets word
			if (ctoken.type == ETT_WORD)
			{
				ingredient.name = ctoken.word;
				ADV_NON_BLANK(1);
				

				/*if a specialoperator (+?*)
				is involved it handles the
				parsing. Should be its own
				function later to save space */
				if (ctoken.type == ETT_PLUS || ctoken.type == ETT_ASTERIX || ctoken.type == ETT_QUESTION_MARK)
				{
					// +
					if (ctoken.type == ETT_PLUS)
					{
						std::cout << ctoken.word << "\n";
						std::cout << utoken.word << "\n";
						ingredient.name = ctoken.word;
						ADV_NON_BLANK(1);
						if (ctoken.type == ETT_ASTERIX || utoken.type == ETT_ASTERIX)
						{
							ERR_VOID("An ingredient with an uncountable operator(+), cannot be a recipe", ctoken)
							/*Ig above token er ikke helt korrekt, da
							det kan vaere i utoken at fejlen sker but w/e */
						}

						if (ctoken.type == ETT_QUESTION_MARK)
						{
							ingredient.name = ctoken.word;
							ADV_NON_BLANK(1);
						}

						if (ctoken.type != ETT_COMMA)
						{
							ERR_VOID("Invalid input. Uncountable ingredients can not specify amount and unit", ctoken);
						}
						rcp->ingredients.push_back(ingredient);
					}

					// *
					if (ctoken.type == ETT_ASTERIX)
					{
						ingredient.name = ctoken.word;
						ADV_NON_BLANK(1);

						if (ctoken.type == ETT_PLUS || utoken.type == ETT_PLUS)
						{
							ERR_VOID("an ingredient cant be uncountable and a recipe", ctoken);
						}
						if (ctoken.type == ETT_QUESTION_MARK)
						{
							ingredient.name = ctoken.word;
							ADV_NON_BLANK(1);
						}

						if (ctoken.type == ETT_ASTERIX || ctoken.type == ETT_PLUS || ctoken.type == ETT_QUESTION_MARK)
						{
							ERR_VOID("Maximun number of specifiers reached", ctoken);
						}
						if (ctoken.type != ETT_BRACKET_OPEN && utoken.type != ETT_NUMBER)
						{
							ERR_VOID("amount must be encapsulated within brackets [  ]", ctoken)
						}

						ingredient.amount = utoken.uid;
						ADV_NON_BLANK(2);

						if (ctoken.type == ETT_WORD)
						{
							ingredient.unit = utoken.word;
							ADV_NON_BLANK(1);
							while (ctoken.type == ETT_WORD)
							{
								ingredient.unit = ctoken.word;
								ADV_NON_BLANK(1);
							}
						}
						if (ctoken.type != ETT_BRACKET_CLOSE)
						{
							ERR_VOID("closing bracket for ingredient amount not found", ctoken)
						}
						rcp->ingredients.push_back(ingredient);
						ADV_NON_BLANK(1);
					}

					// ?
					if (ctoken.type == ETT_QUESTION_MARK)
					{
						ingredient.name = ctoken.word;
						ADV_NON_BLANK(1);

						if (ctoken.type == ETT_PLUS)
						{
							ingredient.name = ctoken.word;
							if (utoken.type != ETT_COMMA)
							{
								ERR_VOID("Invalid input. Uncountable ingredients can not specify amount and unit", utoken);
							}
							else
							{
								rcp->ingredients.push_back(ingredient);
								ADV_NON_BLANK(1);
							}
						}

						if (ctoken.type == ETT_ASTERIX)
						{

							if (ctoken.type != ETT_BRACKET_OPEN && utoken.type != ETT_NUMBER)
							{
								ERR_VOID("amount must be encapsulated within brackets [ ]", ctoken)
							}

							ingredient.amount = utoken.uid;
							ADV_NON_BLANK(2);

							if (ctoken.type == ETT_WORD)
							{
								ingredient.unit = utoken.word;
								ADV_NON_BLANK(1);
								while (ctoken.type == ETT_WORD)
								{
									ingredient.unit = ctoken.word;
									ADV_NON_BLANK(1);
								}
							}

							if (ctoken.type == ETT_PLUS || ctoken.type == ETT_QUESTION_MARK || ctoken.type == ETT_ASTERIX)
							{
								ERR_VOID("invalid tokenoperator", ctoken)
							}

							if (ctoken.type != ETT_BRACKET_CLOSE)
							{
								ERR_VOID("closing bracket for ingredient amount not found", ctoken)
							}
							rcp->ingredients.push_back(ingredient);
							ADV_NON_BLANK(1);
						}
					}
				}

				// finds bracket & amount
				if (ctoken.type == ETT_BRACKET_OPEN && utoken.type == ETT_NUMBER)
				{
					ADV_NON_BLANK(1);
					ingredient.amount = ctoken.uid;
					ADV_NON_BLANK(1);

					if (ctoken.type == ETT_WORD)
					{
						ingredient.unit = utoken.word;
						ADV_NON_BLANK(1);
						while (ctoken.type == ETT_WORD)
						{
							ingredient.unit = ctoken.word;
							ADV_NON_BLANK(1);
						}
					}

					if (ctoken.type != ETT_BRACKET_CLOSE)
					{
						ERR_VOID("closing bracket for ingredient amount not found", ctoken)
					}
					rcp->ingredients.push_back(ingredient);
					ADV_NON_BLANK(1);
				}
			}
		}
	}

	void Parser::ParseInstructions(recipe *rcp)
	{
		ADV_NON_BLANK(2);
		std::vector<instruction> instructions;
		while (utoken.type != ETT_EOF)
		{
			std::cout << "Reading instruction" << std::endl;
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
			ADV_NON_BLANK(1)
			/*
			body
			*/
			if (to_lower(ctoken.word) == "yield" && utoken.type == ETT_COLON)
			{
				ADV_NON_BLANK(2);
				ParseInstructionYield(&singleInstruction);
			}
			
			instructions.push_back(singleInstruction); //something doesnt work here
		}
		rcp->instructions = instructions;
	}
	void Parser::ParseInstructionHeaderWith(instruction *singleInstruction)
	{
		if (ctoken.type != ETT_PARENS_OPEN)
		{
			ERR_VOID("expected open bracket with 'with' ", ctoken);
		}
		ADV_NON_BLANK(1)
		while (ctoken.type != ETT_PARENS_CLOSE)
		{
			if (ctoken.type != ETT_WORD)
			{
				ERR_VOID("expected ingredient name", ctoken);
			}
			ingredient currentIngredient;
			currentIngredient.name = ReadWords();
			if (ctoken.type == ETT_BRACKET_OPEN) // if amount is specified - ie if it is not uncountable
			{
				ADV_NON_BLANK(1);
				if (ctoken.type == ETT_WORD)
				{
					ReadRelativeAmount(&currentIngredient);
				}
				else if (ctoken.type == ETT_NUMBER)
				{
					ReadAmount(&currentIngredient);
				}
				else
				{
					ERR_VOID("expected an amount - either relative with keywords or with an amount and unit", ctoken);
				}
				ADV_NON_BLANK(1);
			}
			if (ctoken.type != ETT_COMMA && ctoken.type != ETT_PARENS_CLOSE)
			{
				ERR_VOID("Expected a ',' as seperator between ingredient or a closing bracket for the 'with'", ctoken);
			}
			if (ctoken.type == ETT_COMMA)
			{
				ADV_NON_BLANK(1);
			}
			singleInstruction->ingredients.push_back(currentIngredient);
		}
		ADV_NON_BLANK(1);
	}
	void Parser::ParseInstructionHeaderUsing(instruction *singleInstruction)
	{
		if (ctoken.type != ETT_PARENS_OPEN)
		{
			ERR_VOID("expected open bracket with 'using' ", ctoken);
		}
		ADV_NON_BLANK(1);
		while (ctoken.type != ETT_PARENS_CLOSE)
		{
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
			if (ctoken.type == ETT_COMMA)
			{
				ADV_NON_BLANK(1);
			}
			singleInstruction->kitchenware.push_back(currentKitchenware);
		}
		ADV_NON_BLANK(1);
	}
	void Parser::ParseInstructionYield(instruction *singleInstruction)
	{
		while (to_lower(ctoken.word) != "with" && to_lower(ctoken.word) != "using" && ctoken.type != ETT_EOF)
		{
			ingredient currentYield;
			if (ctoken.type != ETT_WORD)
			{
				ERR_VOID("expected an ingredient", ctoken);
			}
			currentYield.name = ReadWords();
			if (ctoken.type == ETT_BRACKET_OPEN)
			{
				ADV_NON_BLANK(1)
				if (ctoken.type == ETT_NUMBER)
				{
					ReadAmount(&currentYield);
				}
				else
				{
					ERR_VOID("expected an amount with unit", ctoken);
				}
				ADV_NON_BLANK(1);
			}
			if (ctoken.type != ETT_COMMA && ctoken.type != ETT_EOF && ctoken.type != ETT_BLANK && ctoken.type != ETT_NEWLINE)
			{
				ERR_VOID("Expected a ',' as seperator between ingredients in the yield", ctoken);
			}
			singleInstruction->yields.push_back(currentYield);
			ADV_NON_BLANK(1)
		}
	}
	std::string Parser::ReadWords()
	{
		std::string finalWord = "";
		while (ctoken.type == ETT_WORD || ctoken.type == ETT_BLANK)
			{
				finalWord += ctoken.word;
				ADV(1);
			}	
		return finalWord;
	}
	void Parser::ReadRelativeAmount(ingredient *currentIngredient)
	{
		if (to_lower(ctoken.word) == "half" || to_lower(ctoken.word) == "quarter" || to_lower(ctoken.word) == "rest" || to_lower(ctoken.word) == "all") // relative amount
		// lidt hardcoded - burde måske være constants eller tokens
		{
			currentIngredient->relativeAmount = to_lower(ctoken.word);
			ADV_NON_BLANK(1);
		}
		else
		{
			ERR_VOID("expected a relative amount",ctoken);
		}
		if (ctoken.type != ETT_BRACKET_CLOSE)
		{
			ERR_VOID("expected closing bracket for relative amount",ctoken);
		}
	}
	void Parser::ReadAmount(ingredient *currentIngredient)
	{
		while (ctoken.type != ETT_BRACKET_CLOSE)
		{
			if (ctoken.type == ETT_NUMBER) // amount as number
			{
				currentIngredient->amount = std::stod(to_lower(ctoken.word));
				ADV_NON_BLANK(1);
				if (ctoken.type == ETT_BRACKET_CLOSE) // if it doesn't have a unit
				{
					break;
				}
				if (ctoken.type != ETT_WORD)
				{
					ERR_VOID("Expected a corresponding unit with the ingredient.", ctoken);
				}
				currentIngredient->unit = ReadWords();
			}
		}
	}

	void Parser::ParseInstructionBody(recipe *rcp,instruction *currentInstruction) {
		// assumes body starts from ':' (ctoken)
		ingredient ingredientInBody;
		instruction_word Body;
		ADV_NON_BLANK(1);
		bool insturctionFinished = false;

		while(insturctionFinished == false) {
			std::cout << "Rewdung prowcewdure BODY" << std::endl;

			if(ctoken.type == ETT_EOF || utoken.type == ETT_EOF) {
				ERR("no data found after bodydeclartion ", ctoken);
			}

			//Body.word += ctoken.word;
			if(ctoken.type == ETT_WORD || ctoken.type == ETT_COMMA || ctoken.type == ETT_NUMBER) {
				Body.word += ctoken.word;
				currentInstruction->body.push_back(Body);
				ADV(1);
			}
			/*
			if(ctoken.word == ingredientInBody.name) {
				//ingredientInBody
			}*/
			if(ctoken.type == ETT_BRACKET_OPEN ) {
				Body.word += ctoken.word;
				ADV(1);

				
				if(ctoken.type != ETT_WORD) {
				 ERR("not an ingredient", ctoken);
				}
				if(ctoken.type == ETT_WORD) {
				ingredientInBody.name += ctoken.word;
				Body.word += ctoken.word;
				ADV(1);
				while(ctoken.type != ETT_CURLY_CLOSE || ctoken.type == ETT_EOF) {


				if(ctoken.type == ETT_NUMBER){
					ingredientInBody.amount += ctoken.uid;
					Body.word += ctoken.word;
				}
				if(ctoken.type == ETT_WORD || ctoken.type == ETT_BLANK){
					ingredientInBody.name += ctoken.word;
					Body.word += ctoken.word;
				}
				ADV(1);
				}

				if(ctoken.type != ETT_CURLY_CLOSE) {
					ERR("uh oh no c bracket closing", ctoken);
				}
				Body.word += ctoken.word;
				rcp->ingredients.push_back(ingredientInBody);
				ADV(1);
				}
			}

			//has yield or update
			if(ctoken.word == "update" || ctoken.word == "yield" && utoken.type == ETT_COLON) {
				insturctionFinished = true;
				//yield call
			}
			ADV_NON_BLANK(1);
			//has not yield or update
			if(ctoken.word == "with" || ctoken.word == "using" && utoken.type == ETT_PARENS_OPEN) {
				insturctionFinished = true;
				//yield call
			}
			if(ctoken.word == "with" && ctoken.word == "using" && utoken.type == ETT_PARENS_OPEN) {
				
				insturctionFinished = true;
				//yield call
			}
			currentInstruction->body.push_back(Body);
			ADV_NON_BLANK(1);
		}
	}
	Parser::~Parser()
	{
	}
#pragma endregion
}