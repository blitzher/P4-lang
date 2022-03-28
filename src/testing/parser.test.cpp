#include "./test_lib.h"

void parsed_title()
{
	test_lib::REGISTER;

	epicr::recipe rcp = epicr::parse_recipe("src/testing/Pasta.rcp");
	test_lib::expect_equal_s(rcp.title, "Pasta\n");
	test_lib::expect_equal_s(rcp.instructions[0].yields[0].name, "Pasta");
}

void parsed_description()
{
	test_lib::REGISTER;
	std::ifstream f = epicr::open_file("src/testing/Pasta.rcp");
	epicr::Lexer l = epicr::Lexer(f);
	l.DEBUG_MODE = true;
	epicr::Parser p = epicr::Parser(&l);
	p.DEBUG_MODE = true;
	epicr::recipe rcp = p.Parse();
	test_lib::expect_equal_s(rcp.description, "Frisklavet pasta\n");
}

void parsed_instruction_has_correct_amount_of_ingredients()
{
	test_lib::REGISTER;
	epicr::recipe rcp = epicr::parse_recipe("src/testing/Pasta.rcp");
	test_lib::expect_equal_i(rcp.instructions[0].ingredients.size(), 5);
}

void parsed_instruction_ingredients_name()
{
	std::vector<std::string> ingredients = {"wheatflour", "egg", "salt", "water", "extra wheatflour"};
	test_lib::REGISTER;
	epicr::recipe rcp = epicr::parse_recipe("src/testing/Pasta.rcp");
	size_t ingredientSize = ingredients.size();
	for (size_t i = 0; i < ingredientSize; i++)
	{
		test_lib::expect_equal_s(rcp.instructions[0].ingredients[i].name, ingredients[i]);	
	}
}
void parsed_instruction_has_correct_amount_of_kitchenware()
{
	test_lib::REGISTER;
	epicr::recipe rcp = epicr::parse_recipe("src/testing/Pasta.rcp");
	test_lib::expect_equal_i(rcp.instructions[0].kitchenware.size(), 2);
}

void parsed_instruction_kitchenware_name()
{
	std::vector<std::string> kitchenware = {"plastic wrap","rolling pin"};
	test_lib::REGISTER;
	epicr::recipe rcp = epicr::parse_recipe("src/testing/Pasta.rcp");
	size_t kitchenwareSize = kitchenware.size();
	for (size_t i = 0; i < kitchenwareSize; i++)
	{
		test_lib::expect_equal_s(rcp.instructions[0].kitchenware[i], kitchenware[i]);	
	}
}

void parsed_instruction_body_text_is_parsed_correctly()
{
	test_lib::REGISTER;
	auto rcp = epicr::parse_recipe("src/testing/Pasta.rcp");
	size_t actualInstructionBodySize = rcp.instructions[0].body.size();
	std::string expectedInstructionBody = "Put the wheatflour on the table and make a cavity in the middle. Crack the 3.000000 eggs in the cavity, and add salt."
        "Start mixing the eggs into the wheatflour."
        "If the dough becomes to dry, add water, and if it becomes to sticky add wheatflour."
        "Knead the dough thoroughly."
        "After kneading, let the dough rest in plastic wrap for 30 minutes."
        "Add wheatflour to the table, and begin rolling out the dough thinly using a kitchenroller."
        "Start from the middle and work towards the edges."
        "When the dough is the desired thickness, cut the dough into thin strips."
        "Dust the thin strips with wheatflour, and roll the strips into a nest.";
	std::string actualInstructionBody = "";
	for (size_t i = 0; i < actualInstructionBodySize; i++)
		{
			actualInstructionBody += rcp.instructions[0].body[i].spelling;
			if (rcp.instructions[0].body[i].is_amount)
				actualInstructionBody += std::to_string(rcp.instructions[0].body[i].value.amount) + " ";
		}
	test_lib::expect_equal_s(actualInstructionBody,expectedInstructionBody);
	/* TODO: Take stance on how to deal with instructions words that are amounts */
}


void parsed_instruction_yield()
{
	test_lib::REGISTER;
	epicr::recipe rcp = epicr::parse_recipe("src/testing/Pasta.rcp");
	test_lib::expect_equal_s(rcp.instructions[0].yields[0].name, "Pasta");
}

int main(void)
{
	parsed_title();
	parsed_description();
	parsed_instruction_has_correct_amount_of_ingredients();
	parsed_instruction_ingredients_name();
	parsed_instruction_has_correct_amount_of_kitchenware();
	parsed_instruction_kitchenware_name();
	//parsed_instruction_body_text_is_parsed_correctly();
	parsed_instruction_yield();
	test_lib::print_recap();
	return test_lib::was_success();
}
