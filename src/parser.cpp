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
		return;                                                 \
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
		recipe *rcp = (recipe *)calloc(1, sizeof(recipe));
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
		ParseIngredients(rcp);
		while (ctoken.word != "instructions" && ctoken.type != ETT_EOF) //for testing
		{
			ADV_NON_BLANK(1);
		}
		ParseInstructions(rcp);
		

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
	void Parser::ParseAmount(recipe *rcp)
	{
		ADV_NON_BLANK(2);
		/*Read the first word and store in amount.count as a number is expected as the first word otherwise throw an error */
		if (ctoken.type != ETT_NUMBER || utoken.type == ETT_EOF)
		{
			ERR("No correct desciption for amount has been found!", ctoken);
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
				ERR("Not a word found for nutrient!", ctoken);
			nutrient.name = ctoken.word;

			/* Find open bracket for unit */
			ADV_NON_BLANK(1);
			if (ctoken.type != ETT_CBRACKET_OPEN || ctoken.word != "{")
			{
				ERR("Nutrient must be followed by an amount!", ctoken)
			}

			/* Find number */
			ADV_NON_BLANK(1);
			if (ctoken.type != ETT_NUMBER)
				ERR("Amount must start with a number", ctoken)
			double count = std::stod(ctoken.word);
			nutrient.amount = count;

			/* Find unit */
			ADV_NON_BLANK(1);
			if (ctoken.type != ETT_WORD || utoken.type == ETT_COLON)
			{

				if (ctoken.word != "kcal" && ctoken.word != "cal" && ctoken.word != "g")
				{
					ERR("Invalid unit after nutrient", ctoken);
				}
			}

			/* Find closing bracket */
			/* TODO: Consider refactoring brackets, */
			/* 		 I vores sprog bliver hver bracket,*/
			/* 		 kun brugt til et formaal x) saa*/
			ADV_NON_BLANK(1);
			if (ctoken.type != ETT_CBRACKET_CLOSE && ctoken.word != "}")
				ERR("Unclosed amount", ctoken);

				

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
				ERR("@kitchenware, End of file reached:", ctoken);
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
			ERR("No tags where found!", ctoken);
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
			ERR("No amount was found!", ctoken);
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
			ERR("No ingredients was found! x(", ctoken);
		}

		while (utoken.type != ETT_COLON && utoken.type != ETT_EOF)
		{
			std::cout << "Reading ingreditents x)" << std::endl;
			ingredient ingredient;

			if (ctoken.type == ETT_EOF)  
			{
				ERR("@Ingredients, End of file reached x(( :", ctoken);
			}
			else if (ctoken.type == ETT_COMMA)
			{
				ADV_NON_BLANK(1);
			}

			//gets word
			if (ctoken.type == ETT_WORD) {
				ingredient.name = ctoken.word;
				ADV_NON_BLANK(1);
				

					/*if a specialoperator (+?*) 
					is involved it handles the 
					parsing. Should be its own 
					function later to save space */
				if(ctoken.type == ETT_SPECIAL_OPR_P || ctoken.type == ETT_SPECIAL_OPR_A || ctoken.type == ETT_SPECIAL_OPR_Q) {

					// +
					if(ctoken.type == ETT_SPECIAL_OPR_P) {
						ingredient.name = ctoken.word;
						ADV_NON_BLANK(1);
						if(ctoken.type == ETT_SPECIAL_OPR_A || utoken.type == ETT_SPECIAL_OPR_A) {
							ERR("An ingredient with an uncountable operator(+), cannot be a recipe", ctoken)
							/*Ig above token er ikke helt korrekt, da 
							det kan vaere i utoken at fejlen sker but w/e */
						}	
					
						if(ctoken.type == ETT_SPECIAL_OPR_Q){
							ingredient.name = ctoken.word;
							ADV_NON_BLANK(1);
						}

						if(ctoken.type != ETT_COMMA) {
							ERR("Invalid input. Uncountable ingredients can not specify amount and unit", ctoken);
						}	
					rcp->ingredients.push_back(ingredient);
					}

					// *
					if(ctoken.type == ETT_SPECIAL_OPR_A) {
					ingredient.name = ctoken.word;
					ADV_NON_BLANK(1);

						if(ctoken.type == ETT_SPECIAL_OPR_P || utoken.type == ETT_SPECIAL_OPR_P) {
							ERR("an ingredient cant be uncountable and a recipe", ctoken);
						}
						if(ctoken.type == ETT_SPECIAL_OPR_Q) {
							ingredient.name = ctoken.word;
							ADV_NON_BLANK(1);
						}

						if(ctoken.type == ETT_SPECIAL_OPR_A || ctoken.type == ETT_SPECIAL_OPR_P || ETT_SPECIAL_OPR_Q) {
							ERR("Maximun number of specifiers reached", ctoken);
						}
						if(ctoken.type != ETT_CBRACKET_OPEN && utoken.type != ETT_NUMBER) {
							ERR("amount must be encapsulated within curly brackets {  }",ctoken)
						}

						ingredient.amount = utoken.uid;
						ADV_NON_BLANK(2);
						
						if(ctoken.type == ETT_WORD) {
							ingredient.unit = utoken.word;
							ADV_NON_BLANK(1);
							while (ctoken.type == ETT_WORD) {
								ingredient.unit = ctoken.word;
								ADV_NON_BLANK(1);
							}
						}
						if(ctoken.type != ETT_CBRACKET_CLOSE) {
							ERR("closing bracket for ingredient not found", ctoken)
						}
						rcp->ingredients.push_back(ingredient);
						ADV_NON_BLANK(1);
					}

					// ?
					if(ctoken.type == ETT_SPECIAL_OPR_Q) { 
						ingredient.name = ctoken.word;
						ADV_NON_BLANK(1);

						if(ctoken.type == ETT_SPECIAL_OPR_P) {
							ingredient.name = ctoken.word;
							if(utoken.type != ETT_COMMA){
								ERR("Invalid input. Uncountable ingredients can not specify amount and unit", utoken);
							}
							else {
								rcp->ingredients.push_back(ingredient);
								ADV_NON_BLANK(1);
							}
						}

						if(ctoken.type == ETT_SPECIAL_OPR_A) {
							
							if(ctoken.type != ETT_CBRACKET_OPEN && utoken.type != ETT_NUMBER) {
								ERR("amount must be encapsulated within curly brackets {  }",ctoken)
							}

							ingredient.amount = utoken.uid;
							ADV_NON_BLANK(2);

							if(ctoken.type == ETT_WORD) {
								ingredient.unit = utoken.word;
								ADV_NON_BLANK(1);
							while (ctoken.type == ETT_WORD) {
								ingredient.unit = ctoken.word;
								ADV_NON_BLANK(1);
							}
						}

						if(ctoken.type == ETT_SPECIAL_OPR_P || ctoken.type == ETT_SPECIAL_OPR_Q || ctoken.type == ETT_SPECIAL_OPR_A) {
							ERR("invalid tokenoperator", ctoken)
						}

						if(ctoken.type != ETT_CBRACKET_CLOSE) {
							ERR("closing bracket for ingredient not found", ctoken)
						}
						rcp->ingredients.push_back(ingredient);
						ADV_NON_BLANK(1);
						}
					}
				}

				//finds bracket & amount
				if(ctoken.type == ETT_CBRACKET_OPEN && utoken.type == ETT_NUMBER) { 
					ADV_NON_BLANK(1);
					ingredient.amount =ctoken.uid;
					ADV_NON_BLANK(1);

						if(ctoken.type == ETT_WORD) {
							ingredient.unit = utoken.word;
							ADV_NON_BLANK(1);
							while (ctoken.type == ETT_WORD) {
								ingredient.unit = ctoken.word;
								ADV_NON_BLANK(1);
							}
						}

						if(ctoken.type != ETT_CBRACKET_CLOSE) {
							ERR("closing bracket for ingredient not found", ctoken)
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
		int i = 0;
		while (utoken.type != ETT_EOF)
		{
			if (ctoken.word != "with" && ctoken.word != "using")
			{
				ERR("expected instruction header",ctoken);
			}
			ADV_NON_BLANK(1)
			instruction currentInstruction = rcp->instructions[i];
			if (ctoken.word == "with") //casing stuff here
			{
				ParseInstructionHeaderWith(rcp,currentInstruction);
			}
			if (ctoken.word == "using")
			{
				ParseInstructionHeaderUsing(rcp,currentInstruction);
			}
			/*
			body
			yield
			*/
			i++;
		}	
	}	
	void Parser::ParseInstructionHeaderWith(recipe *rcp, instruction currentInstruction)
	{
		if (ctoken.type != ETT_BRACKET_OPEN)
		{
			ERR("expected open bracket",ctoken)
		}	
		ADV_NON_BLANK(1)
		int i = 0;
		while (utoken.type != ETT_COLON)
		{
			if (ctoken.type == ETT_WORD && ctoken.type == ETT_BLANK)
			{
				currentInstruction.ingredients[i].name += ctoken.word;
				ADV(1)
			}
			if (ctoken.type == ETT_BRACKET_OPEN)
			{
				ADV_NON_BLANK(1)
				if (ctoken.type != ETT_NUMBER) //amount
				{
					ERR("expected amount",ctoken)
				}
				currentInstruction.ingredients[i].amount = std::stod(ctoken.word);
				ADV_NON_BLANK(1)
				while (ctoken.type != ETT_BRACKET_CLOSE)
				{
					while (ctoken.type == ETT_WORD || ctoken.type == ETT_BLANK) //unit
					{
						currentInstruction.ingredients[i].unit += ctoken.word;
						ADV(1) 
					}
				}
				ADV_NON_BLANK(1);
				if (ctoken.type != ETT_COMMA)
				{
					ERR("expected seperator between ingredients",ctoken)
				}
				ADV_NON_BLANK(1);
			}
			i++;
		}
		if (ctoken.type != ETT_BRACKET_CLOSE)
		{
			ERR("with statement has no closing bracket",ctoken)
		}
	}
	void Parser::ParseInstructionHeaderUsing(recipe *rcp, instruction currentInstruction)
	{
		
	}
	Parser::~Parser()
	{
	}
#pragma endregion
}