#include "./test_lib.h"

void no_error_on_valid_file()
{
	test_lib::REGISTER;

	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/Carbonara.rcp").recipe;
	epicr::visitor::IngredientVerifier verifier = epicr::visitor::IngredientVerifier();
	verifier.visit(&rcp);

	if (verifier.has_error != false)
	{
		test_lib::deny(verifier.error);
	}
	else
	{
		test_lib::accept();
	}
}

void metric_units_are_standardized()
{
	test_lib::REGISTER;

	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/visitor_test_recipes/standardizationMetric.rcp").recipe;

	epicr::visitor::AmountConverter converter = epicr::visitor::AmountConverter();
	converter.visit(&rcp);

	test_lib::expect_equal_s(rcp.ingredients[0].amount.unit, "g");
	test_lib::expect_equal_s(rcp.ingredients[1].amount.unit, "kg");
	test_lib::expect_equal_s(rcp.ingredients[2].amount.unit, "dl");
	test_lib::expect_equal_s(rcp.ingredients[3].amount.unit, "cm");
}
void imperial_units_are_standardized()
{
	test_lib::REGISTER;

	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/visitor_test_recipes/standardizationImperial.rcp").recipe;

	epicr::visitor::AmountConverter converter = epicr::visitor::AmountConverter();
	converter.visit(&rcp);

	test_lib::expect_equal_s(rcp.ingredients[0].amount.unit, "oz");
	test_lib::expect_equal_s(rcp.ingredients[1].amount.unit, "lbs");
	test_lib::expect_equal_s(rcp.ingredients[2].amount.unit, "qt");
	test_lib::expect_equal_s(rcp.ingredients[3].amount.unit, "in");
}

void metric_to_imperial_names_conversions()
{
	test_lib::REGISTER;

	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/visitor_test_recipes/Conversions.rcp").recipe;
	epicr::clargs.unit_system = epicr::E_US_IMPERIAL;

	epicr::visitor::AmountConverter converter = epicr::visitor::AmountConverter();
	converter.visit(&rcp);

	test_lib::expect_equal_s(rcp.ingredients[0].amount.unit, "oz");
	test_lib::expect_equal_s(rcp.ingredients[1].amount.unit, "lbs");
	test_lib::expect_equal_s(rcp.ingredients[2].amount.unit, "fl-oz");
	test_lib::expect_equal_s(rcp.ingredients[3].amount.unit, "in");
}

void metric_to_imperial_values_conversions()
{
	test_lib::REGISTER;

	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/visitor_test_recipes/Conversions.rcp").recipe;
	epicr::clargs.unit_system = epicr::E_US_IMPERIAL;

	epicr::visitor::AmountConverter converter = epicr::visitor::AmountConverter();
	converter.visit(&rcp);

	/* 1 gram */
	test_lib::expect_equal_d(rcp.ingredients[0].amount.number, 0.035274f);
	/* 1 kg */
	test_lib::expect_equal_d(rcp.ingredients[1].amount.number, 2.20462f);
	/* 1 dl */
	test_lib::expect_equal_d(rcp.ingredients[2].amount.number, 3.3814f);
	/* 1 cm */
	test_lib::expect_equal_d(rcp.ingredients[3].amount.number, 0.39f);
}

void imperial_to_metric_names_conversions()
{
	test_lib::REGISTER;

	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/ConversionsImperial.rcp").recipe;
	epicr::clargs.unit_system = epicr::E_US_METRIC;

	epicr::visitor::AmountConverter converter = epicr::visitor::AmountConverter();
	converter.visit(&rcp);

	test_lib::expect_equal_s(rcp.ingredients[0].amount.unit, "g");
	test_lib::expect_equal_s(rcp.ingredients[1].amount.unit, "kg");
	test_lib::expect_equal_s(rcp.ingredients[2].amount.unit, "ml");
	test_lib::expect_equal_s(rcp.ingredients[3].amount.unit, "cm");
}

void imperial_to_metric_values_conversions()
{
	test_lib::REGISTER;

	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/ConversionsImperial.rcp").recipe;
	epicr::clargs.unit_system = epicr::E_US_METRIC;

	epicr::visitor::AmountConverter converter = epicr::visitor::AmountConverter();
	converter.visit(&rcp);

	test_lib::expect_equal_d(rcp.ingredients[0].amount.number, 28.35);
	test_lib::expect_equal_d(rcp.ingredients[1].amount.number, 0.453592);
	test_lib::expect_equal_d(rcp.ingredients[2].amount.number, 29.5735);
	test_lib::expect_equal_d(rcp.ingredients[3].amount.number, 2.564103);
	test_lib::expect_equal_d(rcp.ingredients[4].amount.number, 3.78541f);
	test_lib::expect_equal_d(rcp.ingredients[5].amount.number, 2.36588f);
}

