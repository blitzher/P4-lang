#include "./epicr.h"
#include <cmath>

std::unordered_map<std::string, epicr::parse_ret> cached_recipes;
namespace fs = std::filesystem;

namespace epicr
{

	cmd_args clargs;
	std::vector<std::string> included_recipes;
	std::string recurison_error;
	bool has_recurison_error = false;

	std::ifstream open_file(std::string filename)
	{
		std::string fpath = std::filesystem::absolute(filename).string();
		std::ifstream file;

		if ((std::find(included_recipes.begin(), included_recipes.end(), fpath) != included_recipes.end()))
		{
			char* err = (char*)malloc(200);
			sprintf(err, "File %s was already included (recursion)", filename.c_str());
			recurison_error = err;
			has_recurison_error = true;
			return file;
		}
		file.open(filename, std::ios_base::binary);
		if (string_ends_with(filename, ".rcp"))
			included_recipes.push_back(fpath);

		if (!file.is_open())
			std::cout << "File " << filename << " could not be opened!" << std::endl;

		return file;
	}

	std::string token_to_string(epicr_token_type type)
	{
		switch (type)
		{
		case E_TT_WORD:
			return "E_TT_WORD";
		case E_TT_COLON:
			return "E_TT_COLON";
		case E_TT_COMMA:
			return "E_TT_COMMA";
		case E_TT_PARENS_OPEN:
			return "E_TT_PARENS_OPEN"; /* () */
		case E_TT_PARENS_CLOSE:
			return "E_TT_PARENS_CLOSE";
		case E_TT_BRACKET_OPEN:
			return "E_TT_BRACKET_OPEN"; /* [] */
		case E_TT_BRACKET_CLOSE:
			return "E_TT_BRACKET_CLOSE";
		case E_TT_PLUS:
			return "E_TT_PLUS"; /* ingredients  + */
		case E_TT_ASTERIX:
			return "E_TT_ASTERIX"; /* ingredients  * */
		case E_TT_QUESTION_MARK:
			return "E_TT_QUESTION_MARK"; /* ingredients ? */
		case E_TT_NUMBER:
			return "E_TT_NUMBER";
		case E_TT_BLANK:
			return "E_TT_BLANK";
		case E_TT_NEWLINE:
			return "E_TT_NEWLINE";
		case E_TT_EOF:
			return "E_TT_EOF";
		default:
			return "E_TT_UNKOWN";
		}
	}

	void print_token(epicr_token token)
	{
		std::string type = token_to_string(token.type);

		if (token.type != E_TT_BLANK && token.type != E_TT_NEWLINE)
			printf("%-18s-> %-6s (%lu) uid:%i line:%i\n", type.c_str(), token.word.c_str(), token.word.size(), token.uid, token.line);
		else
			printf("%-18s   %-10i uid:%i line:%i\n", type.c_str(), (int)token.word.size(), token.uid, token.line);
	}

	// finds a string in another string and replace it with a third string
	bool replace(std::string &str, const std::string &from_string, const std::string &to_string)
	{
		size_t start_position = str.find(from_string);
		if (start_position == std::string::npos) // if no matching string was found
			return false;
		str.replace(start_position, from_string.length(), to_string);
		return true;
	}

	/* Rented from https://stackoverflow.com/questions/313970/how-to-convert-an-instance-of-stdstring-to-lower-case */
	char char_to_lower(char ch)
	{
		if (ch <= 'Z' && ch >= 'A')
			return ch - ('Z' - 'z');
		return ch;
	}

