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
	string insertStringFields(string header, std::vector<string> elements,bool isInInstruction=false, string type="")
	{
		string result = "";
		size_t elementsCount = elements.size();
		if (elementsCount == 0)
			return result;
		if (!isInInstruction)
			result += "<h3>";
        result.insert(0, "<strong>");
		result += header;
        result.append("</strong>");
		if (!isInInstruction)
			result += "</h3>";
		for (size_t i = 0; i < elementsCount; i++)
		{
			if (!isInInstruction)
				result += "<li>";
			result += elements[i];
			if (!isInInstruction)
				result += "</li>";
			else
			{
				if (i != elementsCount - 1)
					result += ", ";
			}
		}

        if(type == "input"){
            result.append("<hr class='body-rule'>");
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

	string insertServings(servings servings)
	{
		string result = "";
		if (servings.count == 0)
			return result;
        result.insert(0, "<input type='number' class='servings' max='1000' min='1' oninput='updateNumbers()' onfocusout='updateNumbers(this)' value=");
        result.append(std::to_string(servings.count));
        result.append(">");
		result += " " + servings.descriptor;
		return result;
	}

	string insertAmount(amount amount)
	{
		string result = " (";
		result += epicr::double_to_string(amount.number);
		if (!(amount.unit == ""))
			result += " " + amount.unit;
		result += ")";
		return result;
	}

	/*returns whether or not there are any optional ingredients or non-optioanl ingredients, respectively based on bool parameter*/
	bool isAnyIngredients(bool ingredientsAreOptional,std::vector<ingredient> ingredients)
	{
		for (const ingredient &ingredient : ingredients)
		{
			if (ingredientsAreOptional == ingredient.isOptional)
				return true;
		}
		return false;
	}

	// fields are structs
	string insertIngredientFields(string header, std::vector<ingredient> fields,bool isInInstruction=false,bool fieldIsOptional=false, string type ="")
	{
		string result = "";
		size_t fieldCount = fields.size();
		if (!isAnyIngredients(fieldIsOptional,fields))
			return result;
		if (!isInInstruction)
			result += "<h3>";
        result.insert(0, "<strong>");
		result += header;
        result.append("</strong>");
		if (!isInInstruction)
			result += "</h3>";
		for (size_t i = 0; i < fieldCount; i++)
		{
			/*if the ingredient is non-optional and it is an optional field, or the ingredient is optional and it is a non-optional field */
			if ((!fields[i].isOptional && fieldIsOptional) || (fields[i].isOptional && !fieldIsOptional))
				continue;
			if (!isInInstruction)
			{
				result += "<li>";
			}
			result += fields[i].name + " ";
			if (!fields[i].amount.isUncountable){
                std::string number = epicr::double_to_string(fields[i].amount.number);
                std::string unit = fields[i].amount.unit;
                result.append("(");
                result.append("<text class='number'>");
                result.append(number.c_str());
                result.append("</text>");
                if (fields[i].amount.unit != ""){
                    result.append(" ").append(unit);
                }
                result.append(")"); 
            }
			if (!isInInstruction)
			{
				result += "</li>";
			}
			else
			{
				if (i != fieldCount - 1)
					result += ", ";
			}
		}

        if(type == "input"){
            result.append("<hr class='body-rule'>");
        }
        else if (type == "yield"){
            result.insert(0, "<hr class='body-rule'>");
        }

		return result;
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

	// find a string in another string and replace with a third string
	bool replace(string &str, const string &from_string, const string &to_string)
	{
		size_t start_position = str.find(from_string);
		if (start_position == string::npos) // if no matching string was found
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
		int index = 0;
		string instruction_strings;
        bool lineUnderIngredients = false;
		for (auto inst : rcp.instructions)
		{
			index++;

            lineUnderIngredients = false;
            if(inst.kitchenware.size() == 0){
                lineUnderIngredients = true;
            }

			string step_text = "Step " + std::to_string(index);
            string ingredients = "";
            if (lineUnderIngredients){
                ingredients = insertIngredientFields("Ingredients: ", inst.ingredients, true, false, "input");
            }
            else {
                ingredients = insertIngredientFields("Ingredients: ", inst.ingredients, true);
            }
                
			string kitchenware = insertStringFields("Kitchenware: ", inst.kitchenware, true, "input");
			string body = insertInstructionBody(inst.body);
			string yield = insertIngredientFields("<text class='arrow'> &#10230 <text>", inst.yields,true, false, "yield");
			
			string instruction_string = step_template;
			
			// replace placeholders with final HTML
			replace(instruction_string, "~stepText~", step_text);
			replace(instruction_string, "~instructionIngredients~", ingredients);
			replace(instruction_string, "~instructionKitchenware~", kitchenware);
			replace(instruction_string, "~instructionBody~", body);
			replace(instruction_string, "~instructionYield~", yield);
			instruction_strings += instruction_string;
		}
        
		// format final HTML strings
		string servings = insertServings(rcp.servings);
		string tags = insertStringFields("Tags: ", rcp.tags);
		string kitchenware = insertStringFields("Kitchenware: ", rcp.kitchenware);
		string ingredients = insertIngredientFields("Ingredients: ", rcp.ingredients);
		string optionalIngredients = insertIngredientFields("Optional: ",rcp.ingredients,false,true);
		string nutrients = insertIngredientFields("Nutrients", rcp.nutrients);
		string totalTime = insertTime("Total time: ", rcp.time.total_time.c_str());
		string prepTime = insertTime("Prep time: ", rcp.time.prep_time.c_str());
		string cookTime = insertTime("Cook time: ", rcp.time.cook_time.c_str());

		string output_string = base_template; // convert base template to string

		// replace placeholders with final HTML
		replace(output_string, "~title~", rcp.title.c_str());
		replace(output_string, "~servings~", servings);
		replace(output_string, "~description~", rcp.description.c_str());
		replace(output_string, "~total-time~", totalTime);
		replace(output_string, "~prep-time~", prepTime);
		replace(output_string, "~cook-time~", cookTime);
		replace(output_string, "~tags~", tags.c_str());
		replace(output_string, "~ingredients~", ingredients.c_str());
		replace(output_string, "~optionalIngredients~", optionalIngredients.c_str());
		replace(output_string, "~kitchenware~", kitchenware.c_str());
		replace(output_string, "~nutrients~", nutrients.c_str());
		replace(output_string, "~instructions~", instruction_strings.c_str());

		file << output_string << std::endl;
		file.close();

		return true;
	}

}