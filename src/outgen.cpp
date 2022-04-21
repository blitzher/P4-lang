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
	//elements are merely strings
	string insertBulletPoints(string header, std::vector<string> elements)
	{
		string result = "";
		size_t elementsCount = elements.size();
		if (elementsCount == 0)
			return result;
        header.insert(0, "<h3>");
        header.append("</h3>");
		result += header;
		for (size_t i = 0;i<elementsCount;i++)
		{
            elements[i].insert(0, "<li>");
            elements[i].append("</li>");
			result += elements[i];
		}
		return result;
	}

    string insertTime(string header, string time)
	{
		string result = "";
		if (time == "")
			return result;
        header.insert(0, "<div class=time-header>");
        header.append("</div>");
		result += header;
        time.insert(0, "<div class=time-content>");
        time.append("</div>");
		result += time;
		return result;
	}
	
	string insertAmount(amount amount)
	{
		string result = " (";
		result += epicr::double_to_string(amount.number);
		if(!(amount.unit == ""))
			result += " " + amount.unit;
		result += ")";
		return result;
	}
	
	//fields are structs
	string insertIngredients(string header, std::vector<ingredient> fields)
	{
		string result = "";
		size_t fieldCount = fields.size();
		if (fieldCount== 0)
			return result;
        header.insert(0, "<h3>");
        header.append("</h3>");
		result += header;
		for (size_t i = 0;i<fieldCount;i++)
		{
            result += "<li>";
			result += fields[i].name;
			if (!fields[i].amount.isUncountable)
				result += insertAmount(fields[i].amount);
            result += "</li>";
		}	
		return result;
	}

	string insertInstructionBody(std::vector<instruction_word> body)
	{
		string result = "";
		size_t bodyLength = body.size();
		for (size_t i = 0;i<bodyLength;i++)
		{
			result += body[i].spelling;
			if (body[i].is_amount == true)
			{
				result += epicr::double_to_string(body[i].value.number);
				result += " " + body[i].value.unit;
			}
		}	
		return result;
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
		const char *base_template = base_template_s.c_str();
		string step_template_s = load_template("step");
		const char *step_template = step_template_s.c_str();

		std::ofstream file{filename};
		if (!file.is_open())
			return false;
		string instructions_section = "";
		char* instr_s = (char*)malloc(MAX_S_LENGTH);
		int index = 0;
		for (auto inst : rcp.instructions)
		{
			index++;
			string ingredients = insertIngredients("Ingredients: ", inst.ingredients);
			string kitchenware = insertBulletPoints("Kitchenware: ",inst.kitchenware);
			string body = insertInstructionBody(inst.body);
			
			string step_text = "Step " + std::to_string(index);
			sprintf(instr_s, step_template, step_text.c_str(), ingredients.c_str(), kitchenware.c_str(), body.c_str());
			instructions_section += instr_s;
		}

        // format final HTML strings
		string tags = insertBulletPoints("Tags: ", rcp.tags);
		string kitchenware = insertBulletPoints("Kitchenware: ", rcp.kitchenware);
		string ingredients = insertIngredients("Ingredients: ", rcp.ingredients);
        string totalTime = insertTime("Total time: ", rcp.time.total_time.c_str());
        string prepTime = insertTime("Prep time: ", rcp.time.prep_time.c_str());
        string cookTime = insertTime("Cook time: ", rcp.time.cook_time.c_str());

        string output_string = base_template;  // convert base template to string
        
        // replace placeholders with final HTML
        replace(output_string, "~title~", rcp.title.c_str());
        replace(output_string, "~description~", rcp.description.c_str());
        replace(output_string, "~total-time~", totalTime);
        replace(output_string, "~prep-time~", prepTime);
        replace(output_string, "~cook-time~", cookTime);
        replace(output_string, "~tags~", tags.c_str());
        replace(output_string, "~ingredients~", ingredients.c_str());
        replace(output_string, "~kitchenware~", kitchenware.c_str());
        replace(output_string, "~instructions~", instructions_section.c_str());
        
        file << output_string << std::endl;
		file.close();

		return true;
	}
	
}