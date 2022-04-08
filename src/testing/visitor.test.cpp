#include "./test_lib.h"

void compatiable_units()
{
    test_lib::REGISTER;

	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/Carbonara.rcp").recipe;
	epicr::visitor::IngredientVerifier verifier = epicr::visitor::IngredientVerifier();
	verifier.visit(rcp);

	if (verifier.has_error != false)
	{
		test_lib::deny(verifier.error);
	}
	
	test_lib::expect_equal_s(rcp.ingredients[0].amount.unit ,"g");
    test_lib::expect_equal_s(rcp.ingredients[5].amount.unit ,"g");
}


void all_ingredients_used()
{
	test_lib::REGISTER;

	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/Carbonara.rcp").recipe;
	epicr::visitor::IngredientVerifier verifier = epicr::visitor::IngredientVerifier();
	verifier.visit(rcp);

	if (verifier.has_error != false)
	{
		test_lib::deny(verifier.error);
	}
	

	test_lib::expect_equal_i(rcp.ingredients[0].amount.amount, 0);
	test_lib::expect_equal_i(rcp.ingredients[1].amount.amount, 0);
}

int main(void){
	compatiable_units();
	//all_ingredients_used();

	test_lib::print_recap();
	return test_lib::was_success();
}