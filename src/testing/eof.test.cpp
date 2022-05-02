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
		std::cout<<example_string<<"\n";
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

int main(void)
{
	title_before_eof_matches();
	description_before_eof_matches();
	servings_before_eof_matches();
	total_time_before_eof_matches();
	prep_time_before_eof_matches();
	cook_time_before_eof_matches();
	test_lib::print_recap();
	return test_lib::result();
}