#include "./test_lib.h"

void pasta_recipe_parses_without_error()
{
	test_lib::REGISTER;
	std::ifstream f = epicr::open_file("src/test-recipes/Pasta.rcp");
	epicr::Lexer l = epicr::Lexer(f);
	epicr::Parser p = epicr::Parser(&l);
	epicr::recipe rcp = p.Parse();
	test_lib::expect_equal_b(p.has_error, false);
}

void carbonara_recipe_parses_without_error()
{
	test_lib::REGISTER;
	std::ifstream f = epicr::open_file("src/test-recipes/Carbonara.rcp");
	epicr::Lexer l = epicr::Lexer(f);
	epicr::Parser p = epicr::Parser(&l);
	epicr::recipe rcp = p.Parse();
	test_lib::expect_equal_b(p.has_error, false);
}

void parsed_title()
{
	test_lib::REGISTER;

	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;
	test_lib::expect_equal_s(rcp.title, "Pasta");
}

void parsed_description()
{
	test_lib::REGISTER;
	std::ifstream f = epicr::open_file("src/test-recipes/Pasta.rcp");
	epicr::Lexer l = epicr::Lexer(f);
	epicr::Parser p = epicr::Parser(&l);
	epicr::recipe rcp = p.Parse();
	test_lib::expect_equal_s(rcp.description, "Frisklavet pasta");
}

void parsed_servings()
{
	test_lib::REGISTER;

	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;
	test_lib::expect_equal_i(rcp.servings.count, 4);
	test_lib::expect_equal_s(rcp.servings.descriptor, "people");
}

void parsed_prep_time()
{
	test_lib::REGISTER;

	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;
	test_lib::expect_equal_s(rcp.time.prep_time, "5-10 min");
}

void parsed_cook_time()
{
	test_lib::REGISTER;

	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;
	test_lib::expect_equal_s(rcp.time.cook_time, "50 min");
}

void parsed_total_time()
{
	test_lib::REGISTER;

	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;
	test_lib::expect_equal_s(rcp.time.total_time, "around 1 hour");
}

void parsed_tags()
{
	test_lib::REGISTER;

	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;
	test_lib::expect_equal_i(rcp.tags.size(), 3);
	if (rcp.tags.size() < 3)
		return;
	test_lib::expect_equal_s(rcp.tags[0], "pasta");
	test_lib::expect_equal_s(rcp.tags[1], "easy to make (for most)");
	test_lib::expect_equal_s(rcp.tags[2], "under 2 hours");
}

void parsed_kitchenware()
{
	test_lib::REGISTER;

	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;
	test_lib::expect_equal_i(rcp.kitchenware.size(), 2);
	if (rcp.kitchenware.size() < 2)
		return;
	test_lib::expect_equal_s(rcp.kitchenware[0], "plastic wrap");
	test_lib::expect_equal_s(rcp.kitchenware[1], "rolling pin");
}

void parsed_ingredients()
{
	test_lib::REGISTER;

	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;

	if (rcp.ingredients.size() < 5)
	{
		char* m = (char*)malloc(100);
		sprintf(m, "Not all ingredients were parsed properly, found %zu ingredients", rcp.ingredients.size());
		test_lib::deny(m);
		return;
	}

	test_lib::expect_equal_s(rcp.ingredients[4].name, "extra wheatflour");
	if (!rcp.ingredients[4].is_optional)
	{
		test_lib::deny("Expected ingredient extra wheatflour to be optional");
	}
	if (!rcp.ingredients[4].amount.is_uncountable)
	{
		test_lib::deny("Expected ingredient extra wheatflour to be uncountable");
	}
	if (rcp.ingredients[4].amount.number != std::numeric_limits<double>::infinity())
	{
		test_lib::deny("Expected ingredient extra wheatflour to have amount set to INF");
	}
}

void instrucions_has_correct_number_of_instructions()
{
	test_lib::REGISTER;
	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;
	test_lib::expect_equal_i(rcp.instructions.size(), 3);
}

