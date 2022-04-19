#include "./epicr.h"

using namespace std;

std::unordered_map<std::string, epicr::parse_ret> cached_recipes;

namespace epicr
{
	ifstream open_file(string filename)
	{
		ifstream file{filename, ios_base::binary};

		if (!file.is_open())
			cout << "File " << filename << " could not be opened!" << endl;

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

	/* taken from: https://www.techiedelight.com/trim-string-cpp-remove-leading-trailing-spaces/ */
	std::string strip_spaces_right(std::string str)
	{
		std::string whitespace = " \n\r\t\f\v";
		size_t end = str.find_last_not_of(whitespace);
		return (end == std::string::npos) ? "" : str.substr(0, end + 1);
	}

	epicr::parse_ret parse_recipe(std::string filename)
	{
		cmd_args args = {filename, html_style_basic, "dist"};
		return parse_recipe(args);
	}

	epicr::parse_ret parse_recipe(cmd_args clargs)
	{
		if (cached_recipes.find(clargs.input_filepath) != cached_recipes.end())
			return cached_recipes[clargs.input_filepath];

		std::ifstream input_filestream = epicr::open_file(clargs.input_filepath);
		epicr::Lexer lexer(input_filestream);
		epicr::Parser parser(&lexer);
		epicr::recipe rcp = parser.Parse();

		epicr::parse_ret ret = {rcp, parser.error, parser.error_message};

		cached_recipes[clargs.input_filepath] = ret;

		return ret;
	}

	epicr::parse_ret parse_recipe_silent(std::string filename)
	{
		if (cached_recipes.find(filename) != cached_recipes.end())
			return cached_recipes[filename];

		std::ifstream input_filestream = epicr::open_file(filename);
		epicr::Lexer lexer(input_filestream);
		epicr::Parser parser(&lexer);
		parser.silence(true);
		epicr::recipe rcp = parser.Parse();

		epicr::parse_ret ret = {rcp, parser.error, parser.error_message};
		return ret;
	}
}