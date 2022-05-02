#define DEBUG 0
#include "epicr.h"

#define PRINT_VEC(vec)          \
	{                           \
		for (auto val : vec)    \
			cout << val << " "; \
	}

#define IF_ERR_RETURN(obj) \
	{                      \
		if (obj.)          \
	}

using namespace std;

/* Remove or outcomment when not debugging */
void print_lexer_tokens(epicr::Lexer lexer);

int main(int argc, char** argv)
{
	epicr::parse_cmd_args(argc, argv);

	ifstream file = epicr::open_file(epicr::clargs.input_filepath);

	cout << "Compiling " << epicr::clargs.input_filepath << endl;

	epicr::Lexer my_lexer = epicr::Lexer(file);

	epicr::Parser my_parser = epicr::Parser(&my_lexer);
	epicr::recipe my_recipe = my_parser.Parse();

	cout << my_parser.has_error << " Parser: " << my_parser.error << endl;

	auto ingrvisit = epicr::visitor::IngredientVerifier();
	ingrvisit.visit(&my_recipe);
	cout << ingrvisit.has_error << " IngVis: " << ingrvisit.error << endl;

	auto amntconve = epicr::visitor::AmountConverter();
	amntconve.visit(&my_recipe);
	cout << amntconve.has_error << " AmtCon: " << amntconve.error << endl;

	std::cout << "clarg output filepath: " << epicr::clargs.output_filepath << std::endl;

	if (!my_parser.has_error)
	{
		string output_file = epicr::clargs.output_filepath;
		output_file.append("/" + my_recipe.title + ".html");
		epicr::generate_html(my_recipe, output_file);
		printf("Wrote file: %s\n", output_file.c_str());
	}

	return 0;
}

void print_lexer_tokens(epicr::Lexer lexer)
{

	int token_count = 0;

	while (lexer.is_ready())
	{
		epicr::epicr_token token = lexer.next_token();
		epicr::epicr_token peek = lexer.peek_token();
		if (token_count > 70)
		{
			printf("%3i c:", token_count);
			epicr::print_token(token);
			printf("%3i p:", token_count + 1);
			epicr::print_token(peek);
			cout << endl;
		}

		token_count++;
	}
}