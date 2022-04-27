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

template <typename T, typename A>
bool vec_contains(std::vector<T, A> vector, T value)
{
    for (const T t : vector)
        if (value == t)
            return true;
    return false;
}

bool map_is_initalized = false;

namespace epicr
{

    std::map<epicr_unit_type, std::vector<std::string>> units_in_type;
    std::map<std::string, std::vector<std::string>> unit_aliases;
    std::map<epicr_unit_system, std::vector<std::string>> units_in_system;

    void initialize_maps()
    {
        if (map_is_initalized)
            return;

        /* all units and their type */
        std::vector<std::string> weight_units = {
            "g", "kg", "oz", "lbs"};
        units_in_type[E_UT_WEIGHT] = weight_units;

        std::vector<std::string> volume_units = {
            "ml", "dl", "l", "fl-oz", "cup", "qt", "gal"};
        units_in_type[E_UT_VOLUME] = volume_units;

        std::vector<std::string> length_units = {
            "mm", "cm", "in", "ft"};
        units_in_type[E_UT_LENGTH] = length_units;

        std::vector<std::string> temperature_units = {
            "c", "f"};
        units_in_type[E_UT_TEMPERATURE] = temperature_units;

        /* alias' of the different units */
        unit_aliases["g"] = {"g", "gram", "grams"};
        unit_aliases["kg"] = {"kg", "kilogram", "kgs", "kilograms"};
        unit_aliases["oz"] = {"oz", "ounce", "ounces"};
        unit_aliases["lbs"] = {"lbs", "pounds"};

        unit_aliases["ml"] = {"ml", "mili liter"};
        unit_aliases["dl"] = {"dl", "deci liter"};
        unit_aliases["l"] = {"l", "liter"};
        unit_aliases["fl-oz"] = {"fl-oz", "fluid ounce"};
        unit_aliases["cup"] = {"cup", "cups"};
        unit_aliases["qt"] = {"qt", "quarts"};
        unit_aliases["gal"] = {"gal", "gallon", "gallons"};

        unit_aliases["mm"] = {"mm", "milimeter", "milimeters"};
        unit_aliases["cm"] = {"cm", "centimeter", "centimeters"};
        unit_aliases["in"] = {"in", "inch", "inches"};
        unit_aliases["ft"] = {"ft", "feet"};

        unit_aliases["c"] = {"c", "celsius"};
        unit_aliases["f"] = {"f", "fahrenheit"};

        units_in_system[E_US_METRIC] = {
            "g", "kg", "ml", "dl", "l", "mm", "cm", "c"};
        units_in_system[E_US_IMPERIAL] = {
            "oz", "lbs", "fl-oz", "cup", "qt", "gal", "in", "ft", "f"};

        map_is_initalized = true;
    }
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
        error = "No error";
    };

    void IngredientVerifier::visit(recipe *a_rcp)
    {

        /* shallow copy of input recipe */
        // recipe rcp = a_rcp;

        /* fill the symbol table */
        for (auto ingr : a_rcp->ingredients)
        {
            symbols[ingr.name] = ingr;          // used to check correspondance between ingredient list and instructions
            original_symbols[ingr.name] = ingr; // copy of ingredients
        }

        /* check the flow of the instructions are reasonable
        aka check if the recipe comply by all the rules */

        int instruction_count = 0;

        for (auto &inst : a_rcp->instructions)

        {
            instruction_count++;
            /* consume */
            for (auto &ingr : inst.ingredients)
            {

                if (!ingredient_in_map(ingr.name, symbols))
                {
                    /* err */
                }

                if (ingr.amount.isRelativeAmount)
                {

                    /* verify that ingredient exists in either */

                    if (ingr.amount.relativeAmount == "rest" && !ingredient_in_map(ingr.name, original_symbols))
                    {
                        char *err = (char *)malloc(100);
                        sprintf(err, "Ingredient %s used in instruction not found in ingredients list", ingr.name.c_str());
                        ERR(err);
                        return;
                    }

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
                if (!map_contains(symbols, ingr.name))
                {
                    /* TODO: sprintf into error message with ingr name */
                    ERR("Ingredient used in instruction not found in ingredients list");
                    return;
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
                {
                    symbols[yield.name] = yield;
                    original_symbols[yield.name] = yield;
                }
            }
        }
        bool titleIngredientRemaining = false;
        for (auto KeyValuePair : symbols)
        {
            ingredient ingr = KeyValuePair.second;
            if (to_lower(ingr.name) == to_lower(a_rcp->title))
                titleIngredientRemaining = true;
            else if (ingr.amount.number != 0 && !ingr.amount.isUncountable)
            {
                char *err_msg = (char *)malloc(200);
                sprintf(err_msg, "Unused ingredient: %s%s",
                        ingr.name.c_str(), amount_to_string(ingr.amount).c_str());
                ERR(err_msg);
            }
        }
        if (!titleIngredientRemaining)
            ERR("Title-ingredient muse remain after all instructions");
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

#pragma region AmountConverter implemention

    AmountConverter::AmountConverter()
    {
        initialize_maps();
        error = "No error";
        has_error = false;
    }

    bool AmountConverter::is_convertable(std::string unit)
    {
        for (const auto &pair : unit_aliases)
            for (const auto &alias : pair.second)
                if (to_lower(unit) == alias)
                    return true;

        return false;
    }

    /**
     * @brief Convert a unit alias into the standard, i.e. 'grams' to 'g'.
     *
     * @param unit Unit to standardize
     * @return std::string The standard for a specific unit alias
     */
    std::string AmountConverter::standardize(std::string unit)
    {
        std::string unit_lower = to_lower(unit);
        for (const auto &pair : unit_aliases)
            for (const auto &alias : pair.second)
                if (unit_lower == alias)
                    return unit_aliases[pair.first][0];

        ERR("No valid standard found for unit");
        return "";
    }

    /**
     * @brief Convert a unit into a different unit system, i.e. E_TT_METRIC to E_TT_IMPERIAL
     *
     * @param amnt The amount to be scaled
     * @param system The system to be scaled into
     * @return amount
     */
    void AmountConverter::scale_amount(amount *amnt, epicr_unit_system tar_sys)
    {

        /* If the target unit system is undefined, don't modify
         * any of the ingredients */
        if (tar_sys == E_US_NONE)
            return;

        std::string standardized = to_lower(standardize(amnt->unit));
        epicr_unit_system cur_sys;

        /* find out what system the current unit is in*/
        for (const auto &pair : epicr::units_in_system)
        {
            for (const auto &unit_in_sys : pair.second)
            {
                if (standardized == unit_in_sys)
                {
                    cur_sys = pair.first;
                    break;
                }
            }
        }

        /* if no system change is needed, short-circuit  */
        if (cur_sys == tar_sys)
            return;

        /* Conversions metric -> imperial */
        if (tar_sys == E_US_IMPERIAL)
        {
            if (standardized == "g")
            {
                amnt->number = amnt->number * G_TO_OZ;
                amnt->unit = "oz";
            }
            else if (standardized == "kg")
            {
                amnt->number = amnt->number * KG_TO_LBS;
                amnt->unit = "lbs";
            }
            else if (standardized == "ml")
            {
                amnt->number = amnt->number * ML_TO_FLOZ;
                amnt->unit = "fl-oz";
            }
            else if (standardized == "dl")
            {
                amnt->number = amnt->number * DL_TO_FLOZ;
                amnt->unit = "fl-oz";
            }
            else if (standardized == "l")
            {
                amnt->number = amnt->number * L_TO_QT;
                amnt->unit = "qt";
            }
            else if (standardized == "m")
            {
                amnt->number = amnt->number * M_TO_FEET;
                amnt->unit = "ft";
            }
            else if (standardized == "cm")
            {
                amnt->number = amnt->number * CM_TO_INCH;
                amnt->unit = "in";
            }
            else if (standardized == "mm")
            {
                amnt->number = amnt->number * MM_TO_INCH;
                amnt->unit = "in";
            }
            else if (standardized == "c")
            {
                amnt->number = C_TO_F(amnt->number);
                amnt->unit = "f";
            }
        }
        else
        {
            if (standardized == "oz")
            {
                amnt->number = amnt->number / G_TO_OZ;
                amnt->unit = "g";
            }
            else if (standardized == "lbs")
            {
                amnt->number = amnt->number / KG_TO_LBS;
                amnt->unit = "kg";
            }
            else if (standardized == "fl-oz")
            {
                amnt->number = amnt->number / ML_TO_FLOZ;
                amnt->unit = "ml";
            }
            else if (standardized == "fl-oz") /* This if is never entered */
            {
                amnt->number = amnt->number / DL_TO_FLOZ;
                amnt->unit = "dl";
            }
            else if (standardized == "qt")
            {
                amnt->number = amnt->number / L_TO_QT;
                amnt->unit = "l";
            }
            else if (standardized == "ft")
            {
                amnt->number = amnt->number / M_TO_FEET;
                amnt->unit = "m";
            }
            else if (standardized == "in")
            {
                amnt->number = amnt->number / CM_TO_INCH;
                amnt->unit = "cm";
            }
            else if (standardized == "in") /* This if is never entered */
            {
                amnt->number = amnt->number / MM_TO_INCH;
                amnt->unit = "mm";
            }
            else if (standardized == "f")
            {
                amnt->number = F_TO_C(amnt->number);
                amnt->unit = "c";
            }
        }
    }

    void AmountConverter::visit(recipe *rcp)
    {

        std::vector<amount *> scaleables;

        /* Get all amounts from ingredients */
        for (auto &ingr : rcp->ingredients)
        {
            amount *amnt_ptr = &ingr.amount;
            scaleables.push_back(amnt_ptr);
        }

        /* Get all amounts from instructions */
        for (auto &inst : rcp->instructions)
        {
            for (auto &ingr : inst.ingredients)
                scaleables.push_back(&(ingr.amount));

            for (auto &word : inst.body)
            {
                if (word.is_amount)
                    scaleables.push_back(&(word.value));
            }

            for (auto &ingr : inst.yields)
                scaleables.push_back(&(ingr.amount));
        }

        for (auto &amnt : scaleables)
        {
            if (!is_convertable(amnt->unit))
                continue;

            /* Scale to the unit system from clargs */
            scale_amount(amnt, epicr::clargs.unit_system);
        }
    }

#pragma endregion
}