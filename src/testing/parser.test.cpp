#include "./test_lib.h"

void parsed_title()
{
	test_lib::REGISTER;

	epicr::recipe rcp = epicr::parse_recipe("src/testing/Pasta.rcp");
	test_lib::expect_equal_s(rcp.title, "Pasta\n");
	test_lib::expect_equal_s(rcp.instructions[0].yields[0].name, "Pasta");
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
	auto rcp = epicr::parse_recipe("src/testing/Pasta.rcp");
	test_lib::expect_equal_s(rcp.description, "Frisklavet pasta\n");
}

void parsed_instruction_body()
{
	test_lib::REGISTER;
	auto rcp = epicr::parse_recipe("src/testing/Pasta.rcp");
	/* TODO: Take stance on how to deal with instructions words that are amounts */
}

int main(void)
{
	parsed_title();
	parsed_instruction_yield();
	parsed_description();
	parsed_instruction_body();
	test_lib::print_recap();
	return test_lib::was_success();
}
