#define DEBUG 1
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

	ifstream file = epicr::open_file(argv[1]);
	epicr::Lexer myLexer = epicr::Lexer(file);

	epicr::Parser myParser = epicr::Parser(&myLexer);

 	epicr::recipe myRecipe = myParser.Parse();
	
	cout << myParser.error << ": " << myParser.error_message << endl;

	//generate_html(myRecipe, "dist/recipe.html");

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
			//printf("%3i p:", token_count + 1);
			//epicr::print_token(peek);
			cout << endl;
		}

		token_count++;
	}
}