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
    test_lib::expect_exception(rcp, "No correct description for amount has been found!");
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
    auto rcp = epicr::parse_string_silent(incorrect_ingredient_string);
    test_lib::expect_exception(rcp, "Duplicate asterix");
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
    auto rcp = epicr::parse_string_silent(incorrect_ingredient_string);
    test_lib::expect_exception(rcp, "Duplicate question mark");
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
    test_lib::print_recap();
    return test_lib::result();
}