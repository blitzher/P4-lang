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
		case ETT_COLON:
			return "ETT_COLON";
		case ETT_COMMA:
			return "ETT_COMMA";
		case ETT_PARENS_OPEN:
			return "ETT_PARENS_OPEN"; /* () */
		case ETT_PARENS_CLOSE:
			return "ETT_PARENS_CLOSE";
		case ETT_BRACKET_OPEN:
			return "ETT_BRACKET_OPEN"; /* [] */
		case ETT_BRACKET_CLOSE:
			return "ETT_BRACKET_CLOSE";
		case ETT_CURLY_OPEN:
			return "ETT_CURLY_OPEN"; /* {} */
		case ETT_CURLY_CLOSE:
			return "ETT_CURLY_CLOSE";
		case ETT_PLUS:
			return "ETT_PLUS"; /* ingredients  + */
		case ETT_ASTERIX:
			return "ETT_ASTERIX"; /* ingredients  * */
		case ETT_QUESTION_MARK:
			return "ETT_QUESTION_MARK"; /* ingredients ? */
		case ETT_NUMBER:
			return "ETT_NUMBER";
		case ETT_BLANK:
			return "ETT_BLANK";
		case ETT_NEWLINE:
			return "ETT_NEWLINE";
		case ETT_EOF:
			return "ETT_EOF";
		default:
			return "ETT_UNKOWN";
		}
	}

	void print_token(epicr_token token)
	{
		string type = token_to_string(token.type);

		if (token.type != epicr::ETT_BLANK && token.type != epicr::ETT_NEWLINE)
			printf("%-18s-> %-10s uid:%i line:%i\n", type.c_str(), token.word.c_str(), token.uid, token.line);
		else
			printf("%-18s   %-10i uid:%i line:%i\n", type.c_str(), (int)token.word.size(), token.uid, token.line);
	}

	/* Rented from https://stackoverflow.com/questions/313970/how-to-convert-an-instance-of-stdstring-to-lower-case */
	char char_to_lower(char ch)
	{
		if (ch <= 'Z' && ch >= 'A')
			return ch - ('Z' - 'z');
		return ch;
	}

	string to_lower(std::string str)
	{
		string lowered;
		size_t str_size = str.size();
		for (size_t i = 0; i < str_size; i++)
			lowered.push_back(char_to_lower(str[i]));

		return lowered;
	}

}