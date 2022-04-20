#define DEBUG 0
#include "epicr.h"

#define PRINT_VEC(vec)          \
	{                           \
		for (auto val : vec)    \
			cout << val << " "; \
	}

using namespace std;

/* Remove or outcomment when not debugging */
void print_lexer_tokens(epicr::Lexer lexer);

int main(int argc, char **argv)
{
	cout << "Command line arguments:" << endl;

	for (int i = 0; i < argc; i++)
	{
		cout << i << ": " << argv[i] << endl;
	}
	cout << "-------\n";

	epicr::parse_cmd_args(argc, argv);

	ifstream file = epicr::open_file(argv[1]);

	cout << "Compiling " << epicr::clargs.input_filepath << endl;

	epicr::Lexer myLexer = epicr::Lexer(file);
	epicr::Parser myParser = epicr::Parser(&myLexer);
	epicr::recipe myRecipe = myParser.Parse();

	cout << myParser.error << " Parser: " << myParser.error_message << endl;

	auto ingrvisit = epicr::visitor::IngredientVerifier();
	ingrvisit.visit(myRecipe);
	cout << ingrvisit.has_error << " IngVis: " << ingrvisit.error << endl;

	auto amntconve = epicr::visitor::AmountConverter();
	amntconve.visit(&myRecipe);
	cout << amntconve.has_error << " AmtCon: " << amntconve.error << endl;

	std::cout << "clarg output filepath: " << epicr::clargs.output_filepath << std::endl;

	if (!myParser.error)
	{
		string output_file = epicr::clargs.output_filepath;
		output_file.append("/" + myRecipe.title + ".html");
		epicr::generate_html(myRecipe, output_file);
		printf("Wrote file: %s\n", output_file.c_str());
	}

	return 0;
}

void print_lexer_tokens(epicr::Lexer lexer)
{

	int token_count = 0;

	while (lexer.is_ready())
	{
		epicr::epicr_token token = lexer.next_non_blank_token();
		epicr::epicr_token peek = lexer.peek_non_blank_token(1);
		if (token_count < 100)
		{
			printf("%3i c:", token_count);
			epicr::print_token(token);
			// printf("%3i p:", token_count + 1);
			// epicr::print_token(peek);
			cout << endl;
		}

		token_count++;
	}
}