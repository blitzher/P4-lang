#include "epicr.h"
#include <filesystem>

using std::string;
namespace fs = std::filesystem;

#define MAX_S_LENGTH 4096
#define MAX_B_Lenght 65536 /* 4096 * 16 */
namespace epicr
{

	string load_template(string template_name)
	{
		string file_content;
		string dir = "src";
		string file_name = template_name + ".html";
		fs::path fpath = dir;
		fpath /= "output-templates";
		fpath /= file_name;
		std::ifstream fstream;
		fstream.open(fpath, std::istream::in);

		int gotten_char = fstream.get();
		do
		{
			file_content += gotten_char;
			gotten_char = fstream.get();
		} while (!fstream.eof());

		return file_content;
	}

	bool generate_html(recipe rcp, string filename)
	{

		string base_template_s = load_template("base");
		const char *base_template = base_template_s.c_str();
		string step_template_s = load_template("step");
		const char *step_template = step_template_s.c_str();

		std::ofstream file{filename};
		if (!file.is_open())
			return false;

		for (auto instr : rcp.instructions)
		{
			std::cout << "instr found!" << std::endl;
		}

		string instructions_section = "";
		char *instr_s = (char *)malloc(MAX_S_LENGTH);
		char *base_s = (char *)malloc(MAX_B_Lenght);
		int indx = 1;
		for (auto inst : rcp.instructions)
		{
			string ingredients = "";
			for (auto ingr : inst.ingredients)
				ingredients += ingr.name + " ";

			string body = "";
			for (auto word : inst.body)
				body += word.word + " ";

			string step_text = "Step " + std::to_string(indx);
			sprintf(instr_s, step_template, step_text.c_str(), ingredients.c_str(), body.c_str());
			instructions_section += instr_s;
		}

		string tags = "";
		for (auto tag : rcp.tags)
		{
			tags += tag + " ";
		}

		string ingredients = "";
		for (auto ingredient : rcp.ingredients)
		{
			ingredients += ingredient.name + " ";
		}

		string kitchenware = "";
		for (auto ware : rcp.kitchenware)
		{
			kitchenware += ware + " ";
		}

		sprintf(base_s, base_template,
				rcp.title.c_str(),
				rcp.description.c_str(),
				rcp.time.c_str(),
				tags.c_str(),
				ingredients.c_str(),
				kitchenware.c_str(),
				instructions_section.c_str());
		file << base_s << std::endl;
		file.close();

		return true;
	}
}