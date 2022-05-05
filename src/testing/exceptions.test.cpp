#include "./test_lib.h"
#include <sstream>

void parse_incorrect_field_exception()
{
    test_lib::REGISTER;

    std::string incorrect_field_string = "incorrect_field: this is not right";
    auto rcp = epicr::parse_string_silent(incorrect_field_string);
    test_lib::expect_exception(rcp, "invalid field: No field with this name");
}

void parse_invalid_servings_amount_exception()
{
    test_lib::REGISTER;

    std::string incorrect_field_string = "servings: people 2";
    auto rcp = epicr::parse_string_silent(incorrect_field_string);
    test_lib::expect_exception(rcp, "No correct description for amount has been found");
}

void parse_invalid_nutrient_amount_exception()
{
    test_lib::REGISTER;

    std::string incorrect_field_string = "nutrients: calories [100 cm]";
    auto rcp = epicr::parse_string_silent(incorrect_field_string);
    test_lib::expect_exception(rcp, "Invalid unit after nutrient");
}

void parse_missing_colon_after_instruction_header_exception()
{
    test_lib::REGISTER;

    std::string incorrect_field_string = "instructions: with(ingredient) using(kitchenware) do something";
    auto rcp = epicr::parse_string_silent(incorrect_field_string);
    test_lib::expect_exception(rcp, "missing ':' after instruction header");
}

void parse_missing_open_bracket_after_with_exception()
{
    test_lib::REGISTER;

    std::string incorrect_field_string = "instructions: with  ingra,ingrb";
    auto rcp = epicr::parse_string_silent(incorrect_field_string);
    test_lib::expect_exception(rcp, "expected open bracket with 'with' ");
}

void parse_expected_seperator_in_with_exception()
{
    test_lib::REGISTER;

    std::string incorrect_field_string = "instructions: with(ingr1]ingr2]ingr3)";
    auto rcp = epicr::parse_string_silent(incorrect_field_string);
    test_lib::expect_exception(rcp, "Expected a ',' as seperator between ingredient or a closing parenthesis for the 'with'");
}
void parse_expected_end_parenthesis_after_with_exception()
{
    test_lib::REGISTER;

    std::string incorrect_field_string = "instructions: with(ingr1,ingr2: do this";
    auto rcp = epicr::parse_string_silent(incorrect_field_string);
    test_lib::expect_exception(rcp, "Expected a ',' as seperator between ingredient or a closing parenthesis for the 'with'");
}
void parse_missing_open_bracket_after_using_exception()
{
    test_lib::REGISTER;

    std::string incorrect_field_string = "instructions: using  k1,k2";
    auto rcp = epicr::parse_string_silent(incorrect_field_string);
    test_lib::expect_exception(rcp, "expected open bracket with 'using' ");
}

void parse_expected_seperator_in_using_exception()
{
    test_lib::REGISTER;

    std::string incorrect_field_string = "instructions: using(k1*k2))";
    auto rcp = epicr::parse_string_silent(incorrect_field_string);
    test_lib::expect_exception(rcp, "Expected a ',' as seperator between kitchenware or a closing parenthesis for the 'using'");
}
void parse_expected_end_parenthesis_after_using_exception()
{
    test_lib::REGISTER;

    std::string incorrect_field_string = "instructions: using(k1,k2: do this please";
    auto rcp = epicr::parse_string_silent(incorrect_field_string);
    test_lib::expect_exception(rcp, "Expected a ',' as seperator between kitchenware or a closing parenthesis for the 'using'");
}

void parse_read_ingredients_expects_ingredient_name_first_exception()
{
    test_lib::REGISTER;
    std::string incorrect_ingredient_string = "ingredients: [12] æbler";
    auto rcp = epicr::parse_string_silent(incorrect_ingredient_string);
    test_lib::expect_exception(rcp, "Expected ingredient name");
}

void parse_nutrients_cannot_use_asterix_exception()
{
    test_lib::REGISTER;
    std::string incorrect_nutrient_string = "nutrients: calories* [12 cal]";
    auto rcp = epicr::parse_string_silent(incorrect_nutrient_string);
    test_lib::expect_exception(rcp, "An asterix is not valid in the given context");
}

void parse_double_asterix_exception()
{
    test_lib::REGISTER;
    std::string incorrect_ingredient_string = "ingredients: salt**";
    epicr::Parser *parser = new epicr::Parser;
    auto rcp = epicr::parse_string_silent(incorrect_ingredient_string, parser);
    test_lib::expect_warning(*parser, "Duplicate asterix");
}

void parse_nutrients_cannot_use_question_mark_exception()
{
    test_lib::REGISTER;
    std::string incorrect_nutrient_string = "nutrients: calories? [12 cal]";
    auto rcp = epicr::parse_string_silent(incorrect_nutrient_string);
    test_lib::expect_exception(rcp, "A question mark is not valid in the given context");
}

void parse_double_question_mark_exception()
{
    test_lib::REGISTER;
    std::string incorrect_ingredient_string = "ingredients: parmesan?*?";
    epicr::Parser *parser = new epicr::Parser;
    auto rcp = epicr::parse_string_silent(incorrect_ingredient_string, parser);
    test_lib::expect_warning(*parser, "Duplicate question mark");
}

void parse_invalid_relative_amount_exception()
{
    test_lib::REGISTER;
    std::string incorrect_instruction_string = "instructions: with(egg[third]):";
    auto rcp = epicr::parse_string_silent(incorrect_instruction_string);
    test_lib::expect_exception(rcp, "Invalid relative amount");
}

void parse_invalid_type_of_amount_exception()
{
    test_lib::REGISTER;
    std::string incorrect_instruction_string = "instructions: with(egg[,]):";
    auto rcp = epicr::parse_string_silent(incorrect_instruction_string);
    test_lib::expect_exception(rcp, "Did not find number or word in amount");
}

