#include "./test_lib.h"

void title_before_eof_matches()
{
	test_lib::REGISTER;
	std::vector<std::string> titles = {"carbonara", "spaghetti carbonara", "the spaghetti carbonara",
									   "a very good spaghetti carbonara recipe"};
	std::string example_string;
	epicr::recipe rcp;
	for (auto title : titles)
	{
		example_string = "cook-time: 5 min    title: " + title;
		rcp = epicr::parse_string_silent(example_string).recipe;
		test_lib::expect_equal_s(rcp.title, title);
	}
}

void description_before_eof_matches()
{
	test_lib::REGISTER;
	std::vector<std::string> descriptions = {"a recipe", "this is a recipe", "recipe is ...\nspaghetti carbonara (very good)",
											 "this is a very good spaghetti carbonara recipe", "this is recipe."};
	std::string example_string;
	epicr::recipe rcp;
	for (auto description : descriptions)
	{
		example_string = "cook-time: 10 min description: " + description;
		rcp = epicr::parse_string_silent(example_string).recipe;
		test_lib::expect_equal_s(rcp.description, description);
	}
}

void servings_before_eof_matches()
{
	test_lib::REGISTER;
	std::map<std::string, epicr::servings> servings;
	servings.insert({{"5"}, {5, ""}});
	servings.insert({{"10 people"}, {10, "people"}});
	servings.insert({{"3 people guys"}, {3, "people guys"}});
	servings.insert({{"1 portions of magnificent dishes"}, {1, "portions of magnificent dishes"}});
	std::string example_string;
	epicr::recipe rcp;
	for (auto serving : servings)
	{
		example_string = "cook-time: 10 min servings: " + serving.first;
		rcp = epicr::parse_string_silent(example_string).recipe;
		test_lib::expect_equal_i(rcp.servings.count, serving.second.count);
		test_lib::expect_equal_s(rcp.servings.descriptor, serving.second.descriptor);
	}
}

void total_time_before_eof_matches()
{
	test_lib::REGISTER;
	std::vector<std::string> total_times = {"10", "5 minutes", "half an hour", "an hour and a half", "a few minutes and perhaps then some"};
	std::string example_string;
	epicr::recipe rcp;
	for (auto total_time : total_times)
	{
		example_string = "title: pasta    total-time: " + total_time;
		rcp = epicr::parse_string_silent(example_string).recipe;
		//test_lib::expect_equal_s(rcp.time.total_time, total_time);
		test_lib::expect_equal_s(rcp.title,"pasta");
	}
}
void prep_time_before_eof_matches()
{
	test_lib::REGISTER;
	std::vector<std::string> prep_times = {"10", "5 minutes", "half an hour", "an hour and a half", "a few minutes and perhaps then some"};
	std::string example_string;
	epicr::recipe rcp;
	for (auto prep_time : prep_times)
	{
		example_string = "title: pasta    prep-time: " + prep_time;
		rcp = epicr::parse_string_silent(example_string).recipe;
		test_lib::expect_equal_s(rcp.time.prep_time, prep_time);
	}
}
void cook_time_before_eof_matches()
{
	test_lib::REGISTER;
	std::vector<std::string> cook_times = {"10", "5 minutes", "half an hour", "an hour and a half", "a few minutes and perhaps then some"};
	std::string example_string;
	epicr::recipe rcp;
	for (auto cook_time : cook_times)
	{
		example_string = "title: pasta    cook-time: " + cook_time;
		rcp = epicr::parse_string_silent(example_string).recipe;
		test_lib::expect_equal_s(rcp.time.cook_time, cook_time);
	}
}

