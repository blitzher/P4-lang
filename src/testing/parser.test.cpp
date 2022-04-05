#include "./test_lib.h"

void parsed_title()
{
	test_lib::REGISTER;

	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;
	test_lib::expect_equal_s(rcp.title, "Pasta\n");
}

void parsed_description()
{
	test_lib::REGISTER;
	std::ifstream f = epicr::open_file("src/test-recipes/Pasta.rcp");
	epicr::Lexer l = epicr::Lexer(f);
	epicr::Parser p = epicr::Parser(&l);
	epicr::recipe rcp = p.Parse();
	test_lib::expect_equal_s(rcp.description, "Frisklavet pasta\n");
}

void parsed_amount()
{
	test_lib::REGISTER;

	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;
	test_lib::expect_equal_i(rcp.amount.count, 4);
	test_lib::expect_equal_s(rcp.amount.descriptor, "people\n");
}

void parsed_cook_time()
{
	test_lib::REGISTER;

	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;
	test_lib::expect_equal_s(rcp.time, "50 min\n");
}

void parsed_tags()
{
	test_lib::REGISTER;

	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;
	test_lib::expect_equal_s(rcp.tags[0], "pasta");
	test_lib::expect_equal_s(rcp.tags[1], "easy to make");
}

void parsed_kitchenware()
{
	test_lib::REGISTER;

	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;
	test_lib::expect_equal_s(rcp.kitchenware[0], "plastic wrap");
	test_lib::expect_equal_s(rcp.kitchenware[1], "rolling pin");
}

void parsed_ingredients()
{
	test_lib::REGISTER;

	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;

	if (rcp.ingredients.size() < 5)
	{
		char *m = (char *)malloc(100);
		sprintf(m, "Not all ingredients were parsed properly, found %zu ingredients", rcp.ingredients.size());
		test_lib::deny(m);
		return;
	}

	test_lib::expect_equal_s(rcp.ingredients[4].name, "extra wheatflour");
	if (!rcp.ingredients[4].isOptional)
	{
		test_lib::deny("Expected ingredient extra wheatflour to be optional");
	}
	if (!rcp.ingredients[4].amount.isUncountable)
	{
		test_lib::deny("Expected ingredient extra wheatflour to be uncountable");
	}
	if (rcp.ingredients[4].amount.amount != std::numeric_limits<double>::infinity())
	{
		test_lib::deny("Expected ingredient extra wheatflour to have amount set to INF");
	}
}

void instrucions_has_correct_amount_of_instructions()
{
	test_lib::REGISTER;
	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;
	test_lib::expect_equal_i(rcp.instructions.size(), 3);
}

void first_instruction_has_correct_amount_of_ingredients()
{
	test_lib::REGISTER;
	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;
	test_lib::expect_equal_i(rcp.instructions[0].ingredients.size(), 4);
}


void first_instruction_has_correct_ingredients_name()
{
	std::vector<std::string> ingredients = {"wheatflour", "egg", "salt", "water"};
	test_lib::REGISTER;
	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;
	size_t ingredientSize = ingredients.size();
	for (size_t i = 0; i < ingredientSize; i++)
	{
		test_lib::expect_equal_s(rcp.instructions[0].ingredients[i].name, ingredients[i]);	
	}
}

void first_ingredient_in_first_instruction_has_correct_amount()
{
	test_lib::REGISTER;
	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;
	test_lib::expect_equal_i(rcp.instructions[0].ingredients[0].amount.isRelativeAmount, 0); //this one fails
	test_lib::expect_equal_i(rcp.instructions[0].ingredients[0].amount.amount,300);
	test_lib::expect_equal_s(rcp.instructions[0].ingredients[0].amount.unit,"g");
}

void second_ingredient_in_first_instruction_has_correct_amount()
{
	test_lib::REGISTER;
	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;
	test_lib::expect_equal_i(rcp.instructions[0].ingredients[1].amount.isRelativeAmount, 1);
	test_lib::expect_equal_s(rcp.instructions[0].ingredients[1].amount.relativeAmount, "all");
}

void ingredient_in_instruction_without_amount_has_correct_amount()
{
	test_lib::REGISTER;
	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;
	test_lib::expect_equal_i(rcp.instructions[2].ingredients[0].amount.isRelativeAmount, 0);
	test_lib::expect_equal_i(rcp.instructions[2].ingredients[0].amount.isUncountable,0); //this one fails now
	test_lib::expect_equal_s(rcp.instructions[2].ingredients[0].amount.unit,"");
	test_lib::expect_equal_s(rcp.instructions[2].ingredients[0].amount.relativeAmount,"");
	test_lib::expect_equal_i(rcp.instructions[2].ingredients[0].amount.amount,1); 
}


void second_instruction_has_no_ingredients()
{
	test_lib::REGISTER;
	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;
	test_lib::expect_equal_i(rcp.instructions[1].ingredients.size(), 0);
}

void third_instruction_has_correct_ingredients_name()
{
	test_lib::REGISTER;
	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;
	test_lib::expect_equal_s(rcp.instructions[2].ingredients[0].name, "dough");
	test_lib::expect_equal_s(rcp.instructions[2].ingredients[1].name, "extra wheatflour");
}

void first_instruction_has_no_kitchenware()
{
	test_lib::REGISTER;
	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;
	test_lib::expect_equal_i(rcp.instructions[0].kitchenware.size(), 0);
}