void instruction_has_correct_amount_of_ingredients()
{
	test_lib::REGISTER;
	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;
	test_lib::expect_equal_i(rcp.instructions[0].ingredients.size(), 4);
}

void instruction_has_correct_ingredients_name()
{
	test_lib::REGISTER;
	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;

	std::vector<std::string> ingredients = { "wheatflour", "egg", "salt", "water" };
	size_t ingredient_size = ingredients.size();
	for (size_t i = 0; i < ingredient_size; i++)
	{
		test_lib::expect_equal_s(rcp.instructions[0].ingredients[i].name, ingredients[i]);
	}

	test_lib::expect_equal_s(rcp.instructions[2].ingredients[0].name, "dough");
	test_lib::expect_equal_s(rcp.instructions[2].ingredients[1].name, "extra wheatflour");
}

void ingredient_in_instruction_has_correct_amount()
{
	test_lib::REGISTER;
	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;

	/* First ingredient */
	test_lib::expect_equal_b(rcp.instructions[0].ingredients[0].amount.is_relative_amount, false);
	test_lib::expect_equal_i(rcp.instructions[0].ingredients[0].amount.number, 300);
	test_lib::expect_equal_s(rcp.instructions[0].ingredients[0].amount.unit, "g");

	/* Second ingredient */
	test_lib::expect_equal_b(rcp.instructions[0].ingredients[1].amount.is_relative_amount, true);
	test_lib::expect_equal_s(rcp.instructions[0].ingredients[1].amount.relative_amount, "all");
}

void ingredient_in_instruction_without_amount_has_correct_amount()
{
	test_lib::REGISTER;
	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;

	epicr::amount amnt = rcp.instructions[2].ingredients[0].amount;
	test_lib::expect_equal_b(amnt.is_relative_amount, true);
	test_lib::expect_equal_b(amnt.is_uncountable, false);
	test_lib::expect_equal_s(amnt.unit, "");
	test_lib::expect_equal_s(amnt.relative_amount, "rest");
	test_lib::expect_equal_i(amnt.number, 0);
}

void second_instruction_has_no_ingredients()
{
	test_lib::REGISTER;
	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;
	test_lib::expect_equal_i(rcp.instructions[1].ingredients.size(), 0);
}

void first_instruction_has_no_kitchenware()
{
	test_lib::REGISTER;
	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;
	test_lib::expect_equal_i(rcp.instructions[0].kitchenware.size(), 0);
}

void instruction_has_correct_amount_of_kitchenware()
{
	test_lib::REGISTER;
	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;
	test_lib::expect_equal_i(rcp.instructions[1].kitchenware.size(), 1);
}

void instruction_has_correct_kitchenware_name()
{
	test_lib::REGISTER;
	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;
	test_lib::expect_equal_s(rcp.instructions[1].kitchenware[0], "plastic wrap");
	test_lib::expect_equal_s(rcp.instructions[2].kitchenware[0], "kitchenroller");
}

void instruction_with_can_be_empty()
{
	test_lib::REGISTER;
	epicr::parse_ret parse_return = epicr::parse_string("instructions: with(): get some water from the tap yield: water");
	test_lib::expect_equal_b(parse_return.has_err, false);
	test_lib::expect_equal_i(parse_return.recipe.instructions[0].ingredients.size(), 0);
}

void instruction_using_can_be_empty()
{
	test_lib::REGISTER;
	epicr::parse_ret parse_return = epicr::parse_string("instructions: using(): Wash your hands thoroughly");
	test_lib::expect_equal_b(parse_return.has_err, false);
	test_lib::expect_equal_i(parse_return.recipe.instructions[0].kitchenware.size(), 0);
}

