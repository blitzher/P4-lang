#include "./test_lib.h"

void no_error_on_valid_file()
{
	test_lib::REGISTER;

	epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/Carbonara.rcp").recipe;
	epicr::visitor::IngredientVerifier verifier = epicr::visitor::IngredientVerifier();
	epicr::visitor::RelativeResolver relative = epicr::visitor::RelativeResolver();
	relative.visit(&rcp);
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

int main(void){
	no_error_on_valid_file();

	test_lib::print_recap();
	return test_lib::was_success();
}