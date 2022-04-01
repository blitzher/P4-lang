#include "./test_lib.h"

void parsed_title()
{
	test_lib::REGISTER;

	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;
	test_lib::expect_equal_s(rcp.title, "Pasta\n");
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

	test_lib::expect_equal_s(rcp.ingredients[3].name, "water");
	if (!rcp.ingredients[3].isOptional)
	{
		test_lib::deny("Expected ingredient water to be optional");
	}
	if (!rcp.ingredients[3].amount.isUncountable)
	{
		test_lib::deny("Expected ingredient water to be uncountable");
	}
}

void parsed_instruction_yield()
{
	test_lib::REGISTER;
	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;
	test_lib::expect_equal_s(rcp.instructions[0].yields[0].name, "Pasta");
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

void parsed_instruction_has_correct_amount_of_ingredients()
{
	test_lib::REGISTER;
	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;
	test_lib::expect_equal_i(rcp.instructions[0].ingredients.size(), 5);
}

void parsed_instruction_ingredients_name()
{
	std::vector<std::string> ingredients = {"wheatflour", "egg", "salt", "water", "extra wheatflour"};
	test_lib::REGISTER;
	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;
	size_t ingredientSize = ingredients.size();
	for (size_t i = 0; i < ingredientSize; i++)
	{
		test_lib::expect_equal_s(rcp.instructions[0].ingredients[i].name, ingredients[i]);	
	}
}
void parsed_instruction_has_correct_amount_of_kitchenware()
{
	test_lib::REGISTER;
	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;
	test_lib::expect_equal_i(rcp.instructions[0].kitchenware.size(), 2);
}

void parsed_instruction_kitchenware_name()
{
	std::vector<std::string> kitchenware = {"plastic wrap","rolling pin"};
	test_lib::REGISTER;
	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;
	size_t kitchenwareSize = kitchenware.size();
	for (size_t i = 0; i < kitchenwareSize; i++)
	{
		test_lib::expect_equal_s(rcp.instructions[0].kitchenware[i], kitchenware[i]);	
	}
}

void parsed_instruction_body_amounts_have_is_amount_set_to_true()
{
	test_lib::REGISTER;
	auto rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;
	test_lib::expect_equal_i(1,rcp.instructions[0].body[4].is_amount); //should be true - ie 1
}

void parsed_instruction_body_non_amounts_have_is_amount_set_to_false()
{
	test_lib::REGISTER;
	auto rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;
	test_lib::expect_equal_i(0,rcp.instructions[0].body[0].is_amount); //should be false - ie 0	
}

void parsed_instruction_body_text_is_parsed_correctly()
{
	test_lib::REGISTER;
	auto rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;
	size_t actualInstructionBodySize = rcp.instructions[0].body.size();
	std::string expectedInstructionBody = std::string("Put the 300.000000 g wheatflour on the table and make a cavity in the middle.\n")+
										  std::string("Crack the 3.000000 eggs in the cavity, and add salt.\n")+
										  std::string("Start mixing the eggs into the wheatflour.\n")+
								 		  std::string("If the dough becomes to dry, add water, and if it becomes to sticky add wheatflour.\n")+
								 		  std::string("Knead the dough thoroughly.\n")+
								 		  std::string("After kneading, let the dough rest in plastic wrap for 30 minutes.\n")+
										  std::string("Add wheatflour to the table, and begin rolling out the dough thinly using a rolling pin.\n")+
									 	  std::string("Start from the middle and work towards the edges.\n")+
										  std::string("When the dough is the desired thickness, cut the dough into thin strips.\n")+
										  std::string("Dust the thin strips with wheatflour, and roll the strips into a nest.\n");
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
	parsed_instruction_has_correct_amount_of_ingredients();
	parsed_instruction_ingredients_name();
	parsed_instruction_has_correct_amount_of_kitchenware();
	parsed_instruction_kitchenware_name();
	parsed_instruction_body_amounts_have_is_amount_set_to_true();
	parsed_instruction_body_non_amounts_have_is_amount_set_to_false();
	parsed_instruction_body_text_is_parsed_correctly();
	parsed_instruction_body();
	parsed_amount();
	parsed_cook_time();
	parsed_tags();
	parsed_kitchenware();
	parsed_ingredients();
	parsed_instruction_yield();
	parsed_description();
	parsed_instruction_body();
	parse_fields_in_random_order();
	test_lib::print_recap();
	return test_lib::was_success();
}
