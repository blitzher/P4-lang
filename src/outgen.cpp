#include "epicr.h"
#include <filesystem>

using std::string;

#define MAX_S_LENGTH 4096
#define MAX_B_LENGTH 65536 /* 4096 * 16 */
namespace epicr
{
	string load_template(string template_name)
	{
		string file_content;
		string dir = "src";
		string file_name = template_name + ".html";
		string fpath = dir + "/output-templates/" + file_name;
		std::ifstream fstream = epicr::open_file(fpath);

		int gotten_char = fstream.get();
		do
		{
			file_content += gotten_char;
			gotten_char = fstream.get();
		} while (!fstream.eof());

		return file_content;
	}

    // find a string in another string and replace with a third string
    bool replace(string& str, const string& from_string, const string& to_string) {
        size_t start_position = str.find(from_string);   
        if(start_position == string::npos)  // if no matching string was found
            return false;
        str.replace(start_position, from_string.length(), to_string);
        return true;
    }

	bool generate_html(recipe rcp, string filename)
	{
		string base_template_s = load_template("base");
		const char* base_template = base_template_s.c_str();
		string step_template_s = load_template("step");
		const char* step_template = step_template_s.c_str();

		std::ofstream file{ filename };
		if (!file.is_open())
			return false;
		string instructions_section = "";
		char* instr_s = (char*)malloc(MAX_S_LENGTH);
		int index = 0;
		for (auto inst : rcp.instructions)
		{
			index++;
			string ingredients = "";
			for (auto ingr : inst.ingredients)
				ingredients += ingr.name + " ";

			string body = "";
			for (auto word : inst.body)
			{
				body += word.spelling;
				if (word.is_amount == true)
				{
					body += epicr::double_to_string(word.value.number);
					body += " " + word.value.unit;
				}
			}
			string step_text = "Step " + std::to_string(index);
			sprintf(instr_s, step_template, step_text.c_str(), ingredients.c_str(), body.c_str());
			instructions_section += instr_s;
		}

		string tags = "";
		size_t tags_size = rcp.tags.size();
		if (tags_size != 0)
			tags += "Tags: ";
		for (size_t i = 0;i<tags_size;i++)
		{
			tags += rcp.tags[i];
			if (i != tags_size - 1)
				tags += ", ";
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

        string output_string = base_template;  // convert base template to string
        
        // replace placeholders with final HTML
        replace(output_string, "~title~", rcp.title.c_str());
        replace(output_string, "~description~", rcp.description.c_str());
        replace(output_string, "~cook-time~", rcp.time.cook_time.c_str());
        replace(output_string, "~tags~", tags.c_str());
        replace(output_string, "~ingredients~", ingredients.c_str());
        replace(output_string, "~kitchenware~", kitchenware.c_str());
        replace(output_string, "~instructions~", instructions_section.c_str());
        
        file << output_string << std::endl;
		file.close();

		return true;
	}
}