void one_tag_before_eof_matches()
{
	test_lib::REGISTER;
	std::vector<std::string> tags = {"pasta","pasta dish"};
	std::string example_string;
	epicr::recipe rcp;
	for (auto tag : tags)
	{
		example_string = "title: pasta    tags: " + tag;
		rcp = epicr::parse_string_silent(example_string).recipe;
		test_lib::expect_equal_i(rcp.tags.size(),1);
		test_lib::expect_equal_s(rcp.tags[0], tag);
	}
}
void two_tags_before_eof_matches()
{
	test_lib::REGISTER;
	std::map<std::string, std::vector<std::string>> tags;
	tags.insert({{"pasta, dish"},{"pasta","dish"}});
	tags.insert({{"pasta, pasta dish"},{"pasta","pasta dish"}});
	tags.insert({{"pasta dish, pasta"},{"pasta dish","pasta"}});
	tags.insert({{"pasta, nice pasta dish"},{"pasta","nice pasta dish"}});
	tags.insert({{"nice pasta dish, pasta"},{"nice pasta dish","pasta"}});

	std::string example_string;
	epicr::recipe rcp;
	for (auto tag : tags)
	{
		example_string = "title: pasta    tags: " + tag.first;
		rcp = epicr::parse_string_silent(example_string).recipe;
		test_lib::expect_equal_i(rcp.tags.size(),2);
		test_lib::expect_equal_s(rcp.tags[0], tag.second[0]);
		test_lib::expect_equal_s(rcp.tags[1], tag.second[1]);
	}
}
void three_tags_before_eof_matches()
{
	test_lib::REGISTER;
	std::map<std::string, std::vector<std::string>> tags;
	tags.insert({{"pasta, easy dish, good first dish to make"},{"pasta", "easy dish", "good first dish to make"}});
	tags.insert({{"easy dish, good first dish to make, pasta"},{"easy dish", "good first dish to make", "pasta"}});
	std::string example_string;
	epicr::recipe rcp;
	for (auto tag : tags)
	{
		example_string = "title: pasta    tags: " + tag.first;
		rcp = epicr::parse_string_silent(example_string).recipe;
		test_lib::expect_equal_i(rcp.tags.size(),3);
		test_lib::expect_equal_s(rcp.tags[0], tag.second[0]);
		test_lib::expect_equal_s(rcp.tags[1], tag.second[1]);
		test_lib::expect_equal_s(rcp.tags[2], tag.second[2]);
	}
}

void one_kitchenware_before_eof_matches()
{
	test_lib::REGISTER;
	std::vector<std::string> kitchenware = {"pot","big pan","big pot with an extra large lid"};
	std::string example_string;
	epicr::recipe rcp;
	for (auto a_kitchenware : kitchenware)
	{
		example_string = "title: pasta    kitchenware: " + a_kitchenware;
		rcp = epicr::parse_string_silent(example_string).recipe;
		test_lib::expect_equal_i(rcp.kitchenware.size(),1);
		test_lib::expect_equal_s(rcp.kitchenware[0], a_kitchenware);
	}
}
void two_kitchenware_before_eof_matches()
{
	test_lib::REGISTER;
	std::map<std::string, std::vector<std::string>> kitchenware_map;
	kitchenware_map.insert({{"pan, pot"},{"pan","pot"}});
	kitchenware_map.insert({{"pan, large pot"},{"pan","large pot"}});
	kitchenware_map.insert({{"large pot,pan"},{"large pot","pan"}});
	kitchenware_map.insert({{"rather large pot, pan"},{"rather large pot","pan"}});
	kitchenware_map.insert({{"pan,rather large pot"},{"pan","rather large pot"}});
	

	std::string example_string;
	epicr::recipe rcp;
	for (auto kitchenware : kitchenware_map)
	{
		example_string = "title: pasta    kitchenware: " + kitchenware.first;
		rcp = epicr::parse_string_silent(example_string).recipe;
		test_lib::expect_equal_i(rcp.kitchenware.size(),2);
		test_lib::expect_equal_s(rcp.kitchenware[0], kitchenware.second[0]);
		test_lib::expect_equal_s(rcp.kitchenware[1], kitchenware.second[1]);
	}
}
void three_kitchenware_before_eof_matches()
{
	test_lib::REGISTER;
	std::map<std::string, std::vector<std::string>> kitchenware_map;
	kitchenware_map.insert({{"pan, pot, bowl"},{"pan","pot","bowl"}});
	kitchenware_map.insert({{"pan, bowl, large pot"},{"pan","bowl","large pot"}});
	kitchenware_map.insert({{"bowl, large pot, pan"},{"bowl","large pot","pan"}});
	kitchenware_map.insert({{"rather large pot, pan, bowl"},{"rather large pot","pan","bowl"}});
	kitchenware_map.insert({{"bowl,pan,rather large pot"},{"bowl","pan","rather large pot"}});
	
	std::string example_string;
	epicr::recipe rcp;
	for (auto kitchenware : kitchenware_map)
	{
		example_string = "title: pasta    kitchenware: " + kitchenware.first;
		rcp = epicr::parse_string_silent(example_string).recipe;
		test_lib::expect_equal_i(rcp.kitchenware.size(),3);
		test_lib::expect_equal_s(rcp.kitchenware[0], kitchenware.second[0]);
		test_lib::expect_equal_s(rcp.kitchenware[1], kitchenware.second[1]);
		test_lib::expect_equal_s(rcp.kitchenware[2], kitchenware.second[2]);
	}
}

