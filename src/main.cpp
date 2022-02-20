#include "epicr.h"

using namespace std;

int main(int argc, char **argv)
{

	cout << "Command line arguments:" << endl;

	for (int i = 0; i < argc; i++)
	{
		cout << i << ": " << argv[i] << endl;
	}
	ifstream file = epicr::open_file(argv[1]);
	if (file.is_open())
	{
		epicr::Lexer myLexer = epicr::Lexer(file);

		int token_count = 0;
		while (myLexer.is_ready())
		{
			string token = myLexer.next_token();

			cout << token_count << ": " << token << endl;

			if (token_count == 334)
			{
				for (size_t i = 0; i < token.size(); i++)
				{
					printf("%i", token[i]);
				}
				cout << endl;
			}

			token_count++;
		}
	}

	return 0;
}