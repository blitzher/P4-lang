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
	// elements are merely strings
	string insertDelimiterSeperatedFields(string header, string delimiter, std::vector<string> elements)
	{
		string result = "";
		size_t elementsCount = elements.size();
		if (elementsCount == 0)
			return result;
		result += header;
		for (size_t i = 0; i < elementsCount; i++)
		{
			result += elements[i];

			if (i != elementsCount - 1)
				result += delimiter;
		}
		return result;
	}

	string insertAmount(amount amount)
	{
		string result = "(";
		result += epicr::double_to_string(amount.number);
		if (!(amount.unit == ""))
			result += " " + amount.unit;
		result += ")";
		return result;
	}

	// fields are structs
	string insertDelimiterSeperatedFieldsWithAmounts(string header, string delimiter, std::vector<ingredient> fields)
	{
		string result = "";
		size_t fieldCount = fields.size();
		if (fieldCount == 0)
			return result;
		result += header;
		for (size_t i = 0; i < fieldCount; i++)
		{
			result += fields[i].name;
			if (!fields[i].amount.isUncountable)
				result += insertAmount(fields[i].amount);
			if (i != fieldCount - 1)
				result += delimiter;
		}
		return result;
	}

	string insertTags(recipe rcp)
	{
		return insertDelimiterSeperatedFields("Tags: ", ", ", rcp.tags);
	}

	string insertKitchenware(recipe rcp)
	{
		return insertDelimiterSeperatedFields("Kitchenware: ", ", ", rcp.kitchenware);
	}

	string insertIngredients(recipe rcp)
	{
		return insertDelimiterSeperatedFieldsWithAmounts("Ingredients: ", ", ", rcp.ingredients);
	}

	string insertInstructionBody(std::vector<instruction_word> body)
	{
		string result = "";
		size_t bodyLength = body.size();
		for (size_t i = 0; i < bodyLength; i++)
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
		char *instr_s = (char *)malloc(MAX_S_LENGTH);
		char *base_s = (char *)malloc(MAX_B_LENGTH);
		int index = 0;
		for (auto inst : rcp.instructions)
		{
			index++;
			string ingredients = insertDelimiterSeperatedFieldsWithAmounts("Ingredients: ", ", ", inst.ingredients);
			string kitchenware = insertDelimiterSeperatedFields("Kitchenware: ", ", ", inst.kitchenware);
			string body = insertInstructionBody(inst.body);

			string step_text = "Step " + std::to_string(index);
			sprintf(instr_s, step_template, step_text.c_str(), ingredients.c_str(), kitchenware.c_str(), body.c_str());
			instructions_section += instr_s;
		}
		string tags = insertTags(rcp);
		string kitchenware = insertKitchenware(rcp);
		string ingredients = insertIngredients(rcp);

		sprintf(base_s, base_template,
				rcp.title.c_str(),
				rcp.description.c_str(),
				rcp.time.cook_time.c_str(),
				tags.c_str(),
				ingredients.c_str(),
				kitchenware.c_str(),
				instructions_section.c_str());
		file << base_s << std::endl;
		file.close();

		return true;
	}

}