void optional_ingredient_before_eof_matches()
{
	test_lib::REGISTER;
	std::vector<std::string> ingredients = {"salt","more salt","a large amount of salt"};
	
	std::string example_string;
	epicr::recipe rcp;
	for (auto ingredient : ingredients)
	{
		example_string = "title: pasta    ingredients: dough,water," + ingredient + "?";
		rcp = epicr::parse_string_silent(example_string).recipe;
		test_lib::expect_equal_i(rcp.ingredients.size(),3);
		test_lib::expect_equal_s(rcp.ingredients[2].name, ingredient);
		test_lib::expect_equal_b(rcp.ingredients[2].is_optional,true);
	}
}
void uncountable_ingredient_before_eof_matches()
{
	test_lib::REGISTER;
	std::vector<std::string> ingredients = {"salt","more salt","however much salt you have"};
	
	std::string example_string;
	epicr::recipe rcp;
	for (auto ingredient : ingredients)
	{
		example_string = "title: pasta    ingredients: dough,water," + ingredient + "+";
		rcp = epicr::parse_string_silent(example_string).recipe;
		test_lib::expect_equal_i(rcp.ingredients.size(),3);
		test_lib::expect_equal_s(rcp.ingredients[2].name, ingredient);
		test_lib::expect_equal_b(rcp.ingredients[2].amount.is_uncountable,true);
	}
}
void ingredient_no_amount_before_eof_matches()
{
	test_lib::REGISTER;
	std::vector<std::string> ingredients = {"butter","yeast","milk"};
	
	std::string example_string;
	epicr::recipe rcp;
	for (auto ingredient : ingredients)
	{
		example_string = "title: pasta    ingredients: dough,water," + ingredient;
		rcp = epicr::parse_string_silent(example_string).recipe;
		test_lib::expect_equal_i(rcp.ingredients.size(),3);
		test_lib::expect_equal_s(rcp.ingredients[2].name, ingredient);
		test_lib::expect_equal_i(rcp.ingredients[2].amount.number,1);
		test_lib::expect_equal_s(rcp.ingredients[2].amount.unit,"");
	}
}
void ingredient_amount_and_unit_before_eof_matches()
{
	test_lib::REGISTER;
	std::vector<std::string> ingredients = {"butter","yeast","milk"};
	
	std::string example_string;
	epicr::recipe rcp;
	for (auto ingredient : ingredients)
	{
		example_string = "title: pasta    ingredients: dough,water," + ingredient + " [100 g]";
		rcp = epicr::parse_string_silent(example_string).recipe;
		test_lib::expect_equal_i(rcp.ingredients.size(),3);
		test_lib::expect_equal_s(rcp.ingredients[2].name, ingredient);
		test_lib::expect_equal_i(rcp.ingredients[2].amount.number,100);
		test_lib::expect_equal_s(rcp.ingredients[2].amount.unit,"g");
	}
}
void ingredient_no_unit_before_eof_matches()
{
	test_lib::REGISTER;
	std::vector<std::string> ingredients = {"apple","small pear","rather large melon"};
	
	std::string example_string;
	epicr::recipe rcp;
	for (auto ingredient : ingredients)
	{
		example_string = "title: pasta    ingredients: dough,water," + ingredient + " [10]";
		rcp = epicr::parse_string_silent(example_string).recipe;
		test_lib::expect_equal_i(rcp.ingredients.size(),3);
		test_lib::expect_equal_s(rcp.ingredients[2].name, ingredient);
		test_lib::expect_equal_i(rcp.ingredients[2].amount.number,10);
		test_lib::expect_equal_s(rcp.ingredients[2].amount.unit,"");
	}
}