void second_instruction_has_correct_amount_of_kitchenware()
{
	test_lib::REGISTER;
	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;
	test_lib::expect_equal_i(rcp.instructions[1].kitchenware.size(), 1);
}

void second_instruction_has_correct_kitchenware_name()
{
	test_lib::REGISTER;
	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;
	test_lib::expect_equal_s(rcp.instructions[1].kitchenware[0], "plastic wrap");
}

void third_instruction_has_correct_kitchenware_name()
{
	test_lib::REGISTER;
	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;
	test_lib::expect_equal_s(rcp.instructions[2].kitchenware[0], "kitchenroller");
}

void parsed_instruction_body()
{
	test_lib::REGISTER;
	auto rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;
	/* TODO: Take stance on how to deal with instructions words that are amounts */
	auto body = rcp.instructions[0].body;
	test_lib::expect_equal_s(body[0].spelling, "Put");
	test_lib::expect_equal_s(body[1].spelling, " ");
	test_lib::expect_equal_s(body[2].spelling, "the");
	test_lib::expect_equal_s(body[3].spelling, " ");
	test_lib::expect_equal_i(body[4].is_amount, true);
	test_lib::expect_equal_i(body[4].value.amount, 300);
	test_lib::expect_equal_s(body[4].value.unit, "g");
}
void first_instruction_body_text_is_parsed_correctly()
{
	test_lib::REGISTER;
	auto rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;
	size_t actualInstructionBodySize = rcp.instructions[0].body.size();
	std::string expectedInstructionBody = std::string("Put the 300.000000 g wheatflour on the table and make a cavity in the middle.\n")+
										  std::string("Crack the 3.000000 eggs in the cavity, and add salt.\n")+
										  std::string("Start mixing the eggs into the wheatflour.\n")+
								 		  std::string("If the dough becomes to dry, add water, and if it becomes to sticky add wheatflour.\n")+
								 		  std::string("Knead the dough thoroughly.\n");
	std::string actualInstructionBody = "";
	for (size_t i = 0; i < actualInstructionBodySize; i++)
		{	
			actualInstructionBody += rcp.instructions[0].body[i].spelling;
			if (rcp.instructions[0].body[i].is_amount)
			{
				actualInstructionBody += std::to_string(rcp.instructions[0].body[i].value.amount) + " ";
				if (rcp.instructions[0].body[i].value.unit != "")
					actualInstructionBody += rcp.instructions[0].body[i].value.unit + " ";
			}
		}
	test_lib::expect_equal_s(expectedInstructionBody,actualInstructionBody);
	/* TODO: Take stance on how to deal with instructions words that are amounts 
	works now, but the 0's after the decimal-seperator should be omitted*/
}

void instruction_body_amounts_have_is_amount_set_to_true()
{
	test_lib::REGISTER;
	auto rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;
	test_lib::expect_equal_i(1,rcp.instructions[0].body[4].is_amount); //should be true - ie 1
}

void instruction_body_non_amounts_have_is_amount_set_to_false()
{
	test_lib::REGISTER;
	auto rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;
	test_lib::expect_equal_i(0,rcp.instructions[0].body[0].is_amount); //should be false - ie 0	
	test_lib::expect_equal_i(0,rcp.instructions[0].body[11].is_amount); //should be false - ie 0
	test_lib::expect_equal_i(0,rcp.instructions[1].body[0].is_amount); //should be false - ie 0	
}

void first_instruction_yields_zero_ingredients()
{
	test_lib::REGISTER;
	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;
	test_lib::expect_equal_i(rcp.instructions[0].yields.size(),0);
}

void second_instruction_yields_an_ingredient()
{
	test_lib::REGISTER;
	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;
	test_lib::expect_equal_s(rcp.instructions[1].yields[0].name, "dough");
}


void parse_fields_in_random_order()
{
	test_lib::REGISTER;

	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/PastaRandomOrder.rcp").recipe;

	test_lib::expect_equal_s(rcp.title, "Pasta\n");
	test_lib::expect_equal_s(rcp.instructions[0].ingredients[0].name, "wheatflour");
	test_lib::expect_equal_s(rcp.description, "Frisklavet pasta\n");
	test_lib::expect_equal_i(rcp.ingredients[0].amount.amount, 300);
}

int main(void)
{
	parsed_title();
	parsed_description();
	parsed_amount();
	parsed_cook_time();
	parsed_tags();
	parsed_kitchenware();
	parsed_ingredients();
	instrucions_has_correct_amount_of_instructions();
	first_instruction_has_correct_amount_of_ingredients();
	first_instruction_has_correct_ingredients_name();
	first_ingredient_in_first_instruction_has_correct_amount();
	second_ingredient_in_first_instruction_has_correct_amount();
	ingredient_in_instruction_without_amount_has_correct_amount();
	second_instruction_has_no_ingredients();
	third_instruction_has_correct_ingredients_name();
	first_instruction_has_no_kitchenware();
	second_instruction_has_correct_amount_of_kitchenware();
	second_instruction_has_correct_kitchenware_name();
	third_instruction_has_correct_kitchenware_name();
	parsed_instruction_body();
	first_instruction_body_text_is_parsed_correctly();
	instruction_body_amounts_have_is_amount_set_to_true();
	instruction_body_non_amounts_have_is_amount_set_to_false();
	first_instruction_yields_zero_ingredients();
	second_instruction_yields_an_ingredient();
	parse_fields_in_random_order();
	test_lib::print_recap();
	return test_lib::was_success();
}

//mangler test for [amounts i with]