void parsed_instruction_body()
{
	test_lib::REGISTER;
	auto rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;
	auto body = rcp.instructions[0].body;
	test_lib::expect_equal_s(body[0].spelling, "Put");
	test_lib::expect_equal_s(body[1].spelling, " ");
	test_lib::expect_equal_s(body[2].spelling, "the");
	test_lib::expect_equal_s(body[3].spelling, " ");
	test_lib::expect_equal_b(body[4].is_amount, true);
	test_lib::expect_equal_i(body[4].value.number, 300);
	test_lib::expect_equal_s(body[4].value.unit, "g");
}
void instruction_body_text_is_parsed_correctly()
{
	test_lib::REGISTER;
	auto rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;
	size_t actual_instruction_body_size = rcp.instructions[0].body.size();
	std::string expected_instruction_body = std::string("Put the 300 g wheatflour on the table and make a cavity in the middle.\n") +
											std::string("Crack the 3 eggs in the cavity, and add salt.\n") +
											std::string("Start mixing the eggs into the wheatflour.\n") +
											std::string("If the dough becomes too dry, add water, and if it becomes too sticky add wheatflour.\n") +
											std::string("Knead the dough thoroughly.");
	std::string actual_instruction_body = "";
	for (size_t i = 0; i < actual_instruction_body_size; i++)
	{
		actual_instruction_body += rcp.instructions[0].body[i].spelling;
		if (rcp.instructions[0].body[i].is_amount)
		{
			actual_instruction_body += epicr::round_double_to_string(rcp.instructions[0].body[i].value.number) + " ";
			if (rcp.instructions[0].body[i].value.unit != "")
				actual_instruction_body += rcp.instructions[0].body[i].value.unit + " ";
		}
	}
	test_lib::expect_equal_s(expected_instruction_body, actual_instruction_body);
	/* TODO: Take stance on how to deal with instructions words that are amounts
	works now, but the 0's after the decimal-seperator should be omitted*/
}

void instruction_body_amounts_have_is_amount_set_to_true()
{
	test_lib::REGISTER;
	auto rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;
	test_lib::expect_equal_b(rcp.instructions[0].body[4].is_amount, true);
}

void instruction_body_non_amounts_have_is_amount_set_to_false()
{
	test_lib::REGISTER;
	auto rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;
	test_lib::expect_equal_b(rcp.instructions[0].body[0].is_amount, false);
	test_lib::expect_equal_b(rcp.instructions[0].body[11].is_amount, false);
	test_lib::expect_equal_b(rcp.instructions[1].body[0].is_amount, false);
}

void instruction_yields_zero_ingredients()
{
	test_lib::REGISTER;
	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;
	test_lib::expect_equal_i(rcp.instructions[0].yields.size(), 0);
}

void instruction_yields_an_ingredient()
{
	test_lib::REGISTER;
	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;
	test_lib::expect_equal_s(rcp.instructions[1].yields[0].name, "dough");
}

void instruction_yields_1_if_no_amount_specified()
{
	test_lib::REGISTER;
	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;
	test_lib::expect_equal_i(rcp.instructions[1].yields[0].amount.number, 1);
	test_lib::expect_equal_s(rcp.instructions[1].yields[0].amount.unit, "");
}

void last_instruction_yields_correct_ingredient_name()
{
	test_lib::REGISTER;
	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/Pasta.rcp").recipe;
	test_lib::expect_equal_s(rcp.instructions[2].yields[0].name, "Pasta");
}

void parse_fields_in_random_order()
{
	test_lib::REGISTER;

	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/PastaRandomOrder.rcp").recipe;

	test_lib::expect_equal_s(rcp.title, "Pasta");
	test_lib::expect_equal_i(rcp.instructions.size(), 3);
	test_lib::expect_equal_s(rcp.description, "Frisklavet pasta");
	test_lib::expect_equal_i(rcp.ingredients.size(), 5);
	test_lib::expect_equal_i(rcp.ingredients[0].amount.number, 300);
}

void trailing_commas_is_allowed()
{
	test_lib::REGISTER;
	std::string incorrect_tag_string = "tags: tag1,tag2,";
	auto rcp = epicr::parse_string_silent(incorrect_tag_string).recipe;
	test_lib::expect_equal_i(rcp.tags.size(), 2);
}