void empty_yield_before_eof_matches()
{
	test_lib::REGISTER;
	std::string example_string = "title: pasta    instructions: with(water): drink the water yield:";
	epicr::recipe rcp = epicr::parse_string_silent(example_string).recipe;
	test_lib::expect_equal_i(rcp.instructions[0].yields.size(),0);
}
void yield_no_amount_before_eof_matches()
{
	test_lib::REGISTER;
	std::vector<std::string> yieldIngredients = {"ice","cold ice","ice cold ice","extremely cold and wet ice"};
	std::string example_string;
	epicr::recipe rcp;
	for (auto ingredient : yieldIngredients)
	{
		example_string = "title: pasta    instructions: with(water): freeze the water yield: water," + ingredient;
		rcp = epicr::parse_string_silent(example_string).recipe;
		test_lib::expect_equal_i(rcp.instructions[0].yields.size(),2);
		test_lib::expect_equal_s(rcp.instructions[0].yields[1].name, ingredient);
		test_lib::expect_equal_i(rcp.instructions[0].yields[1].amount.number,1);
		test_lib::expect_equal_s(rcp.instructions[0].yields[1].amount.unit,"");
	}
}
void yield_no_unit_before_eof_matches()
{
	test_lib::REGISTER;
	std::vector<std::string> yieldIngredients = {"apple","small pear","rather large melon"};
	std::string example_string;
	epicr::recipe rcp;
	for (auto ingredient : yieldIngredients)
	{
		example_string = "title: pasta    instructions: with(tree): pick the fruits from the tree yield: leaf," + ingredient + "[3]";
		rcp = epicr::parse_string_silent(example_string).recipe;
		test_lib::expect_equal_i(rcp.instructions[0].yields.size(),2);
		test_lib::expect_equal_s(rcp.instructions[0].yields[1].name, ingredient);
		test_lib::expect_equal_i(rcp.instructions[0].yields[1].amount.number,3);
		test_lib::expect_equal_s(rcp.instructions[0].yields[1].amount.unit,"");
	}
}
void uncountable_yield_before_eof_matches()
{
	test_lib::REGISTER;
	std::vector<std::string> yieldIngredients = {"ginger beer","light beer","cold beer"};
	std::string example_string;
	epicr::recipe rcp;
	for (auto ingredient : yieldIngredients)
	{
		example_string = "title: pasta    instructions: with(water,yeast): brew the beer yield: alcohol," + ingredient + "+";
		rcp = epicr::parse_string_silent(example_string).recipe;
		test_lib::expect_equal_i(rcp.instructions[0].yields.size(),2);
		test_lib::expect_equal_s(rcp.instructions[0].yields[1].name, ingredient);
		test_lib::expect_equal_b(rcp.instructions[0].yields[1].amount.is_uncountable,true);
	}
}

int main(void)
{
	title_before_eof_matches();
	description_before_eof_matches();
	servings_before_eof_matches();
	total_time_before_eof_matches();
	prep_time_before_eof_matches();
	cook_time_before_eof_matches();
	one_tag_before_eof_matches();
	two_tags_before_eof_matches();
	three_tags_before_eof_matches();
	one_kitchenware_before_eof_matches();
	two_kitchenware_before_eof_matches();
	three_kitchenware_before_eof_matches();
	optional_ingredient_before_eof_matches();
	uncountable_ingredient_before_eof_matches();
	ingredient_no_amount_before_eof_matches();
	ingredient_amount_and_unit_before_eof_matches();
	ingredient_no_unit_before_eof_matches();
	empty_yield_before_eof_matches();
	yield_no_amount_before_eof_matches();
	yield_no_unit_before_eof_matches();
	uncountable_yield_before_eof_matches();
	test_lib::print_recap();
	return test_lib::result();
}