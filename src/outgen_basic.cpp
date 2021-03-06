#include "epicr.h"

using std::string;

namespace epicr
{
	/* create markdown template file */
	string load_basic_template(string template_name)
	{
		string file_content;
		string dir = "src";
		string file_name = template_name + ".md";
		std::filesystem::path filePath = std::filesystem::path(dir) / "output-templates" / file_name;
		string fpath = filePath.string();
		std::ifstream fstream = epicr::open_file(fpath);

		int gotten_char = fstream.get();
		do
		{
			file_content += gotten_char;
			gotten_char = fstream.get();
		} while (!fstream.eof());
		return file_content;
	}

	/* constructs strings for time fields */
	string basic_insert_time(string header, string time)
	{
		if (time == "")
			return "";

		return header + time;
	}

	/* constructs strings for servings field */
	string basic_insert_servings(servings servings)
	{
		string number = std::to_string(servings.count);
		string descriptor = servings.descriptor;

		if (servings.count == 0)
		{
			number = "1";
			descriptor = "servings";
		}

		return "# Servings\n\t" + number + " " + descriptor;
	}

	/* constructs strings for ingredients listing */
	string basic_insert_declaration_ingredients(string header, std::vector<ingredient> ingredients)
	{
		string result;

		for (size_t i = 0; i < ingredients.size(); i++)
		{

			if (!ingredients[i].is_optional)
			{
				result += "\t" + ingredients[i].name;
				if (!ingredients[i].amount.is_uncountable)
				{
					result += " (" + epicr::round_double_to_string(ingredients[i].amount.number) + " " + ingredients[i].amount.unit + ")";
					if (ingredients[i].is_ingredient_ref)
					{
						result += " [Reference recipe: " + ingredients[i].name + ".md]";
					}
				}
				result += "\n";
			}
		}
		return header + "\n" + result;
	}
	/* constructs strings for optional ingredients listing */
	string basic_insert_optional_ingredients(string header, std::vector<ingredient> ingredients)
	{
		string result;
		for (size_t i = 0; i < ingredients.size(); i++)
		{
			if (ingredients[i].is_optional)
			{
				result += "\t" + ingredients[i].name;
				if (!ingredients[i].amount.is_uncountable)
				{
					result += " (" + epicr::round_double_to_string(ingredients[i].amount.number) + " " + ingredients[i].amount.unit + ")";
				}

				if (ingredients[i].is_ingredient_ref)
				{
					result += " [Reference recipe: " + ingredients[i].name + ".md]";
				}

				result += "\n";
			}
		}
		return header + "\n" + result;
	}

	/* constructs strings for a list -> used for tags and kitchenware */
	string basic_insert_text_in_list(string header, std::vector<string> listElements)
	{
		if (listElements.size() == 0)
			return "";
		string result;

		if (header == "# Tags\n")
		{
			result += "\t";
			for (size_t i = 0; i < listElements.size(); i++)
			{
				result += listElements[i] + ", ";
			}
			result.erase(result.size() - 2);
			return header + result + "\n";
		}
		else
		{ // kitchenware
			for (size_t i = 0; i < listElements.size(); i++)
			{
				result += "\t" + listElements[i] + "\n";
			}
			return header + '\n' + result;
		}
	}

	/* constructs strings for nutrients listing */
	string basic_insert_nutrients(std::vector<ingredient> nutrients)
	{
		string result;
		if (nutrients.size() == 0)
		{
			return "";
		}
		for (size_t i = 0; i < nutrients.size(); i++)
		{
			std::string number = epicr::round_double_to_string(nutrients[i].amount.number);
			std::string unit = nutrients[i].amount.unit;
			result += "\t" + nutrients[i].name + " (" + number + " " + unit + ")\n";
		}
		result += "*pr. 100 grams*";
		return "# Nutrients\n" + result;
	}

	/* constructs strings for ingredient field in instructions */
	string basic_insert_instruction_ingredients(string header, std::vector<ingredient> ingredients)
	{
		string result;
		for (size_t i = 0; i < ingredients.size(); i++)
		{
			if (i != 0)
				result += ", ";
			result += ingredients[i].name + " ";
			if (!ingredients[i].amount.is_uncountable)
				result += "(" + epicr::round_double_to_string(ingredients[i].amount.number) + " " + ingredients[i].amount.unit + ")";
		}

		return header + " " + result + "\n";
	}

	/* constructs strings for kitchenware field in instructions */
	string basic_insert_instruction_kitchenware(string header, std::vector<string> kitchenware)
	{
		string result;
		if (kitchenware.size() == 0)
			return "";

		for (size_t i = 0; i < kitchenware.size(); i++)
		{
			result += kitchenware[i];
			if (i != kitchenware.size() - 1)
				result += ", ";
		}

		return header + " " + result + "\n";
	}

