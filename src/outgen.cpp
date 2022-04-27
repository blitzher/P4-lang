#include "epicr.h"
#include <filesystem>

using std::string;

#define MAX_S_LENGTH 4096
#define MAX_B_LENGTH 65536 /* 4096 * 16 */
namespace epicr{
	string load_template(string template_name){
		string file_content;
		string dir = "src";
		string file_name = template_name + ".html";
		string fpath = dir + "/output-templates/" + file_name;
		std::ifstream fstream = epicr::open_file(fpath);

		int gotten_char = fstream.get();
		do {
			file_content += gotten_char;
			gotten_char = fstream.get();
		} while (!fstream.eof());
		return file_content;
	}

    /* construct HTML string for time fields */
    string insertTime(string header, string time){
		if (time == "")
			return "";
        return "<div class=time-header>" + header + "</div><div class=time-content>" + time + "</div>";
	}

    /* construct HTML string for servings field */
    string insertServings(servings servings){
		if (servings.count == 0)
			return "";
        string result = "<input type='number' class='servings' max='1000' min='1' oninput='updateNumbers()'";
        result += "onfocusout='updateNumbers(this)' value=" + std::to_string(servings.count) + "> " + servings.descriptor;
		return result;
	}

    /* construct HTML string for ingredient list item */
    string insertIngredient(std::vector<ingredient> ingredients, size_t i){
        string result = "<li>" + ingredients[i].name + " ";
        if (!ingredients[i].amount.isUncountable){
            std::string number = epicr::double_to_string(ingredients[i].amount.number);
            std::string unit = ingredients[i].amount.unit;
            result += "(<text class='number'>" + number + "</text><text class='unit'>";
            if (unit != "")
                result += " " + unit;
            result += "</text>)"; 
        }
        return result;
    }

    /* construct HTML string for ingredients listing */
	string insertDeclarationIngredients(string header, std::vector<ingredient> ingredients){
		string result = "<h3 class=ingredientsHeader><strong>" + header + "</strong></h3>";
		for (size_t i = 0; i < ingredients.size(); i++){
			if (ingredients[i].isOptional)
				continue;
			result += insertIngredient(ingredients, i);
		}
		return result;
    }
    
    /* construct HTML string for optional ingredients listings */
    string insertOptionalIngredients(string header, std::vector<ingredient> ingredients){
		string result = "<h3 class=fieldHeader><strong>" + header + "</strong></h3>";
		for (size_t i = 0; i < ingredients.size(); i++){
			if (!ingredients[i].isOptional) 
				continue;
            result += insertIngredient(ingredients, i);
		}
		return result;
    }

    /* construct HTML string for a list -> used for tags and kitchenware */
    string insertTextInList(string header, std::vector<string> listElements){
        if (listElements.size() == 0)
			return "";
        string result = "<h3>" + header + "</h3>";
		for (size_t i = 0; i < listElements.size(); i++){
			result += "<li>" + listElements[i] + "</li>";
		}
		return result;
    }

    /* construct HTML string for nutrients listing */
    string insertNutrients(string header, std::vector<ingredient> fields){
		string result = "<h3 class='fieldHeader'><strong>" + header + "</strong></h3>";
		for (size_t i = 0; i < fields.size(); i++){
            std::string number = epicr::double_to_string(fields[i].amount.number);
            std::string unit = fields[i].amount.unit;
			result += "<li>" + fields[i].name + " (" + number + " " + unit + ")</li>";
		}
        result += "<text>*pr. 100 grams</text>";
		return result;
    }

    /* construct HTML string for ingredient field in instructions */
    string insertInstructionIngredients(string header, std::vector<ingredient> fields){
        string result = "<h5><strong>" + header + "</strong>";
		for (size_t i = 0; i < fields.size(); i++){
            if (i != 0)
                result += ", ";
			result += fields[i].name + " ";
			if (!fields[i].amount.isUncountable){
                std::string number = epicr::double_to_string(fields[i].amount.number);
                std::string unit = fields[i].amount.unit;
                result += "(<text class='number'>" + number + "</text><text class='unit'>";
                if (unit != "")
                    result += " " + unit;
                result += "</text>)"; 
            }
		}
        result += "</h5>";
		return result;
	}

