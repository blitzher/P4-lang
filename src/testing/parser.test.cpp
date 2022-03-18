#include "./test_lib.h"

void parsed_title()
{
	test_lib::REGISTER;

	/* should split them up into 'hello' and a bunch of single length tokens */
	std::ifstream pasta = epicr::open_file("src/testing/Pasta.rcp");
	epicr::Lexer lexer(pasta);
	epicr::Parser parser(&lexer);
	epicr::recipe rcp = parser.Parse();
	test_lib::expect_equal_s(rcp.title, "Pasta\n");
}

int main(void)
{
	parsed_title();
	test_lib::print_recap();
	return test_lib::was_success();
}