void parse_no_comma_as_seperator_for_tags_exception()
{
    test_lib::REGISTER;
    std::string field_name = "tags";
    std::string incorrect_tag_string = field_name + ": tag1,tag2*tag3";
    auto rcp = epicr::parse_string_silent(incorrect_tag_string);
    test_lib::expect_exception(rcp, "expected a comma as a seperator between " + field_name);
}

void parse_body_cannot_be_empty_exception()
{
    test_lib::REGISTER;
    std::string incorrect_instruction = "instructions: with(egg): yield: scrambled eggs";
    auto rcp = epicr::parse_string_silent(incorrect_instruction);
    test_lib::expect_exception(rcp, "Instruction body cannot be empty");
}

void visit_no_repeating_ingredients_exception()
{
    test_lib::REGISTER;
    epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/visitor_exception_recipes/duplicateIngredient.rcp").recipe;
    auto ingrvisit = epicr::visitor::IngredientVerifier();
    auto final_rcp = epicr::ingredient_verify_recipe(&rcp);
    test_lib::expect_exception(final_rcp, "duplicate ingredient, egg was found");
}

void visit_relative_amount_only_on_ingredients_in_ingredient_list_exception()
{
    test_lib::REGISTER;
    epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/visitor_exception_recipes/wrongRelativeAmount.rcp").recipe;
    auto ingrvisit = epicr::visitor::IngredientVerifier();
    auto final_rcp = epicr::ingredient_verify_recipe(&rcp);
    test_lib::expect_exception(final_rcp, "relative amounts can only be used for ingredients in the ingredient list. b is not in the ingredient list");
}

void visit_title_ingredient_must_remain_in_the_end_exception()
{
    test_lib::REGISTER;
    epicr::recipe rcp = epicr::parse_recipe("src/test-recipes/visitor_exception_recipes/noRemainingTitle.rcp").recipe;
    auto ingrvisit = epicr::visitor::IngredientVerifier();
    auto final_rcp = epicr::ingredient_verify_recipe(&rcp);
    test_lib::expect_exception(final_rcp, "Title-ingredient must remain after all instructions have been executed");
}

/*this is if the wordCanBeEmpty boolean parameter for readWords is false*/
void read_words_empty_words_exception()
{
    test_lib::REGISTER;
    std::string incorrect_tag_string = "tags: tag1,tag2,,";
    auto rcp = epicr::parse_string_silent(incorrect_tag_string);

    test_lib::expect_exception(rcp, "word cannot be empty");
}

void visit_mandatory_fields_title_exception()
{
    test_lib::REGISTER;
    epicr::recipe rcp = epicr::parse_string("title: ingredients: instructions:").recipe;
    epicr::visitor::MandatoryFields mand_fields = epicr::visitor::MandatoryFields();
    mand_fields.visit(&rcp);
    epicr::rcp_ret ret = {&rcp, mand_fields.has_error, mand_fields.error};
    test_lib::expect_exception(ret, "No title was found");
}

void visit_mandatory_fields_ingredients_exception()
{
    test_lib::REGISTER;
    epicr::recipe rcp = epicr::parse_string("title: hello ingredients: instructions:").recipe;
    epicr::visitor::MandatoryFields mand_fields = epicr::visitor::MandatoryFields();
    mand_fields.visit(&rcp);
    epicr::rcp_ret ret = {&rcp, mand_fields.has_error, mand_fields.error};
    test_lib::expect_exception(ret, "No ingredients was found");
}

void visit_mandatory_fields_instructions_exception()
{
    test_lib::REGISTER;
    epicr::recipe rcp = epicr::parse_string("title: hello ingredients: carrot instructions:").recipe;
    epicr::visitor::MandatoryFields mand_fields = epicr::visitor::MandatoryFields();
    mand_fields.visit(&rcp);
    epicr::rcp_ret ret = {&rcp, mand_fields.has_error, mand_fields.error};
    test_lib::expect_exception(ret, "No instructions was found");
}

void recursive_files_exception()
{
    test_lib::REGISTER;

    epicr::parse_ret ret = epicr::parse_recipe_silent("src/test-recipes/recursion0.rcp");
    std::string file = std::filesystem::absolute("src/test-recipes/recursion0.rcp").string();
    test_lib::expect_exception(ret, "In recursion1:\n\tIn recursion0:\n\t\tFile " + file + " was already included (recursion)");
}

int main()
{
    parse_incorrect_field_exception();
    parse_invalid_servings_amount_exception();
    parse_invalid_nutrient_amount_exception();
    parse_missing_colon_after_instruction_header_exception();
    parse_missing_open_bracket_after_with_exception();
    parse_expected_seperator_in_with_exception();
    parse_expected_end_parenthesis_after_with_exception();
    parse_missing_open_bracket_after_using_exception();
    parse_expected_seperator_in_using_exception();
    parse_expected_end_parenthesis_after_using_exception();
    parse_read_ingredients_expects_ingredient_name_first_exception();
    parse_nutrients_cannot_use_asterix_exception();
    parse_double_asterix_exception();
    parse_nutrients_cannot_use_question_mark_exception();
    parse_double_question_mark_exception();
    parse_invalid_relative_amount_exception();
    parse_invalid_type_of_amount_exception();
    parse_body_cannot_be_empty_exception();
    visit_no_repeating_ingredients_exception();
    visit_relative_amount_only_on_ingredients_in_ingredient_list_exception();
    read_words_empty_words_exception();
    visit_mandatory_fields_title_exception();
    visit_mandatory_fields_ingredients_exception();
    visit_mandatory_fields_instructions_exception();
    recursive_files_exception();
    test_lib::print_recap();
    return test_lib::result();
}
