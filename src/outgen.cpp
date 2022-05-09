#include "epicr.h"
#include <filesystem>

using std::string;

#define MAX_S_LENGTH 4096
#define MAX_B_LENGTH 65536 /* 4096 * 16 */
namespace epicr
{
	/* create html file */
	string load_HTML_template(string template_name)
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

		/* create txt file */
	string load_txt_template(string template_name)
	{
		string file_content;
		string dir = "src";
		string file_name = template_name + ".txt";
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

	/* constructs HTML and .txt strings for time fields */
	string insert_time(string header, string time, string template_style)
	{
		if (time == "")
			return "";

		if(template_style == "HTML") {
			return "<div class=time-header>" + header + "</div><div class=time-content>" + time + "</div>";
		}
		return header + time;
	}

	/* constructs HTML and .txt strings for servings field */
	string insert_servings(servings servings, string template_style)
	{
		string number = std::to_string(servings.count);
		string descriptor = servings.descriptor;
		
		
		if (servings.count == 0)
		{
			number = "1";
			descriptor = "servings";
		}

		if(template_style == "HTML") {
			string result = "<input onClick='this.select();' type='number' class='servings' max='1000' min='0' oninput='update_numbers()'";
			result += "onfocusout='update_numbers(this)' value=" + number + "> " + descriptor;
			return result;
		}
		
		return "Servings: " + number + " " + descriptor;

	}

	/* insert number and amount */
	string generate_ingredient_html(string number, string unit)
	{
		string result = "(<text class='number'>" + number + "</text>";
		if (unit != "")
			result += " <text class='unit'>" + unit;
		else
			result += "<text class='unit'>";
		result += "</text>)";
		return result;
	}

	/* construct HTML string for ingredient list item */
	string insert_ingredient(std::vector<ingredient> ingredients, size_t i)
	{
		string result = "<li>" + ingredients[i].name + " ";
		if (!ingredients[i].amount.is_uncountable)
			result += generate_ingredient_html(epicr::round_double_to_string(ingredients[i].amount.number), ingredients[i].amount.unit);
		return result;
	}

	/* constructs HTML and .txt strings for ingredients listing */
	string insert_declaration_ingredients(string header, std::vector<ingredient> ingredients, string template_style)
	{
		string result;

		if(template_style == "HTML") {
		string result = "<h3 class=ingredients-header><strong>Ingredients</strong></h3>";
		for (size_t i = 0; i < ingredients.size(); i++)
		{
			if (ingredients[i].is_ingredient_ref)
			{
				result += "<li><a class='is_ingredient_ref' target='_blank'>" + ingredients[i].name + " ";
				result += generate_ingredient_html(epicr::round_double_to_string(ingredients[i].amount.number), ingredients[i].amount.unit) + "</a>";
			}
			else if (ingredients[i].is_optional)
				continue;
			else
				result += insert_ingredient(ingredients, i) + "</text>";
		}
		return result;

		}

		for (size_t i = 0; i < ingredients.size(); i++)
		{
			result +=ingredients[i].name;
			if (!ingredients[i].amount.is_uncountable) {
			result += " (" + epicr::round_double_to_string(ingredients[i].amount.number) + " " + ingredients[i].amount.unit + ")";
				if (ingredients[i].is_ingredient_ref) {
				result += " [Reference recipe: " + ingredients[i].name + ".txt]";	
			}
			}
			else {
				result += " [Uncountable]";
			}
			if (ingredients[i].is_optional){
				result += " [Optional]";
			}
			result += "\n";
		}
		return header + "\n" + result;		
	}

	/* construct HTML string for optional ingredients listings */
	string insert_optional_ingredients(std::vector<ingredient> ingredients)
	{
		bool thereIsAtLeastOneOptionalIngredient = false;
		string result = "<h3 class=field-header><strong>Optional</strong></h3>";
		for (size_t i = 0; i < ingredients.size(); i++)
		{
			if (!ingredients[i].is_optional)
				continue;
			thereIsAtLeastOneOptionalIngredient = true;
			result += insert_ingredient(ingredients, i);
		}
		if (!thereIsAtLeastOneOptionalIngredient)
			return "";
		return result;
	}

	/* constructs HTML and .txt strings for a list -> used for tags and kitchenware */
	string insert_text_in_list(string header, std::vector<string> listElements, string template_style)
	{
		if (listElements.size() == 0)
			return "";
		string result;

		if(template_style == "HTML") {
			string result = "<h3>" + header + "</h3>";
			for (size_t i = 0; i < listElements.size(); i++)
				result += "<li>" + listElements[i] + "</li>";
			return result;
		}


		if(header == "Tags: "){
			for (size_t i = 0; i < listElements.size(); i++) {
			result += listElements[i] + ", ";
			}
			result.erase(result.size()-2);
			return header + result + "\n";
		}
		else { //kitchenware
			for (size_t i = 0; i < listElements.size(); i++) {
			result += listElements[i] + "\n";
			}
			return header + "\n" + result;
		}	
	}

	/* constructs HTML and .txt strings for nutrients listing */
	string insert_nutrients(std::vector<ingredient> nutrients, string template_style)
	{
		string result;
		if (nutrients.size() == 0) {
			return "";
		}
			
		if(template_style == "HTML") {
			string result = "<h3 class='field-header'><strong>Nutrients*</strong></h3>";
		
			for (size_t i = 0; i < nutrients.size(); i++)
			{
				std::string number = epicr::round_double_to_string(nutrients[i].amount.number);
				std::string unit = nutrients[i].amount.unit;
				result += "<li>" + nutrients[i].name + " (" + number + " " + unit + ")</li>";
			}
			result += "<text>*pr. 100 grams</text>";
			return result;
		}

		for (size_t i = 0; i < nutrients.size(); i++)
		{
			std::string number = epicr::round_double_to_string(nutrients[i].amount.number);
			std::string unit = nutrients[i].amount.unit;
			result += nutrients[i].name + " (" + number + " " + unit + ")\n";
		}
		result += "*pr. 100 grams";
		return "Nutrients:\n" + result;
	}

	/* constructs HTML and .txt strings for ingredient field in instructions */
	string insert_instruction_ingredients(string header, std::vector<ingredient> ingredients, string template_style)
	{
		if(template_style == "HTML") {
			string result = "<h5><strong>" + header + "</strong>";
			for (size_t i = 0; i < ingredients.size(); i++)
			{
				if (i != 0)
					result += ", ";
				result += ingredients[i].name + " ";
				if (!ingredients[i].amount.is_uncountable)
					result += generate_ingredient_html(epicr::round_double_to_string(ingredients[i].amount.number), ingredients[i].amount.unit);
			}
			result += "</h5>";
			return result;
		}

		string result;
		for (size_t i = 0; i < ingredients.size(); i++)
			{
				if (i != 0)
					result += ", ";
				result += ingredients[i].name + " ";
				if (!ingredients[i].amount.is_uncountable)
					result += epicr::round_double_to_string(ingredients[i].amount.number) + " " + ingredients[i].amount.unit;
			}

		return header + "(" + result + ")";

	}

	/* constructs HTML and .txt strings for kitchenware field in instructions */
	string insert_instruction_kitchenware(string header, std::vector<string> kitchenware, string template_style)
	{
		string result;
		if (kitchenware.size() == 0)
				return "";
		
		if(template_style == "HTML"){
			string result = "<strong>" + header + "</strong>";
			for (size_t i = 0; i < kitchenware.size(); i++)
			{
				result += kitchenware[i];
				if (i != kitchenware.size() - 1)
					result += ", ";
			}
			result += "<hr class='body-rule'>";
			return result;
		}

		for (size_t i = 0; i < kitchenware.size(); i++)
			{
				result += kitchenware[i];
				if (i != kitchenware.size() - 1)
					result += ", ";
			}

		return header + "(" + result + ")";
		
	}

	/*return a new string with its newlines converted to <br>'s*/
	std::string newlines_in_html(string a_string)
	{
		for (size_t i = 0;i<a_string.size();i++) 
		{
			if (a_string[i] == '\n')
			{
				a_string.replace(i,1,"<br>");
			}
		}
		return a_string;
	}

	
	/* constructs HTML and .txt strings for instructions body */
	string insert_instruction_body(std::vector<instruction_word> body, string template_style)
	{
		if(template_style == "HTML"){
			string result = "<p class='instruction-body'>";
			for (size_t i = 0; i < body.size(); i++)
			{	
				result += newlines_in_html(body[i].spelling);
				if (body[i].is_amount == true)
				{
					result += "<text class='number'>" + epicr::round_double_to_string(body[i].value.number) + "</text>";
					result += " <text class='unit'>" + body[i].value.unit + "</text>";
				}
			}
			result += "</p>";
			return result;
		}
		string result;
		for (size_t i = 0; i < body.size(); i++)
		{	
			result += body[i].spelling;
			if (body[i].is_amount == true)
			{
				result += epicr::round_double_to_string(body[i].value.number);
				result += body[i].value.unit;
			}
		}
		return strip_spaces_right(result) + "\n"; //readability ig

	}

	/* constructs HTML and .txt strings for yield field in instructions */
	string insert_yield_ingredients(string header, std::vector<ingredient> ingredients, string template_style)
	{
		if(template_style == "HTML") {
			string result = "<h5><strong>" + header + "</strong>";
			for (size_t i = 0; i < ingredients.size(); i++)
			{
				if (i != 0)
					result += ", ";
				result += ingredients[i].name + " ";
				if (!ingredients[i].amount.is_uncountable)
					result += generate_ingredient_html(epicr::round_double_to_string(ingredients[i].amount.number), ingredients[i].amount.unit);
			}
			result += "</h5>";
			return result;
			}
	
		string result;
		for (size_t i = 0; i < ingredients.size(); i++)
		{
			if (i != 0)
				result += ", ";
			result += ingredients[i].name + " ";
			if (!ingredients[i].amount.is_uncountable)
				result += epicr::round_double_to_string(ingredients[i].amount.number) + ingredients[i].amount.unit;
		}
		return header + "\n" + result +"\n";

	}

	/* insert description in HTML unless it is empty */
	string insert_description(string description)
	{
		if (description == "")
			return "";
		string result = "<div class='collapsible-wrapper'><div class='arrow-down'></div><h3 class='collapsible'>";
		result += "Description</h3></div><div class='collapsible-body'>" + newlines_in_html(description) + "</div>";
		return result;
	}
	
	
	// finds a string in another string and replace it with a third string
	bool replace(string &str, const string &from_string, const string &to_string)
	{
		size_t start_position = str.find(from_string);
		if (start_position == string::npos) // if no matching string was found
			return false;
		str.replace(start_position, from_string.length(), to_string);
		return true;
	}

	/* generate HTML strings and replace placeholders */
	bool generate_html(recipe rcp, string filename)
	{
		string base_template_s = load_HTML_template("base");
		const char *base_template = base_template_s.c_str();
		string step_template_s = load_HTML_template("step");
		const char *step_template = step_template_s.c_str();

		std::filesystem::create_directory("dist");

		std::ofstream file{filename};
		if (!file.is_open())
			return false;
		int index = 0;
		string instruction_strings;

		/* format final HTML strings for instructions*/
		for (auto inst : rcp.instructions)
		{
			index++;
			string instruction_string = step_template;
			string step_text = "Step " + std::to_string(index);
			string instructionIngredients = "";
			string instructionKitchenware = "";
			string body = "";
			string yield = "";

			if (inst.ingredients.size() > 0)
				instructionIngredients = insert_instruction_ingredients("Ingredients: ", inst.ingredients, "HTML");

			if (inst.kitchenware.size() == 0)
				instructionIngredients += "<hr class='body-rule'>";
			else
				instructionKitchenware += insert_instruction_kitchenware("Kitchenware: ", inst.kitchenware, "HTML");

			if (inst.body.size() > 0)
				body += insert_instruction_body(inst.body, "HTML");

			if (inst.yields.size() > 0)
			{
				yield += "<hr class='body-rule'>";
				yield += insert_yield_ingredients("<text class='arrow'> &#10230 <text>", inst.yields, "HTML");
			}

			/* replace step placeholders with final HTML */
			replace(instruction_string, "~stepText~", step_text);
			replace(instruction_string, "~instructionIngredients~", instructionIngredients);
			replace(instruction_string, "~instructionKitchenware~", instructionKitchenware);
			replace(instruction_string, "~instructionBody~", body);
			replace(instruction_string, "~instructionYield~", yield);
			instruction_strings += instruction_string;
		}

		/* format final HTML strings */
		string servings = insert_servings(rcp.servings, "HTML");
		string tags = insert_text_in_list("Tags: ", rcp.tags, "HTML");
		string kitchenware = insert_text_in_list("Kitchenware", rcp.kitchenware, "HTML");
		string ingredients = insert_declaration_ingredients("Ingredients: ", rcp.ingredients, "HTML");
		string optional_ingredients = insert_optional_ingredients(rcp.ingredients);
		string nutrients = insert_nutrients(rcp.nutrients, "HTML");
		string total_time = insert_time("Total time: ", rcp.time.total_time.c_str(), "HTML");
		string prep_time = insert_time("Prep time: ", rcp.time.prep_time.c_str(), "HTML");
		string cook_time = insert_time("Cook time: ", rcp.time.cook_time.c_str(), "HTML");
		string description = insert_description(rcp.description.c_str());

		string output_string = base_template; // convert base template to string

		/* replace placeholders with final HTML */
		replace(output_string, "~title~", rcp.title.c_str());
		replace(output_string, "~servings~", servings);
		replace(output_string, "~description~", description);
		replace(output_string, "~total-time~", total_time);
		replace(output_string, "~prep-time~", prep_time);
		replace(output_string, "~cook-time~", cook_time);
		replace(output_string, "~tags~", tags.c_str());
		replace(output_string, "~ingredients~", ingredients.c_str());
		replace(output_string, "~optionalIngredients~", optional_ingredients.c_str());
		replace(output_string, "~kitchenware~", kitchenware.c_str());
		replace(output_string, "~nutrients~", nutrients.c_str());
		replace(output_string, "~instructions~", instruction_strings.c_str());

		file << output_string << std::endl;
		file.close();

		return true;
	}


	/* generate strings for txt and replace placeholders */
	bool generate_txt(recipe rcp, string filename) {
		string base_template_txt_s = load_txt_template("base");
		const char *base_template_txt = base_template_txt_s.c_str();
		string step_template_txt_s = load_txt_template("step");
		const char *step_template_txt = step_template_txt_s.c_str();

		std::filesystem::create_directory("dist");

		std::ofstream file{filename};
		if (!file.is_open())
			return false;
		int index = 0;
		string instruction_strings;

		for (auto inst : rcp.instructions)
		{
			index++;
			string instruction_string = step_template_txt;
			string step_text = "\nStep " + std::to_string(index);
			string instructionIngredients = "";
			string instructionKitchenware = "";
			string body = "";
			string yield = "";

			if (inst.ingredients.size() > 0)
				instructionIngredients = insert_instruction_ingredients("with", inst.ingredients, "txt");

			if (inst.kitchenware.size() == 0)
				instructionIngredients += "";
			else
				instructionKitchenware += insert_instruction_kitchenware("using", inst.kitchenware, "txt");

			if (inst.body.size() > 0)
				body += insert_instruction_body(inst.body, "txt");

			if (inst.yields.size() > 0)
			{
				yield += insert_yield_ingredients("yield: ", inst.yields, "txt");
			}

			/* replace step placeholders with final strings */
			replace(instruction_string, "~stepText~", step_text);
			replace(instruction_string, "~instructionIngredients~", instructionIngredients);
			replace(instruction_string, "~instructionKitchenware~", instructionKitchenware);
			replace(instruction_string, "~instructionBody~", body);
			replace(instruction_string, "~instructionYield~", yield);
			instruction_strings += instruction_string;
		}


		/* format final strings for .txt*/
		string title = "Title: ";
		title += rcp.title.c_str();
		string description = "Description: ";
		description += rcp.description.c_str();
		string servings = insert_servings(rcp.servings, "txt");
		string tags = insert_text_in_list("Tags: ", rcp.tags, "txt");
		string kitchenware = insert_text_in_list("Kitchenware: ", rcp.kitchenware, "txt");
		string ingredients = insert_declaration_ingredients("Ingredients: ", rcp.ingredients, "txt");
		string nutrients = insert_nutrients(rcp.nutrients, "txt");
		string total_time = insert_time("Total time: ", rcp.time.total_time.c_str(), "txt");
		string prep_time = insert_time("Prep time: ", rcp.time.prep_time.c_str(), "txt");
		string cook_time = insert_time("Cook time: ", rcp.time.cook_time.c_str(), "txt");

		string output_string = base_template_txt; // convert base template to string

		/* replace placeholders with final strings */
		replace(output_string, "~title~", title);
		replace(output_string, "~servings~", servings);
		replace(output_string, "~description~", description);
		replace(output_string, "~total-time~", total_time);
		replace(output_string, "~prep-time~", prep_time);
		replace(output_string, "~cook-time~", cook_time);
		replace(output_string, "~tags~", tags.c_str());
		replace(output_string, "~ingredients~", ingredients.c_str());
		replace(output_string, "~kitchenware~", kitchenware.c_str());
		replace(output_string, "~nutrients~", nutrients.c_str());
		replace(output_string, "~instructions~", instruction_strings.c_str());

		file << output_string << std::endl;
		file.close();

		return true;
	}
}