void parse_recipe_with_random_casing()
{
	test_lib::REGISTER;

	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/PastaRandomCasing.rcp").recipe;

	test_lib::expect_equal_s(rcp.title, "Pasta");
	test_lib::expect_equal_s(rcp.description, "Frisklavet pasta");
	test_lib::expect_equal_i(rcp.servings.count, 4);
	test_lib::expect_equal_i(rcp.ingredients.size(), 4);
	test_lib::expect_equal_i(rcp.instructions.size(), 1);
	test_lib::expect_equal_i(rcp.instructions[0].ingredients.size(), 4);
	test_lib::expect_equal_i(rcp.instructions[0].yields.size(), 1);
}

void parse_ingredients_with_non_ascii_chars()
{
	test_lib::REGISTER;
	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/ingrWithNonAscii.rcp").recipe;
	test_lib::expect_equal_s(rcp.ingredients[0].name, "??ble????");
	test_lib::expect_equal_s(rcp.ingredients[1].name, "r??d ??lborg");
	test_lib::expect_equal_s(rcp.ingredients[2].name, "sorb??t");
	test_lib::expect_equal_s(rcp.ingredients[3].name, "???");
	test_lib::expect_equal_s(rcp.ingredients[4].name, "????????????????????");
}

void parse_percentage_relative() {
	test_lib::REGISTER;

	epicr::recipe rcp = epicr::parse_string("ingredients: bacon [100g] instructions: with(bacon[100%]): do stuff").recipe;
	test_lib::expect_equal_b(rcp.instructions[0].ingredients[0].amount.is_relative_amount, true);
	test_lib::expect_equal_d(rcp.instructions[0].ingredients[0].amount.number, 1.00);
	test_lib::expect_equal_s(rcp.instructions[0].ingredients[0].amount.unit, "%");
}

void parse_fraction_relative() {
	test_lib::REGISTER;

	epicr::recipe rcp = epicr::parse_string("ingredients: bacon [100g] instructions: with(bacon[1/10]): do stuff").recipe;
	test_lib::expect_equal_b(rcp.instructions[0].ingredients[0].amount.is_relative_amount, true);
	test_lib::expect_equal_d(rcp.instructions[0].ingredients[0].amount.number, 0.10);
	test_lib::expect_equal_s(rcp.instructions[0].ingredients[0].amount.unit, "%");
}

int main(void)
{
	pasta_recipe_parses_without_error();
	carbonara_recipe_parses_without_error();
	parsed_title();
	parsed_description();
	parsed_servings();
	parsed_prep_time();
	parsed_cook_time();
	parsed_total_time();
	parsed_tags();
	parsed_ingredients();
	parsed_kitchenware();
	instrucions_has_correct_number_of_instructions();
	instruction_has_correct_amount_of_ingredients();
	instruction_has_correct_ingredients_name();
	ingredient_in_instruction_has_correct_amount();
	ingredient_in_instruction_without_amount_has_correct_amount();
	second_instruction_has_no_ingredients();
	first_instruction_has_no_kitchenware();
	instruction_has_correct_amount_of_kitchenware();
	instruction_has_correct_kitchenware_name();
	instruction_with_can_be_empty();
	instruction_using_can_be_empty();
	parsed_instruction_body();
	instruction_body_text_is_parsed_correctly();
	instruction_body_amounts_have_is_amount_set_to_true();
	instruction_body_non_amounts_have_is_amount_set_to_false();
	instruction_yields_zero_ingredients();
	instruction_yields_an_ingredient();
	instruction_yields_1_if_no_amount_specified();
	last_instruction_yields_correct_ingredient_name();
	trailing_commas_is_allowed();
	parse_fields_in_random_order();
	parse_recipe_with_random_casing();
	parse_ingredients_with_non_ascii_chars();
	parse_percentage_relative();
	parse_fraction_relative();
	test_lib::print_recap();
	return test_lib::result();
}
