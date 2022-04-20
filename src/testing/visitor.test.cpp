#include "./test_lib.h"

void no_error_on_valid_file()
{
	test_lib::REGISTER;

	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/Carbonara.rcp").recipe;
	epicr::visitor::IngredientVerifier verifier = epicr::visitor::IngredientVerifier();
	verifier.visit(rcp);

	if (verifier.has_error != false)
	{
		test_lib::deny(verifier.error);
	}
	else
	{
		test_lib::accept();
	}
}

void metric_to_imperial_names_conversions()
{
	test_lib::REGISTER;

	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/Conversions.rcp").recipe;
	epicr::clargs.unit_system = epicr::E_US_IMPERIAL;

	epicr::visitor::AmountConverter converter = epicr::visitor::AmountConverter();
	converter.visit(&rcp);

	test_lib::expect_equal_s(rcp.ingredients[0].amount.unit, "oz");
	test_lib::expect_equal_s(rcp.ingredients[1].amount.unit, "lbs");
	test_lib::expect_equal_s(rcp.ingredients[2].amount.unit, "fl-oz");
	test_lib::expect_equal_s(rcp.ingredients[3].amount.unit, "in");
}
/*
void metric_to_imperial_values_conversions()
{
	test_lib::REGISTER;

	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/Conversions.rcp").recipe;
	epicr::clargs.unit_system = epicr::E_US_IMPERIAL;

	epicr::visitor::AmountConverter converter = epicr::visitor::AmountConverter();
	converter.visit(&rcp);

	test_lib::expect_equal_d(rcp.ingredients[0].amount.number, 0.035274f);
	test_lib::expect_equal_s(rcp.ingredients[1].amount.number, 1);
	test_lib::expect_equal_s(rcp.ingredients[2].amount.number, n);
	test_lib::expect_equal_s(rcp.ingredients[3].amount.number, 1);
} */
int main(void)
{
	no_error_on_valid_file();
	metric_to_imperial_names_conversions();
	// metric_to_imperial_values_conversions();

	test_lib::print_recap();
	return test_lib::was_success();
}