    /* construct HTML string for kitchenware field in instructions */
    string insertInstructionKitchenware(string header, std::vector<string> kitchenware){
		if (kitchenware.size() == 0)
			return "";
        string result = "<strong>" + header + "</strong>";
		for (size_t i = 0; i < kitchenware.size(); i++){
			result += kitchenware[i];
            if (i != kitchenware.size() - 1)
                result += ", ";
		}
        result += "<hr class='body-rule'>";
		return result;
	}

    /* construct HTML string for instructions body */
	string insertInstructionBody(std::vector<instruction_word> body){
		string result = "<p class='instructionBody'>";
		for (size_t i = 0; i < body.size(); i++){
			result += body[i].spelling;
			if (body[i].is_amount == true)
				result += epicr::double_to_string(body[i].value.number) + " " + body[i].value.unit;
		}
        result += "</p>";
		return result;
	}

    /* construct HTML string for yield field in instructions */
    string insertYieldIngredients(string header, std::vector<ingredient> fields){
		string result = "<h5><strong>" + header + "</strong>";
		for (size_t i = 0; i < fields.size(); i++)
		{
            if (i != 0)
                result += ", ";
			result += fields[i].name + " ";
			if (!fields[i].amount.isUncountable){
                std::string number = epicr::double_to_string(fields[i].amount.number);
                std::string unit = fields[i].amount.unit;
                result += "(<text class='number'>" + number + "</text><text class='unit'>";
                if (unit != "")
                    result += " " + unit;
                result += "</text>)"; 
            }
		}
        result += "</h5>";
		return result;
    }

	// find a string in another string and replace it with a third string
	bool replace(string &str, const string &from_string, const string &to_string){
		size_t start_position = str.find(from_string);
		if (start_position == string::npos) // if no matching string was found
			return false;
		str.replace(start_position, from_string.length(), to_string);
		return true;
	}

	bool generate_html(recipe rcp, string filename){
		string base_template_s = load_template("base");
		const char *base_template = base_template_s.c_str();
		string step_template_s = load_template("step");
		const char *step_template = step_template_s.c_str();

		std::ofstream file{filename};
		if (!file.is_open())
			return false;
		int index = 0;
		string instruction_strings;
		for (auto inst : rcp.instructions)
		{
			index++;

			string step_text = "Step " + std::to_string(index);

            string instructionIngredients = "";
            string instructionKitchenware = "";
            string body = "";
            string yield = "";

            if (inst.ingredients.size() > 0){
                instructionIngredients = insertInstructionIngredients("Ingredients: ", inst.ingredients);
            }
            if(inst.kitchenware.size() == 0){
                instructionIngredients += "<hr class='body-rule'>";
            }
            else {
                instructionKitchenware += insertInstructionKitchenware("Kitchenware: ", inst.kitchenware);
            }
            if(inst.body.size() > 0){
                body += insertInstructionBody(inst.body);
            }
			if(inst.yields.size() > 0){
                yield += "<hr class='body-rule'>";
                yield += insertYieldIngredients("<text class='arrow'> &#10230 <text>", inst.yields);
            }		
			string instruction_string = step_template;
			
			// replace placeholders with final HTML
			replace(instruction_string, "~stepText~", step_text);
			replace(instruction_string, "~instructionIngredients~", instructionIngredients);
			replace(instruction_string, "~instructionKitchenware~", instructionKitchenware);
			replace(instruction_string, "~instructionBody~", body);
			replace(instruction_string, "~instructionYield~", yield);
			instruction_strings += instruction_string;
		}
        
		// format final HTML strings
		string servings = insertServings(rcp.servings);
		string tags = insertTextInList("Tags: ", rcp.tags);
		string kitchenware = insertTextInList("Kitchenware", rcp.kitchenware);
		string ingredients = insertDeclarationIngredients("Ingredients", rcp.ingredients);
		string optionalIngredients = insertOptionalIngredients("Optional",rcp.ingredients);
		string nutrients = insertNutrients("Nutrients*", rcp.nutrients);
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