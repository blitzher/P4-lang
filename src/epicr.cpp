#include "./epicr.h"

using namespace std;

namespace epicr
{

	ifstream open_file(string filename)
	{
		ifstream file{filename};

		if (!file.is_open())
			cout << "File " << filename << " could not be opened!" << endl;
		else
			cout << "Opened file " << filename << endl;

		return file;
	}

	string token_to_string(epicr_token_type type)
	{
		switch (type)
		{
		case ETT_WORD:
			return "ETT_WORD";
		case ETT_BRACKET_OPEN:
			return "ETT_BRACKET_OPEN";
		case ETT_BRACKET_CLOSE:
			return "ETT_BRACKET_CLOSE";
		case ETT_NUMBER:
			return "ETT_NUMBER";
		case ETT_EOF:
			return "ETT_EOF";
		default:
			return "ETT_UNKOWN";
		}
	}
}