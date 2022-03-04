
#include "epicr.h"

#define PRINT_VEC(vec) { for(auto v: vec) file << v << ", "; file << std::endl;}

namespace epicr {
	bool generate_html(recipe rcp, std::string filename) {

		std::ofstream file{filename};
		if (!file.is_open())
			return false;

		file << rcp.title << std::endl;

		file << rcp.time << std::endl;

		file << rcp.description << std::endl;

		PRINT_VEC(rcp.tags);
		file << std::endl;

		PRINT_VEC(rcp.kitchenware);
		file << std::endl;

		for (auto ingr : rcp.ingredients) {
			file << ingr.name << std::endl;
			file << ingr.amount << std::endl;
			file << ingr.unit << std::endl;
		}

		return true;
	}
}