void imperial_weight_units_are_compatible()
{
	test_lib::REGISTER;

	std::string recipeString = "title: a  ingredients: bacon [1 lbs]  instructions: with(bacon[16 oz]): do this yield: a";
	auto rcp = epicr::parse_string_silent(recipeString).recipe;
	epicr::visitor::IngredientVerifier verifier = epicr::visitor::IngredientVerifier();
	verifier.visit(&rcp);

	test_lib::expect_equal_b(verifier.has_error, false);
}
void imperial_volume_units_are_compatible()
{
	test_lib::REGISTER;

	std::string recipeString = "title: a  ingredients: water [1 qt]  instructions: with(water[4 cup]): do this yield: a";
	auto rcp = epicr::parse_string_silent(recipeString).recipe;
	epicr::visitor::IngredientVerifier verifier = epicr::visitor::IngredientVerifier();
	verifier.visit(&rcp);

	test_lib::expect_equal_b(verifier.has_error, false);
}
void imperial_length_units_are_compatible()
{
	test_lib::REGISTER;

	std::string recipeString = "title: a  ingredients: cocaine [1 ft]  instructions: with(cocaine[12 in]): do this yield: a";
	auto rcp = epicr::parse_string_silent(recipeString).recipe;
	epicr::visitor::IngredientVerifier verifier = epicr::visitor::IngredientVerifier();
	verifier.visit(&rcp);

	test_lib::expect_equal_b(verifier.has_error, false);
}

void metric_weight_units_are_compatible()
{
	test_lib::REGISTER;

	std::string recipeString = "title: a  ingredients: bacon [100 g]  instructions: with(bacon[0.1 kg]): do this yield: a";
	auto rcp = epicr::parse_string_silent(recipeString).recipe;
	epicr::visitor::IngredientVerifier verifier = epicr::visitor::IngredientVerifier();
	verifier.visit(&rcp);

	test_lib::expect_equal_b(verifier.has_error, false);
}
void metric_volume_units_are_compatible()
{
	test_lib::REGISTER;

	std::string recipeString = "title: a  ingredients: water [100 ml]  instructions: with(water[1 dl]): do this yield: a";
	auto rcp = epicr::parse_string_silent(recipeString).recipe;
	epicr::visitor::IngredientVerifier verifier = epicr::visitor::IngredientVerifier();
	verifier.visit(&rcp);

	test_lib::expect_equal_b(verifier.has_error, false);
}
void metric_length_units_are_compatible()
{
	test_lib::REGISTER;

	std::string recipeString = "title: a  ingredients: cocaine [10 mm]  instructions: with(cocaine[1 cm]): do this yield: a";
	auto rcp = epicr::parse_string_silent(recipeString).recipe;
	epicr::visitor::IngredientVerifier verifier = epicr::visitor::IngredientVerifier();
	verifier.visit(&rcp);

	test_lib::expect_equal_b(verifier.has_error, false);
}

void mandatory_fields_check()
{
	test_lib::REGISTER;

	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/Carbonara.rcp").recipe;

	epicr::visitor::MandatoryFields mand_fields = epicr::visitor::MandatoryFields();
	mand_fields.visit(&rcp);
	if (mand_fields.has_error)
		test_lib::deny("An unexpected error has occured");
	else
		test_lib::accept();
}

void resolved_percentage_relative() {
	test_lib::REGISTER;

	epicr::recipe rcp = epicr::parse_string("title: stuff ingredients: bacon [250g] instructions: with(bacon[100%]): do stuff yield: stuff").recipe;

	auto verifier = epicr::visitor::IngredientVerifier();
	verifier.visit(&rcp);

	test_lib::expect_equal_d(rcp.instructions[0].ingredients[0].amount.number, 250);
	test_lib::expect_equal_s(rcp.instructions[0].ingredients[0].amount.unit, "g");
}

void resolved_fraction_relative() {
	test_lib::REGISTER;

	epicr::recipe rcp = epicr::parse_string("title: stuff ingredients: bacon [250g] instructions: with(bacon[1/4]): do stuff yield: stuff").recipe;

	auto verifier = epicr::visitor::IngredientVerifier();
	verifier.visit(&rcp);

	test_lib::expect_equal_d(rcp.instructions[0].ingredients[0].amount.number, 250 * 0.25);
	test_lib::expect_equal_s(rcp.instructions[0].ingredients[0].amount.unit, "g");

}

int main(void)
{
	no_error_on_valid_file();
	metric_units_are_standardized();
	imperial_units_are_standardized();
	metric_to_imperial_names_conversions();
	metric_to_imperial_values_conversions();
	imperial_to_metric_names_conversions();
	imperial_to_metric_values_conversions();
	metric_weight_units_are_compatible();
	metric_volume_units_are_compatible();
	metric_length_units_are_compatible();
	imperial_weight_units_are_compatible();
	imperial_volume_units_are_compatible();
	imperial_length_units_are_compatible();
	mandatory_fields_check();
	resolved_percentage_relative();
	resolved_fraction_relative();
	test_lib::print_recap();
	return test_lib::result();
}