#include "./test_lib.h"

void parsed_title()
{
	test_lib::REGISTER;

	epicr::recipe rcp = epicr::parse_recipe("src/testing/Pasta.rcp");
	test_lib::expect_equal_s(rcp.title, "Pasta\n");
}

void parsed_amount()
{
	test_lib::REGISTER;

	epicr::recipe rcp = epicr::parse_recipe("src/testing/Pasta.rcp");
	test_lib::expect_equal_i(rcp.amount.count, 4);
	test_lib::expect_equal_s(rcp.amount.descriptor, "people\n");
}

void parsed_cook_time()
{
	test_lib::REGISTER;

	epicr::recipe rcp = epicr::parse_recipe("src/testing/Pasta.rcp");
	test_lib::expect_equal_s(rcp.time, "50 min\n");
}

void parsed_tags()
{
	test_lib::REGISTER;

	epicr::recipe rcp = epicr::parse_recipe("src/testing/Pasta.rcp");
	test_lib::expect_equal_s(rcp.tags[0], "pasta");
	test_lib::expect_equal_s(rcp.tags[1], "easy to make");
}

void parsed_kitchenware()
{
	test_lib::REGISTER;

	epicr::recipe rcp = epicr::parse_recipe("src/testing/Pasta.rcp");
	test_lib::expect_equal_s(rcp.kitchenware[0], "Pasta");
	test_lib::expect_equal_s(rcp.kitchenware[1], "roller");
}

void parsed_ingredients()
{
	test_lib::REGISTER;

	epicr::recipe rcp = epicr::parse_recipe("src/testing/Pasta.rcp");

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
	epicr::recipe rcp = epicr::parse_recipe("src/testing/Pasta.rcp");
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

void parsed_instruction_body()
{
	test_lib::REGISTER;
	auto rcp = epicr::parse_recipe("src/testing/Pasta.rcp");
	/* TODO: Take stance on how to deal with instructions words that are amounts */
}

void parse_fields_in_random_order()
{
	test_lib::REGISTER;

	epicr::recipe rcp = epicr::parse_recipe("src/testing/PastaRandomOrder.rcp");

	test_lib::expect_equal_s(rcp.title, "Pasta\n");
	test_lib::expect_equal_s(rcp.instructions[0].ingredients[0].name, "wheatflour");
	test_lib::expect_equal_s(rcp.description, "Frisklavet pasta\n");
	test_lib::expect_equal_i(rcp.ingredients[0].amount.amount, 300);
}

int main(void)
{
	parsed_title();
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
