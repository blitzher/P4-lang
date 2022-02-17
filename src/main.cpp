#include "epicr.h"

using namespace std;

int main(int argc, char **argv)
{

	cout << "Command line arguments:" << endl;

	for (int i = 0; i < argc; i++)
	{
		cout << i << ": " << argv[i] << endl;
	}
	ifstream file{argv[1]};
	epicr::Lexer myLexer = epicr::Lexer(file);
	cout << "Hello!" << std::endl;

	return 0;
}