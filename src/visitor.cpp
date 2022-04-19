#include "./epicr.h"

#define ERR(s)            \
    {                     \
        has_error = true; \
        error = s;        \
    }

/* Check if something exits in the map. E.g checks if an ingredient is in the ingredient list */
bool map_contains(std::unordered_map<std::string, epicr::ingredient> m, std::string k)
{
    for (auto KeyValuePair : m)
    {
        if (KeyValuePair.first == k)
            return true;
    }
    return false;
}

namespace epicr::visitor
{
/* Just used to group things */
#pragma region IngredientVerifier implementation

    IngredientVerifier::IngredientVerifier()
    {
        symbols = std::unordered_map<std::string, ingredient>();
        original_symbols = std::unordered_map<std::string, ingredient>();
        has_error = false;
    };
    void IngredientVerifier::visit(recipe* a_rcp)
    {

        /* shallow copy of input recipe */
        recipe rcp = *a_rcp;

        /* fill the symbol table */
        for (auto ingr : rcp.ingredients)
        {
            symbols[ingr.name] = ingr;
            original_symbols[ingr.name] = ingr;
        }

        /* check the flow of the instructions are reasonable
        aka check if the recipe comply by all the rules */

        int instruction_count = 0;
        for (auto inst : a_rcp->instructions)
        {
            instruction_count++;
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
                        ingr.amount.number *= 0.5f;
                        break;
                    case 'q': /* quarter */
                        ingr.amount = original_symbols[ingr.name].amount;
                        ingr.amount.number *= 0.25f;
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

                if (ingr.amount.number > symbols[ingr.name].amount.number)
                {
                    ingredients_compatible(symbols[ingr.name], ingr);
                    ERR("Used too much of ingredient");
                    return;
                }
                symbols[ingr.name].amount.number -= ingr.amount.number;
            }

            /* yield */
            for (auto yield : inst.yields)
            {
                /* if ingredient already exists, update */
                if (map_contains(symbols, yield.name))
                {
                    if (!ingredients_compatible(symbols[yield.name], yield))
                    {
                        char *err_msg = (char *)malloc(200);
                        sprintf(err_msg, "Differing unit types for yield\
\nIn list: %s\nIn yield: %s\nFor ingredient '%s' in instruction #%i\n",
                                symbols[yield.name].amount.unit.c_str(), yield.amount.unit.c_str(), yield.name.c_str(), instruction_count);
                        ERR(err_msg);
                        return;
                    }
                    symbols[yield.name].amount.number += yield.amount.number;
                }
                /* otherwise, add it to the symbol table */
                else
                    symbols[yield.name] = yield;
            }
        }

        for (auto KeyValuePair : symbols)
        {
            ingredient ingr = KeyValuePair.second;
            if (ingr.amount.number != 0 && !ingr.amount.isUncountable && ingr.name != a_rcp->title)
            {
                ERR("Unused ingredient after instructions");
            }
        }
    }

    bool IngredientVerifier::ingredients_compatible(ingredient a, ingredient b)
    {
        if (a.amount.unit != b.amount.unit)
        {
            ERR("Invalid unit used in instruction");
            return false;
        }
        return true;
    }
#pragma endregion
}