	/* constructs strings for instructions body */
	string basic_insert_instruction_body(std::vector<instruction_word> body)
	{
		string result = "\t";
		for (size_t i = 0; i < body.size(); i++)
		{
			result += body[i].spelling;
			if (body[i].is_amount == true)
			{
				result += epicr::round_double_to_string(body[i].value.number);
				result += body[i].value.unit;
			}
			else if (body[i].spelling[0] == '\n')
				result += "\t";
		}
		return strip_spaces_right(result) + "\n"; // readability ig
	}

	/* constructs strings for yield field in instructions */
	string basic_insert_yield_ingredients(string header, std::vector<ingredient> ingredients)
	{
		string result;
		for (size_t i = 0; i < ingredients.size(); i++)
		{
			result += "\t\t" + ingredients[i].name + " ";
			if (!ingredients[i].amount.is_uncountable)
				result += epicr::round_double_to_string(ingredients[i].amount.number) + ingredients[i].amount.unit;
			if (i + 1 != ingredients.size())
				result += ", ";
			result += "\n";
		}
		return "\t" + header + "\n" + result;
	}

	/* generate strings for output and replace placeholders */
	bool generate_basic(recipe rcp, string filename)
	{
		string base_template_basic_s = load_basic_template("base");
		const char *base_template_basic = base_template_basic_s.c_str();
		string step_template_basic_s = load_basic_template("step");
		const char *step_template_basic = step_template_basic_s.c_str();

		std::filesystem::create_directory("dist");

		std::ofstream file{filename};
		if (!file.is_open())
			return false;
		int index = 0;
		string instruction_strings = "\n# Instructions\n";

		for (auto inst : rcp.instructions)
		{
			index++;
			string instruction_string = step_template_basic;
			string step_text = "\n## Step " + std::to_string(index);
			string instructionIngredients = "";
			string instructionKitchenware = "";
			string body = "";
			string yield = "";

			if (inst.ingredients.size() > 0)
				instructionIngredients = basic_insert_instruction_ingredients("\tIngredients:", inst.ingredients);

			if (inst.kitchenware.size() == 0)
				instructionIngredients += "";
			else
				instructionKitchenware += basic_insert_instruction_kitchenware("\tKitchenware:", inst.kitchenware);

			if (inst.body.size() > 0)
				body += basic_insert_instruction_body(inst.body);

			size_t length_of_longest = 0;
			size_t line_length = 0;
			for (const auto &word : inst.body)
			{
				if (word.spelling[0] == '\n')
				{
					if (line_length > length_of_longest)
						length_of_longest = line_length;
					line_length = 0;
				}
				else
					line_length += word.spelling.size();
			}

			if (inst.yields.size() > 0)
			{
				yield += basic_insert_yield_ingredients("yield: ", inst.yields);
			}

			/* replace step placeholders with final strings */
			epicr::replace(instruction_string, "~stepText~", step_text);
			epicr::replace(instruction_string, "~instructionIngredients~", instructionIngredients);
			epicr::replace(instruction_string, "~instructionKitchenware~", instructionKitchenware);
			epicr::replace(instruction_string, "~instructionBody~", body);
			epicr::replace(instruction_string, "~instructionYield~", yield);
			instruction_strings += instruction_string;
		}

		/* format final strings for .txt*/
		string title = "# Title\n\t";
		title += rcp.title.c_str();
		string description = "# Description\n";
		description += rcp.description;
		for (size_t i = 0; i < description.size(); i++)
			if (description[i] == '\n')
				description.insert(++i, "\t");
		string servings = basic_insert_servings(rcp.servings);
		string tags = basic_insert_text_in_list("# Tags", rcp.tags);
		string kitchenware = basic_insert_text_in_list("# Kitchenware", rcp.kitchenware);
		string ingredients = basic_insert_declaration_ingredients("# Ingredients", rcp.ingredients);
		string optional_ingredients = basic_insert_optional_ingredients("# Optional Ingredients ", rcp.ingredients);
		string nutrients = basic_insert_nutrients(rcp.nutrients);
		string total_time = basic_insert_time("# Total time\n\t", rcp.time.total_time.c_str());
		string prep_time = basic_insert_time("# Prep time\n\t", rcp.time.prep_time.c_str());
		string cook_time = basic_insert_time("# Cook time\n\t", rcp.time.cook_time.c_str());

		string output_string = base_template_basic; // convert base template to string

		/* replace placeholders with final strings */
		epicr::replace(output_string, "~title~", title);
		epicr::replace(output_string, "~servings~", servings);
		epicr::replace(output_string, "~description~", description);
		epicr::replace(output_string, "~total-time~", total_time);
		epicr::replace(output_string, "~prep-time~", prep_time);
		epicr::replace(output_string, "~cook-time~", cook_time);
		epicr::replace(output_string, "~tags~", tags.c_str());
		epicr::replace(output_string, "~ingredients~", ingredients.c_str());
		epicr::replace(output_string, "~optionalIngredients~", optional_ingredients.c_str());
		epicr::replace(output_string, "~kitchenware~", kitchenware.c_str());
		epicr::replace(output_string, "~nutrients~", nutrients.c_str());
		epicr::replace(output_string, "~instructions~", instruction_strings.c_str());

		file << output_string << std::endl;
		file.close();

		return true;
	}
}