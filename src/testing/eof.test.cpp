#include "./test_lib.h"

void title_before_eof_matches()
{
	test_lib::REGISTER;
	std::vector<std::string> titles = { "carbonara","spaghetti carbonara","the spaghetti carbonara",
									   "a very good spaghetti carbonara recipe" };
	std::string example_string;
	epicr::recipe rcp;
	for (size_t i = 0;i < titles.size();i++)
	{
		example_string = "cook-time: 5 min    title: " + titles[i];
		rcp = epicr::parse_string_silent(example_string).recipe;
		test_lib::expect_equal_s(rcp.title, titles[i]);
	}
}

void description_before_eof_matches()
{
	test_lib::REGISTER;
	std::vector<std::string> descriptions = { "a recipe","this is a recipe","recipe is ...\nspaghetti carbonara (very good)",
									   "this is a very good spaghetti carbonara recipe","this is recipe." };
	std::string example_string;
	epicr::recipe rcp;
	for (size_t i = 0;i < descriptions.size();i++)
	{
		example_string = "cook-time: 10 min description: " + descriptions[i];
		rcp = epicr::parse_string_silent(example_string).recipe;
		test_lib::expect_equal_s(rcp.description, descriptions[i]);
	}
}
void servings_before_eof_matches()
{
	test_lib::REGISTER;
	std::map<std::string, epicr::servings> servings;
	servings.insert({ {"5"},{5,""} });
	servings.insert({ {"10 people"},{10,"people"} });
	servings.insert({ {"3 people guys"},{3,"people guys"} });
	servings.insert({ {"1 portions of magnificent dishes"},{1,"portions of magnificent dishes"} });
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

int main(void)
{
	title_before_eof_matches();
	description_before_eof_matches();
	servings_before_eof_matches();
	test_lib::print_recap();
	return test_lib::result();
}