	std::string to_lower(std::string str)
	{
		std::string lowered;
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

	std::string round_double_to_string(double num)
	{
		num = floor((num + 0.005) * 100.0) / 100.0; /*rounds to the nearest 2 decimals*/
		std::string str = std::to_string(num);
		int seperatorPosition = str.find('.');
		if (seperatorPosition == -1)
			return str;
		/* if the 2 first decimals are 0, the number is seen as a whole number */
		if (!(str[seperatorPosition + 1] == '0' && str[seperatorPosition + 2] == '0'))
			return str.substr(0, seperatorPosition + 3); // if it is a decimal we take the first 2 decimals
		return str.substr(0, seperatorPosition);
	}

	std::string amount_to_string(amount amount)
	{
		std::string result = "(";
		result += epicr::round_double_to_string(amount.number);
		if (!(amount.unit == ""))
			result += " " + amount.unit;
		result += ")";
		return result;
	}

	std::string concat_output_dir(std::string dest)
	{
		fs::path p = fs::current_path();
		fs::path output_dir = p;
		fs::path destination = (dest);
		fs::path full_path = output_dir / destination;
		return full_path.string();
	}

	bool ingredient_in_map(
		std::string ingredientName,
		std::unordered_map<std::string, ingredient> ingredients)
	{
		for (const auto& pair : ingredients)
		{
			if (pair.first == ingredientName)
				return true;
		}
		return false;
	}

	bool string_ends_with(std::string const& value, std::string const& ending)
	{
		if (ending.size() > value.size())
			return false;
		return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
	}

	parse_ret parse_recipe(std::string filename)
	{
		cmd_args args = { filename, E_OS_BASIC, "dist", E_US_NONE, false };
		return parse_recipe(args);
	}

	parse_ret parse_recipe_silent(std::string filename)
	{
		cmd_args args = { filename, E_OS_BASIC, "dist", E_US_NONE, true };
		return parse_recipe_silent(args);
	}

	parse_ret parse_recipe(cmd_args clargs)
	{
		if (cached_recipes.find(clargs.input_filepath) != cached_recipes.end())
			return cached_recipes[clargs.input_filepath];

		std::ifstream input_filestream = open_file(clargs.input_filepath);
		if (has_recurison_error)
			return { {}, true, recurison_error };
		Lexer lexer(input_filestream);
		Parser parser(&lexer);
		recipe rcp = parser.Parse();

		parse_ret ret = { rcp, parser.has_error, parser.error };

		cached_recipes[clargs.input_filepath] = ret;

		return ret;
	}

	parse_ret parse_recipe_silent(cmd_args clargs)
	{
		if (epicr::clargs.input_filepath == "")
			epicr::clargs.input_filepath = clargs.input_filepath;
		if (cached_recipes.find(clargs.input_filepath) != cached_recipes.end())
			return cached_recipes[clargs.input_filepath];

		std::ifstream input_filestream = open_file(clargs.input_filepath);
		if (has_recurison_error)
			return { {}, true, recurison_error };
		Lexer lexer(input_filestream);
		Parser parser(&lexer);
		parser.silence(true);
		recipe rcp = parser.Parse();

		parse_ret ret = { rcp, parser.has_error, parser.error };
		return ret;
	}

	parse_ret parse_string(std::string recipeExcerpt)
	{
		std::istringstream test_string(recipeExcerpt);
		Lexer lexer(test_string);
		Parser parser(&lexer);
		recipe rcp = parser.Parse();

		parse_ret ret = { rcp, parser.has_error, parser.error };
		return ret;
	}

	parse_ret parse_string_silent(std::string recipeExcerpt)
	{
		std::istringstream test_string(recipeExcerpt);
		Lexer lexer(test_string);
		Parser parser(&lexer);
		parser.silence(true);
		recipe rcp = parser.Parse();

		parse_ret ret = { rcp, parser.has_error, parser.error };
		return ret;
	}

	parse_ret parse_string_silent(std::string recipeExcerpt, Parser* o_parser)
	{
		std::istringstream test_string(recipeExcerpt);
		Lexer lexer(test_string);
		Parser _parser(&lexer);
		_parser.silence(true);
		*o_parser = _parser;
		recipe rcp = o_parser->Parse();

		parse_ret ret = { rcp, o_parser->has_error, o_parser->error };


		return ret;
	}

	rcp_ret ingredient_verify_recipe(recipe* recipe)
	{
		auto ingrvisit = epicr::visitor::IngredientVerifier();
		ingrvisit.visit(recipe);
		rcp_ret ret = { recipe, ingrvisit.has_error, ingrvisit.error };
		return ret;
	}

	epicr::epicr_html_style parse_style(std::string argv)
	{
		epicr::epicr_html_style choosen_style = epicr::E_OS_BASIC;
		if (argv == "--basic" || argv == "-b")
		{
			choosen_style = epicr::E_OS_BASIC;
		}
		else if (argv == "--fancy" || argv == "-f")
		{
			choosen_style = epicr::E_OS_FANCY;
		}
		return choosen_style;
	}
	epicr_unit_system parse_unit_system(std::string argv)
	{
		epicr_unit_system choosen_system = epicr::E_US_NONE;
		if (argv == "--imperial" || argv == "-i")
		{
			choosen_system = epicr::E_US_IMPERIAL;
		}
		else if (argv == "--metric" || argv == "-m")
		{
			choosen_system = epicr::E_US_METRIC;
		}
		return choosen_system;
	}

	void parse_cmd_args(int argc, char** argv)
	{
		std::vector<std::string> argv_s;
		for (int i = 0; i < argc; i++)
		{
			argv_s.push_back(std::string(argv[i]));
		}

		epicr::cmd_args CMD_ARGS = { "", E_OS_BASIC, concat_output_dir("dist"), E_US_NONE, false };
		for (int i = 0; i < argc; i++)
		{
			std::string arg = to_lower(argv_s[i]);
			if (arg == "--basic" || arg == "-b")
			{
				CMD_ARGS.choosen_style = parse_style(arg);
			}
			else if (arg == "--fancy" || arg == "-f")
			{
				CMD_ARGS.choosen_style = parse_style(arg);
			}
			else if (arg == "--imperial" || arg == "-i")
			{
				CMD_ARGS.unit_system = parse_unit_system(arg);
			}
			else if (arg == "--metric" || arg == "-m")
			{
				CMD_ARGS.unit_system = parse_unit_system(arg);
			}
			else if (arg == "--none" || arg == "-n")
			{
				CMD_ARGS.unit_system = parse_unit_system(arg);
			}
			else if (arg == "-o")
			{
				CMD_ARGS.output_filepath = argv_s[i + 1];
				i++;
			}
			else
			{
				CMD_ARGS.input_filepath = argv_s[i];
			}
		}

		clargs = CMD_ARGS;
	}
}
