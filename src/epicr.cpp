#include "./epicr.h"

using namespace std;

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

	html_style parse_style(char *argv)
	{
		html_style choosen_style;
		if (*argv == '--basic' || *argv == '-b')
		{
			choosen_style = html_style_basic;
		}
		else if (*argv == '--fancy' || *argv == '-f')
		{
			choosen_style = html_style_fancy;
		}
		return choosen_style;
	}

	cmd_args parse_cmd_args(int argc, char **argv)
	{
		cmd_args CMD_ARGS;
		for (int i = 0; i < argc; i++)
		{
			if (argv[i] == "--basic" || argv[i] == "-b")
			{
				CMD_ARGS.choosen_style = parse_style(argv[i]);
			}
			else if (argv[i] == "--fancy" || argv[i] == "-f")
			{
				CMD_ARGS.choosen_style = parse_style(argv[i]);
			}
			else if (argv[i] == "-o ")
			{
				CMD_ARGS.output_filepath = argv[i + 1];
				i++;
			}
			else
			{
				CMD_ARGS.input_filepath = argv[i];
			}
		}
		return CMD_ARGS;
	}

	epicr::recipe parse_recipe(cmd_args clargs)
	{

		std::ifstream pasta = epicr::open_file(clargs.input_filepath);
		epicr::Lexer lexer(pasta);
		epicr::Parser parser(&lexer);
		parser.clargs = clargs;
		epicr::recipe rcp = parser.Parse();
		return rcp;
	}

}