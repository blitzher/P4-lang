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

int main(int argc, char **argv)
{
	epicr::parse_cmd_args(argc, argv);

	if (epicr::clargs.is_cookbook)
	{
		epicr::generate_cookbook(epicr::clargs.input_filepath, epicr::clargs.output_filepath);
	}

	ifstream file = epicr::open_file(epicr::clargs.input_filepath);
	cout << "Compiling " << epicr::clargs.input_filepath << endl;

	epicr::Lexer my_lexer = epicr::Lexer(file);

	epicr::Parser my_parser = epicr::Parser(&my_lexer);
	epicr::recipe my_recipe = my_parser.Parse();

	cout << my_parser.has_error << " Parser  : " << my_parser.error << endl;

	// creates temp file for gui to read and early return
	if (my_parser.has_error)
	{
		ofstream ParsererrorLog(".epicr-error.txt");
		ParsererrorLog << my_parser.error << endl;
		return 1;
	}

	epicr::rcp_ret vis_ret = epicr::visitor::visit_all(&my_recipe);
	cout << vis_ret.has_err << vis_ret.err_source << vis_ret.err << endl;

	// creates temp file for gui to read and early return
	if (vis_ret.has_err)
	{
		ofstream visitorErrorlog(".epicr-error.txt");
		visitorErrorlog << vis_ret.err << endl;
		return 1;
	}

	string output_file = epicr::clargs.output_filepath;
	string filename = (std::filesystem::path(epicr::clargs.input_filepath).stem()).string();

	epicr::epicr_html_style choosen_style = epicr::clargs.choosen_style;
	std::filesystem::path o_file = output_file / std::filesystem::path(filename);
	if (choosen_style == epicr::E_STYLE_FANCY)
	{
		output_file = o_file.string() + ".html";
		epicr::generate_fancy(my_recipe, output_file);
	}
	else if (choosen_style == epicr::E_STYLE_BASIC)
	{
		output_file = o_file.string() + ".md";
		epicr::generate_basic(my_recipe, output_file);
	}
	printf("Wrote file: %s\n", output_file.c_str());

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