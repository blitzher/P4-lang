#include "./epicr.h"

#pragma region Helper macros

#define ERR(msg, token)                                    \
	{                                                      \
		has_error = true;                                  \
		error = msg;                                       \
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
		longjmp(exit_jmp, 1);                              \
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
		ctoken = lexer->next_token(count).back();                           \
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

std::jmp_buf exit_jmp;

namespace epicr
{
#pragma region Parser implementation

	Parser::Parser(Lexer* lexer_r)
	{
		lexer = lexer_r;
		silent = false;
	}
	recipe Parser::Parse()
	{
		has_error = 0;
		error = "No error";
		recipe rcp = recipe();
		ctoken = lexer->next_non_blank_token();
		utoken = lexer->peek_non_blank_token();

		/* If an error occured during parsing,
		 * return what was parsed so far */
		setjmp(exit_jmp);
		/* Parse all fields */
		while (ctoken.type != E_TT_EOF && !has_error)
			/* TODO: refactor x */
		{
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
			}
			else
			{
				ADV(1);
			}
		}
		return rcp;
	}

	void Parser::ParseTitle(recipe* rcp)
	{
		ADV_NON_BLANK(2);

		/* Read all words and spaces in title */
		while (utoken.type != E_TT_COLON && ctoken.type != E_TT_EOF)
		{
			rcp->title = ReadWords(E_RW_NUMBERS,false);
			if (utoken.type != E_TT_COLON)
				ADV_NON_BLANK(1);
		}
		rcp->title = strip_spaces_right(rcp->title);
	}
	void Parser::ParseDescription(recipe* rcp)
	{
		ADV_NON_BLANK(2);
		/* Read all words and spaces in description */
		while (utoken.type != E_TT_COLON && ctoken.type != E_TT_EOF)
		{
			rcp->description += ctoken.word;
			ADV(1);
		}
		rcp->description = strip_spaces_right(rcp->description);
	}
	void Parser::ParseServings(recipe* rcp)
	{
		ADV_NON_BLANK(2);
		if (ctoken.type != E_TT_NUMBER)
		{
			ERR_VOID("No correct description for amount has been found", ctoken);
		}
		rcp->servings.count = stoi(ctoken.word);
		ADV_NON_BLANK(1);
		rcp->servings.descriptor = ReadWords(E_RW_NONE,true);
	}

	void Parser::ParseNutrients(recipe* rcp)
	{
		ADV_NON_BLANK(2);

		std::vector<ingredient> nutrients;

		while (utoken.type != E_TT_COLON)
		{
			ingredient nutrient = ReadIngredient(0);
			if (has_error)
				return;
			if (nutrient.amount.unit != "kcal" && nutrient.amount.unit != "cal" && nutrient.amount.unit != "g")
				ERR_VOID("Invalid unit after nutrient", ctoken);

			nutrients.push_back(nutrient);
			ReadSeperatorOrWaitAtNextField("nutrients");
		}

		rcp->nutrients = nutrients;
	}

	void Parser::ParseKitchenware(recipe* rcp)
	{
		ADV_NON_BLANK(2);
		while (utoken.type != E_TT_COLON && ctoken.type != E_TT_EOF)
		{
			std::string kitchenware = ReadWords(E_RW_NUMBERS,false);
			rcp->kitchenware.push_back(kitchenware);

			ReadSeperatorOrWaitAtNextField("kitchenware");
		}
	}

	void Parser::ParseTags(recipe* rcp)
	{
		ADV_NON_BLANK(2);

		while (utoken.type != E_TT_COLON && ctoken.type != E_TT_EOF)
		{
			std::string tag = ReadWords(E_RW_NUMBERS | E_RW_PARENTHESIS,false);
			rcp->tags.push_back(tag);
			ReadSeperatorOrWaitAtNextField("tags");
		}
	}

	void Parser::ParseTime(recipe* rcp)
	{
		/*saves which kind of time it is: */
		std::string time_type = to_lower(ctoken.word);
		ADV_NON_BLANK(2);
		std::string time = "";
		while (utoken.type != E_TT_COLON && ctoken.type != E_TT_EOF)
		{
			time += ctoken.word;
			ADV(1);
		}
		time = strip_spaces_right(time);
		if (time_type == "prep-time")
			rcp->time.prep_time = time;
		else if (time_type == "cook-time")
			rcp->time.cook_time = time;
		else /*total-time*/
			rcp->time.total_time = time;
	}

	void Parser::ParseIngredients(recipe* rcp)
	{
		ADV_NON_BLANK(2);
		while (utoken.type != E_TT_COLON && ctoken.type != E_TT_EOF)
		{
			// ADV_NON_BLANK(1);
			ingredient ingr = ReadIngredient(E_RI_HAS_PLUS | E_RI_HAS_ASTERIX | E_RI_HAS_QMARK | E_RI_ASSUME_1_NUM);
			if (has_error)
				return;

			if (ingr.is_ingredient_ref)
			{
				std::filesystem::path fpath = std::filesystem::absolute(clargs.input_filepath);
				std::filesystem::path ppath = fpath.parent_path();

				auto rcp_ret = parse_recipe((ppath / ingr.name).string() + ".rcp");
				if (rcp_ret.has_err)
				{
					char* err = (char*)malloc(100);
					sprintf(err, "In %s: %s", ingr.name.c_str(), rcp_ret.err.c_str());
					error = err;
					longjmp(exit_jmp, 1);
				}
				recipe rcp = rcp_ret.recipe;
				ingr.name = rcp.title;
				ingr.amount.unit = ingr.amount.unit == ""
					? (rcp.servings.descriptor == ""
						? "servings"
						: rcp.servings.descriptor)
					: ingr.amount.unit;

				ingr.amount.number = ingr.amount.number == 0
					? (rcp.servings.count == 0
						? 1
						: rcp.servings.count)
					: ingr.amount.number;

				generate_html(rcp, ((std::filesystem::path)clargs.output_filepath / rcp.title).string() + ".html");
			}

			rcp->ingredients.push_back(ingr);
			ReadSeperatorOrWaitAtNextField("ingredients");
		}
	}

	void Parser::ParseInstructions(recipe* rcp)
	{

		ADV_NON_BLANK(2);

		while (utoken.type != E_TT_COLON && utoken.type != E_TT_EOF)
		{
			instruction single_instruction = instruction();
			if (to_lower(ctoken.word) == "with")
			{
				ADV_NON_BLANK(1);
				ParseInstructionHeaderWith(&single_instruction);
				if (has_error)
					return;
			}
			if (to_lower(ctoken.word) == "using")
			{
				ADV_NON_BLANK(1);
				ParseInstructionHeaderUsing(&single_instruction);
				if (has_error)
					return;
			}
			if (ctoken.type != E_TT_COLON)
			{
				ERR_VOID("missing ':' after instruction header", ctoken);
			}
			ADV_NON_BLANK(1);
			ParseInstructionBody(&single_instruction);
			if (to_lower(ctoken.word) == "yield" && utoken.type == E_TT_COLON)
			{
				ADV_NON_BLANK(2);
				ParseInstructionYield(&single_instruction);
			}

			rcp->instructions.push_back(single_instruction); // something doesnt work here
		}
	}
	void Parser::ParseInstructionHeaderWith(instruction* single_instruction)
	{
		if (ctoken.type != E_TT_PARENS_OPEN)
		{
			ERR_VOID("expected open bracket with 'with' ", ctoken);
		}
		while (ctoken.type != E_TT_PARENS_CLOSE) /*reads every ingredient in the "with"*/
		{
			ADV_NON_BLANK(1);
			if (ctoken.type == E_TT_PARENS_CLOSE) /*if the with is empty*/
			{
				ADV_NON_BLANK(1);
				return;
			}
			ingredient current_ingredient = ReadIngredient(E_RI_ASSUME_REST);
			if (has_error)
				return;
			if (ctoken.type != E_TT_COMMA && ctoken.type != E_TT_PARENS_CLOSE)
			{
				ERR_VOID("Expected a ',' as seperator between ingredient or a closing parenthesis for the 'with'", ctoken);
			}
			single_instruction->ingredients.push_back(current_ingredient);
		}
		ADV_NON_BLANK(1) /*reads through the end parenthesis*/
	}
	void Parser::ParseInstructionHeaderUsing(instruction* single_instruction)
	{
		if (ctoken.type != E_TT_PARENS_OPEN)
			ERR_VOID("expected open bracket with 'using' ", ctoken);

		while (ctoken.type != E_TT_PARENS_CLOSE) /*reads every kitchenware in the "using"*/
		{
			ADV_NON_BLANK(1);
			if (ctoken.type == E_TT_PARENS_CLOSE) /*if the using is empty*/
			{
				ADV_NON_BLANK(1);
				return;
			}
			std::string current_kitchenware = ReadWords(E_RW_NUMBERS,false);
			if (ctoken.type != E_TT_COMMA && ctoken.type != E_TT_PARENS_CLOSE)
			{
				ERR_VOID("Expected a ',' as seperator between kitchenware or a closing parenthesis for the 'using'", ctoken);
			}
			single_instruction->kitchenware.push_back(current_kitchenware);
		}
		ADV_NON_BLANK(1); /*reads through the end parenthesis*/
	}
	void Parser::ParseInstructionBody(instruction* current_instruction)
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
		if (Body.size() == 0)
		  ERR_VOID("Instruction body cannot be empty", ctoken);
		current_instruction->body = Body;
	}

	void Parser::ParseInstructionYield(instruction* single_instruction)
	{
		do
		{
			if (ctoken.type == E_TT_COMMA)
				ADV_NON_BLANK(1);
			if (ctoken.type == E_TT_EOF)
				break;
			ingredient current_yield = ReadIngredient(E_RI_HAS_PLUS | E_RI_ASSUME_1_NUM);
			if (has_error)
				return;
			single_instruction->yields.push_back(current_yield);
		} while (ctoken.type == E_TT_COMMA);
	}
	ingredient Parser::ReadIngredient(ingredient_arg arg)
	{
		bool can_have_asterix = (arg & E_RI_HAS_ASTERIX) >> 1;
		bool can_have_qmark = (arg & E_RI_HAS_QMARK) >> 2;

		ingredient current_ingredient = ingredient();
		amount ingredient_amount = amount();

		if (ctoken.type != E_TT_WORD && ctoken.type != E_TT_NUMBER)
		{
			ERR("Expected ingredient name", ctoken);
			return current_ingredient;
		}
		current_ingredient.name = ReadWords(E_RW_NUMBERS,false);

		while (ctoken.type == E_TT_ASTERIX || ctoken.type == E_TT_QUESTION_MARK)
		{
			if (ctoken.type == E_TT_ASTERIX)
			{
				if (!can_have_asterix)
				{
					ERR("An asterix is not valid in the given context", ctoken);
					return current_ingredient;
				}
				if (current_ingredient.is_ingredient_ref)
				{
					ERR("Duplicate asterix", ctoken);
				} // should be a warning
				current_ingredient.is_ingredient_ref = true;
			}

			if (ctoken.type == E_TT_QUESTION_MARK)
			{
				if (!can_have_qmark)
				{
					ERR("A question mark is not valid in the given context", ctoken);
					return current_ingredient;
				}
				else if (current_ingredient.is_optional)
				{
					ERR("Duplicate question mark", ctoken); // should be a warning
				}
				current_ingredient.is_optional = true;
			}
			ADV_NON_BLANK(1);
		}

		ingredient_amount = ReadAmount(arg);
		current_ingredient.amount = ingredient_amount;
		return current_ingredient;
	}

	amount Parser::ReadAmount(ingredient_arg arg)
	{
		bool assume_1_num = (arg & E_RI_ASSUME_1_NUM) >> 3;
		bool assume_rest = (arg & E_RI_ASSUME_REST) >> 4;
		amount amnt = amount();

		if (ctoken.type == E_TT_PLUS)
		{
			amnt.is_uncountable = true;
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
				amnt.is_relative_amount = true;
				amnt.relative_amount = "rest";
			}
			return amnt;
		}

		ADV_NON_BLANK(1);

		if (ctoken.type == E_TT_NUMBER)
		{
			amnt.number = std::stod(ctoken.word);
			ADV_NON_BLANK(1);
			amnt.unit = ReadWords(E_RW_NONE,true);
		}
		else if (ctoken.type == E_TT_WORD)
		{
			amnt.is_relative_amount = true;
			amnt.relative_amount = ReadWords(E_RW_NONE,false);

			std::string valid_relatives[]{ "rest", "quarter", "half", "all" };

			bool is_valid = false;
			size_t i;
			for (i = 0; i < 4; i++)
			{
				if (valid_relatives[i] == amnt.relative_amount)
				{
					is_valid = true;
					break;
				}
			}

			if (!is_valid)
			{
				ERR("Invalid relative amount", ctoken);
				return amnt;
			}
		}
		else
		{
			ERR("Did not find number or word in amount", ctoken);
		}

		if (ctoken.type != E_TT_BRACKET_CLOSE)
		{
			ERR("Expected closing square bracket after amount", ctoken);
		}
		ADV_NON_BLANK(1);

		return amnt;
	}

	std::string Parser::ReadWords(readwords_arg arg,bool wordCanBeEmpty)
	{
		std::string final_word = "";

		while (ReadWordsPredicate(ctoken.type, arg) && utoken.type != E_TT_COLON)
		{
			final_word += ctoken.word;
			ADV(1);
		}

		if (utoken.type == E_TT_COLON)
			return strip_spaces_right(final_word);

		if (ctoken.type == E_TT_NEWLINE)
			ADV_NON_BLANK(1);

		utoken = lexer->peek_token();
		final_word = strip_spaces_right(final_word);
		if(final_word.empty() && !wordCanBeEmpty) 
			ERR("word cannot be empty",ctoken);
		return final_word;
	}

	bool Parser::ReadWordsPredicate(epicr_token_type type, readwords_arg arg)
	{
		bool can_read_numbers = arg & E_RW_NUMBERS;
		bool can_read_parenthesis = arg & E_RW_PARENTHESIS;

		switch (type)
		{
		case E_TT_WORD:
		case E_TT_BLANK:
			return true;
		case E_TT_NUMBER:
			if (can_read_numbers)
				return true;
			return false;
		case E_TT_PARENS_OPEN:
		case E_TT_PARENS_CLOSE:
			if (can_read_parenthesis)
				return true;
			return false;
		default:
			return false;
		}
	}

	void Parser::ReadSeperatorOrWaitAtNextField(std::string field_name)
	{
		if (ctoken.type == E_TT_COMMA)
		{
			ADV_NON_BLANK(1);
			return;
		}
		if (utoken.type != E_TT_COLON && ctoken.type != E_TT_EOF)
		{
			std::string error_string = "expected a comma as a seperator between " + field_name;
			ERR_VOID(error_string, ctoken);
		}
		return;
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
