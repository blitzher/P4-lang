#include "./epicr.h"

#define ERR(s)            \
    {                     \
        has_error = true; \
        error = s;        \
    }

bool map_contains(std::unordered_map<std::string, epicr::ingredient> m, std::string k)
{
    for (auto pair : m)
    {
        if (pair.first == k)
            return true;
    }
    return false;
}

namespace epicr::visitor
{

#pragma region IngredientVerifier implementation
    IngredientVerifier::IngredientVerifier()
    {
        symbols = std::unordered_map<std::string, ingredient>();
        original_symbols = std::unordered_map<std::string, ingredient>();
        has_error = 0;
    };
    void IngredientVerifier::visit(recipe a_rcp)
    {

        /* shallow copy of input recipe */
        recipe rcp = a_rcp;

        /* fill the symbol table */
        for (auto ingr : rcp.ingredients)
        {
            symbols[ingr.name] = ingr;
            original_symbols[ingr.name] = ingr;
        }

        /* check the flow of the instructions are reasonable */

        for (auto inst : rcp.instructions)
        {
            /* consume */
            for (auto ingr : inst.ingredients)
            {
                if (!map_contains(symbols, ingr.name))
                {
                    /* TODO: sprintf into error message with ingr name */
                    ERR("Ingredient used in instruction not found in ingredients list");
                    return;
                }

                if (ingr.amount.isRelativeAmount)
                {
                    switch (ingr.amount.relativeAmount[0])
                    {
                    case 'r': /* rest */
                        ingr.amount = symbols[ingr.name].amount;
                        break;
                    case 'h': /* half */
                        ingr.amount = original_symbols[ingr.name].amount;
                        ingr.amount.amount *= 0.5f;
                        break;
                    case 'q': /* quarter */
                        ingr.amount = original_symbols[ingr.name].amount;
                        ingr.amount.amount *= 0.25f;
                        break;
                    case 'a': /* all */
                        ingr.amount = original_symbols[ingr.name].amount;
                        break;
                    default:

                        char *err = (char *)malloc(100);
                        sprintf(err, "Received unexpected relative amount [%s] for ingredient [%s]", ingr.amount.relativeAmount.c_str(), ingr.name.c_str());
                        ERR(err);
                        return;
                    }
                }

                if (symbols[ingr.name].amount.isUncountable)
                {
                }
                else if (ingr.amount.amount > symbols[ingr.name].amount.amount)
                {
                    ingredients_compatible(symbols[ingr.name], ingr);
                    ERR("Used too much of ingredient");
                    return;
                }

                symbols[ingr.name].amount.amount -= ingr.amount.amount;
            }

            /* yield */
            for (auto yield : inst.yields)
            {
                /* if ingredient already exists, update */
                if (map_contains(symbols, yield.name))
                {
                    if (!ingredients_compatible(symbols[yield.name], yield))
                    {
                        ERR("Yielded ingredient of different unit type than in ingredients list");
                        return;
                    }
                    symbols[yield.name].amount.amount += yield.amount.amount;
                }
                /* if  */
                else
                    symbols[yield.name] = yield;
            }
        }

        for (auto pair : symbols)
        {
            ingredient ingr = pair.second;
            if (ingr.amount.amount != 0 && ingr.name != rcp.title)
            {
                ERR("Unused ingredient after instructions");
            }

            printf("[%s %lf %s]\n", ingr.name.c_str(), ingr.amount.amount, ingr.amount.unit.c_str());
        }
    }

    bool IngredientVerifier::ingredients_compatible(ingredient a, ingredient b)
    {
        printf("a unit: %s, b unit: %s\n", a.amount.unit.c_str(), b.amount.unit.c_str());
        if (a.amount.unit != b.amount.unit)
        {
            ERR("Invalid unit used in instruction");
            return false;
        }
        return true;
    }
#pragma endregion
}