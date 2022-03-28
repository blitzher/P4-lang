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

void parsed_instruction_body_first_line_is_parsed_correctly()
{
	test_lib::REGISTER;
	auto rcp = epicr::parse_recipe("src/testing/Pasta.rcp");
	size_t actualInstructionBodySize = rcp.instructions[0].body.size();
	std::string expectedInstructionBody = "Put the 300.000000 wheatflour on the table and make a cavity in the middle.";
	//the rest: \nCrack the 3.000000 eggs in the cavity, and add salt.\nStart mixing the eggs into the wheatflour.\nIf the dough becomes to dry, add water, and if it becomes to sticky add wheatflour.\nKnead the dough thoroughly.\nAfter kneading, let the dough rest in plastic wrap for 30 minutes.\nAdd wheatflour to the table, and begin rolling out the dough thinly using a rolling pin.\nStart from the middle and work towards the edges.\nWhen the dough is the desired thickness, cut the dough into thin strips.\nDust the thin strips with wheatflour, and roll the strips into a nest.\n
	std::string actualInstructionBody = "";
	for (size_t i = 0; i < actualInstructionBodySize; i++)
		{
			if (rcp.instructions[0].body[i].spelling == "\n")
				break;
			actualInstructionBody += rcp.instructions[0].body[i].spelling;
			if (rcp.instructions[0].body[i].is_amount)
				actualInstructionBody += std::to_string(rcp.instructions[0].body[i].value.amount) + " ";
		}
	test_lib::expect_equal_s(expectedInstructionBody,actualInstructionBody);
	/* TODO: Take stance on how to deal with instructions words that are amounts 
	works now, but the 0's after the decimal-seperator should be omitted*/
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
	parsed_instruction_body_first_line_is_parsed_correctly();
	parsed_instruction_yield();
	test_lib::print_recap();
	return test_lib::was_success();
}
