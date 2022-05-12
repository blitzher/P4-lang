#include "./epicr.h"
#include <iterator>

#ifndef FLT_EPSILON
#define FLT_EPSILON __FLT_EPSILON__
#endif

#define ERR(s)                \
    {                         \
        if (!has_error)       \
        {                     \
            has_error = true; \
            error = s;        \
        }                     \
    }

#define WARN(msg)                                 \
    {                                             \
        std::cout << "WARNING ON LINE "           \
                  << __LINE__ << " (" << __FILE__ \
                  << ":" << __FUNCTION__          \
                  << ")" << std::endl;            \
        std::cout << msg << std::endl;            \
        std::cout << std::endl;                   \
    }

/* Check if something exits in the map. E.g checks if an ingredient is in the ingredient list */
template <typename T, typename A>
bool map_contains(std::unordered_map<T, A> map, T key)
{
    for (auto key_value_pair : map)
    {
        if (key_value_pair.first == key)
            return true;
    }
    return false;
}

template <typename K, typename VT>
K key_of_value_in_map_vec(std::map<K, std::vector<VT>> map, VT val, K def)
{
    for (const auto &pair : map)
    {
        for (const auto &vecv : pair.second)
        {
            if (vecv == val)
                return pair.first;
        }
    }
    return def;
}

template <typename T>
bool vec_contains(std::vector<T> vector, T value)
{
    for (const T &t : vector)
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
            "C", "F"};
        units_in_type[E_UT_TEMPERATURE] = temperature_units;

        /* alias' of the different units */
        unit_aliases["g"] = {"g", "gram", "grams"};
        unit_aliases["kg"] = {"kg", "kilogram", "kgs", "kilograms"};
        unit_aliases["oz"] = {"oz", "ounce", "ounces"};
        unit_aliases["lbs"] = {"lbs", "pounds", "lb"};

        unit_aliases["ml"] = {"ml", "milliliter"};
        unit_aliases["dl"] = {"dl", "deciliter"};
        unit_aliases["l"] = {"l", "liter"};
        unit_aliases["fl-oz"] = {"fl-oz", "fluid ounce"};
        unit_aliases["cup"] = {"cup", "cups"};
        unit_aliases["qt"] = {"qt", "quarts"};
        unit_aliases["gal"] = {"gal", "gallon", "gallons"};

        unit_aliases["mm"] = {"mm", "millimeter", "millimeters"};
        unit_aliases["cm"] = {"cm", "centimeter", "centimeters"};
        unit_aliases["in"] = {"in", "inch", "inches"};
        unit_aliases["ft"] = {"ft", "feet", "foot"};

        unit_aliases["C"] = {"C", "celsius"};
        unit_aliases["F"] = {"F", "fahrenheit"};

        units_in_system[E_US_METRIC] = {
            "g", "kg", "ml", "dl", "l", "mm", "cm", "C"};
        units_in_system[E_US_IMPERIAL] = {
            "oz", "lbs", "fl-oz", "cup", "qt", "gal", "in", "ft", "F"};

        map_is_initalized = true;
    }
}

namespace epicr::visitor
{

#pragma region IngredientVerifier implementation

    IngredientVerifier::IngredientVerifier()
    {
        symbols = std::unordered_map<std::string, ingredient>();
        original_symbols = std::unordered_map<std::string, ingredient>();
        uniqueIngredients = std::unordered_set<std::string>();
        has_error = false;
        error = "No error";
    };

    void IngredientVerifier::visit(recipe *a_rcp)
    {

        /* shallow copy of input recipe */
        // recipe rcp = a_rcp;

        /* fill the symbol table and check for duplicate ingredients*/
        for (auto ingr : a_rcp->ingredients)
        {
            symbols[to_lower(ingr.name)] = ingr;          // used to check correspondance between ingredient list and instructions
            original_symbols[to_lower(ingr.name)] = ingr; // copy of ingredients

            /*duplicate ingredients check*/
            size_t ingredients_count = uniqueIngredients.size();
            uniqueIngredients.insert(ingr.name);
            if (ingredients_count == uniqueIngredients.size()) /*if it already exist in the set, it has been previously defined */
            {
                ERR("Duplicate ingredient '" + ingr.name + "' was found");
                return;
            }
        }

        /* check the flow of the instructions are reasonable
        aka check if the recipe comply by all the rules */

        int instruction_count = 0;

        for (auto &inst : a_rcp->instructions)
        {
            if (has_error)
                break;
            instruction_count++;
            /* consume */
            for (auto &ingr : inst.ingredients)
            {
                if (has_error)
                    break;
                if (!ingredient_in_map(ingr.name, symbols))
                {
                    std::string err_msg = "Ingredient '" + ingr.name + "' used in instruction not found in ingredients list";
                    ERR(err_msg);
                    return;
                }

                if (ingr.amount.is_relative_amount && ingr.amount.unit == "")
                {
                    switch (ingr.amount.relative_amount[0])
                    {
                    case 'r': /* rest */
                        if (symbols[to_lower(ingr.name)].amount.number == 0)
                            WARN("Ingredient '" + ingr.name + "' has already been completely consumed, and therefore it has no impact in this context");
                        ingr.amount = symbols[to_lower(ingr.name)].amount;
                        break;
                    case 'h': /* half */
                        ingr.amount = original_symbols[to_lower(ingr.name)].amount;
                        ingr.amount.number *= 0.5f;
                        break;
                    case 'q': /* quarter */
                        ingr.amount = original_symbols[to_lower(ingr.name)].amount;
                        ingr.amount.number *= 0.25f;
                        break;
                    case 'a': /* all */
                        ingr.amount = original_symbols[to_lower(ingr.name)].amount;
                        break;
                    default:
                        std::string err_msg = "Received unexpected relative amount [" +
                                              ingr.amount.relative_amount + "] for Ingredient '" + ingr.name + "'";
                        ERR(err_msg);
                        return;
                    }
                }
                else if (ingr.amount.unit == "%")
                {
                    double percent = ingr.amount.number;
                    ingr.amount = original_symbols[ingr.name].amount;
                    ingr.amount.number *= percent;
                }

                amount original_amount = ingr.amount;
                if (ingredients_compatible(symbols[to_lower(ingr.name)], ingr))
                {
                    ingr = match_ingredients(ingr, symbols[to_lower(ingr.name)]);
                }

                if (ingr.amount.number - symbols[to_lower(ingr.name)].amount.number > FLT_EPSILON)
                {
                    std::string err_msg = "Used too much of Ingredient '" + ingr.name + "'. " +
                                          epicr::round_double_to_string(symbols[to_lower(ingr.name)].amount.number) + " is available, and " +
                                          epicr::round_double_to_string(ingr.amount.number) + " was used";
                    ERR(err_msg);
                    return;
                }
                symbols[to_lower(ingr.name)].amount.number -= ingr.amount.number;
                ingr.amount = original_amount;
            }

            /* yield */
            for (auto yield : inst.yields)
            {
                /* if ingredient already exists, update */
                if (map_contains(symbols, yield.name))
                {
                    if (!ingredients_compatible(symbols[to_lower(yield.name)], yield))
                    {
                        return;
                    }
                    yield = match_ingredients(yield, symbols[to_lower(yield.name)]);
                    symbols[to_lower(yield.name)].amount.number += yield.amount.number;
                }
                /* otherwise, add it to both symbol tables */
                else
                {
                    symbols[to_lower(yield.name)] = yield;
                    original_symbols[to_lower(yield.name)] = yield;
                }
            }
        }
        if (has_error)
            return;
        bool title_ingredient_remaining = false;
        for (auto key_value_pair : symbols)
        {
            ingredient ingr = key_value_pair.second;

            if (to_lower(ingr.name) == to_lower(a_rcp->title))
                title_ingredient_remaining = true;
            else if (ingr.amount.number > FLT_EPSILON && !ingr.amount.is_uncountable)
            {

                std::string err_msg = "Unused Ingredient '" + ingr.name + amount_to_string(ingr.amount) + "'";
                ERR(err_msg);
            }
        }
        if (!title_ingredient_remaining)
        {
            std::string err_msg = "Title-Ingredient '" + a_rcp->title + "' must remain after all instructions have been executed";
            ERR(err_msg);
        }
    }

    bool IngredientVerifier::ingredients_compatible(ingredient a, ingredient b)
    {
        /*if units are not of the same type*/
        if (
            key_of_value_in_map_vec(units_in_type, a.amount.unit, E_UT_NONE) != key_of_value_in_map_vec(units_in_type, b.amount.unit, E_UT_NONE))
        {
            ERR("Unit '" + a.amount.unit + "' and unit '" + b.amount.unit + "' are not the same type of unit for Ingredient '" + a.name + "'");
            return false;
        }
        epicr_unit_system ingredient_a_unit_system = key_of_value_in_map_vec(units_in_system, a.amount.unit, E_US_NONE);
        epicr_unit_system ingredient_b_unit_system = key_of_value_in_map_vec(units_in_system, b.amount.unit, E_US_NONE);
        if (a.amount.unit == b.amount.unit)
        {
            return true;
        }
        /*if one of them (or both) are a user defined unit*/
        else if (ingredient_a_unit_system == E_US_NONE || ingredient_b_unit_system == E_US_NONE)
        {
            ERR("User created units are not compatible with each other. The user created units are unit " + a.amount.unit + " and unit " + b.amount.unit);
            return false;
        }
        /*if units are not in the same unit system*/
        else if (ingredient_a_unit_system != ingredient_b_unit_system)
        {
            ERR("Unit '" + a.amount.unit + "' and unit '" + b.amount.unit + "' are not in the same unit system for Ingredient '" + a.name + "'");
            return false;
        }
        return true;
    }
    ingredient IngredientVerifier::match_ingredients(ingredient a, const ingredient b)
    {
        /*metric*/
        if (key_of_value_in_map_vec(units_in_system, b.amount.unit, E_US_NONE) == E_US_METRIC)
        {
            /*weight units*/
            if (a.amount.unit == "g")
            {
                if (b.amount.unit == "kg")
                {
                    a.amount.number *= G_TO_KG;
                    a.amount.unit = "kg";
                }
            }
            else if (a.amount.unit == "kg")
            {
                if (b.amount.unit == "g")
                {
                    a.amount.number /= G_TO_KG;
                    a.amount.unit = "g";
                }
            }
            /*volume units*/
            else if (a.amount.unit == "ml")
            {
                if (b.amount.unit == "dl")
                {
                    a.amount.number *= ML_TO_DL;
                    a.amount.unit = "dl";
                }
                else if (b.amount.unit == "l")
                {
                    a.amount.number *= ML_TO_DL * DL_TO_L;
                    a.amount.unit = "l";
                }
            }
            else if (a.amount.unit == "dl")
            {
                if (b.amount.unit == "l")
                {
                    a.amount.number *= DL_TO_L;
                    a.amount.unit = "l";
                }
                else if (b.amount.unit == "ml")
                {
                    a.amount.number /= ML_TO_DL;
                    a.amount.unit = "ml";
                }
            }
            else if (a.amount.unit == "l")
            {
                if (b.amount.unit == "dl")
                {
                    a.amount.number /= DL_TO_L;
                    a.amount.unit = "dl";
                }
                else if (b.amount.unit == "ml")
                {
                    a.amount.number /= ML_TO_DL / DL_TO_L;
                    a.amount.unit = "ml";
                }
            }
            /*length units*/
            else if (a.amount.unit == "mm")
            {
                if (b.amount.unit == "cm")
                {
                    a.amount.number *= MM_TO_CM;
                    a.amount.unit = "cm";
                }
            }
            else if (a.amount.unit == "cm")
            {
                if (b.amount.unit == "mm")
                {
                    a.amount.number /= MM_TO_CM;
                    a.amount.unit = "mm";
                }
            }
        }
        /*imperial*/
        if (key_of_value_in_map_vec(units_in_system, b.amount.unit, E_US_NONE) == E_US_IMPERIAL)
        {
            /*weight units*/
            if (a.amount.unit == "oz")
            {
                if (b.amount.unit == "lbs")
                {
                    a.amount.number *= OZ_TO_LBS;
                    a.amount.unit = "lbs";
                }
            }
            else if (a.amount.unit == "lbs")
            {
                if (b.amount.unit == "oz")
                {
                    a.amount.number /= OZ_TO_LBS;
                    a.amount.unit = "oz";
                }
            }
            /*volume units*/
            else if (a.amount.unit == "fl-oz")
            {
                if (b.amount.unit == "cup")
                {
                    a.amount.number *= FLOZ_TO_CUP;
                    a.amount.unit = "cup";
                }
                else if (b.amount.unit == "qt")
                {
                    a.amount.number *= FLOZ_TO_CUP * CUP_TO_QT;
                    a.amount.unit = "qt";
                }
                else if (b.amount.unit == "gal")
                {
                    a.amount.number *= FLOZ_TO_CUP * CUP_TO_QT * QT_TO_GAL;
                    a.amount.unit = "gal";
                }
            }
            else if (a.amount.unit == "cup")
            {
                if (b.amount.unit == "fl-oz")
                {
                    a.amount.number /= FLOZ_TO_CUP;
                    a.amount.unit = "fl-oz";
                }
                else if (b.amount.unit == "qt")
                {
                    a.amount.number *= CUP_TO_QT;
                    a.amount.unit = "qt";
                }
                else if (b.amount.unit == "gal")
                {
                    a.amount.number *= CUP_TO_QT * QT_TO_GAL;
                    a.amount.unit = "gal";
                }
            }
            else if (a.amount.unit == "qt")
            {
                if (b.amount.unit == "cup")
                {
                    a.amount.number /= CUP_TO_QT;
                    a.amount.unit = "cup";
                }
                else if (b.amount.unit == "fl-oz")
                {
                    a.amount.number /= FLOZ_TO_CUP / CUP_TO_QT;
                    a.amount.unit = "fl-oz";
                }
                else if (b.amount.unit == "gal")
                {
                    a.amount.number *= QT_TO_GAL;
                    a.amount.unit = "gal";
                }
            }
            else if (a.amount.unit == "gal")
            {
                if (b.amount.unit == "qt")
                {
                    a.amount.number /= QT_TO_GAL;
                    a.amount.unit = "qt";
                }
                else if (b.amount.unit == "cup")
                {
                    a.amount.number /= CUP_TO_QT / QT_TO_GAL;
                    a.amount.unit = "cup";
                }
                else if (b.amount.unit == "fl-oz")
                {
                    a.amount.number /= FLOZ_TO_CUP / CUP_TO_QT / QT_TO_GAL;
                    a.amount.unit = "fl-oz";
                }
            }
            /*length units*/
            else if (a.amount.unit == "in")
            {
                if (b.amount.unit == "ft")
                {
                    a.amount.number *= IN_TO_FT;
                    a.amount.unit = "ft";
                }
            }
            else if (a.amount.unit == "ft")
            {
                if (b.amount.unit == "in")
                {
                    a.amount.number /= IN_TO_FT;
                    a.amount.unit = "in";
                }
            }
        }
        return a;
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

    std::string AmountConverter::standardize(std::string unit)
    {
        std::string unit_lower = to_lower(unit);
        for (const auto &pair : unit_aliases)
            for (const auto &alias : pair.second)
                if (unit_lower == alias)
                    return unit_aliases[pair.first][0];

        ERR("No valid standard found for unit, " + unit);
        return "";
    }

    void AmountConverter::convert_amount(amount *amnt, epicr_unit_system tar_sys)
    {
        std::string standardized = standardize(to_lower(amnt->unit));
        amnt->unit = standardized;

        /* If the target unit system is undefined, don't modify
         * any of the ingredients */
        if (tar_sys == E_US_NONE)
            return;

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
            else if (standardized == "C")
            {
                amnt->number = C_TO_F(amnt->number);
                amnt->unit = "F";
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
            else if (standardized == "gal")
            {
                amnt->number = amnt->number / L_TO_GAL;
                amnt->unit = "L";
            }
            else if (standardized == "cup")
            {
                amnt->number = amnt->number / DL_TO_CUP;
                amnt->unit = "dl";
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
            else if (standardized == "F")
            {
                amnt->number = F_TO_C(amnt->number);
                amnt->unit = "C";
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
            convert_amount(amnt, epicr::clargs.unit_system);
        }
    }

#pragma endregion

#pragma region MandatoryFields implementation

    MandatoryFields::MandatoryFields()
    {
        error = "No error";
        has_error = false;
    }

    void MandatoryFields::check_mandatory_fields(const recipe *rcp)
    {
        if (rcp->title.empty())
        {
            error = "No title or recipe was found";
            has_error = true;
            return;
        }
        if (rcp->ingredients.empty())
        {
            error = "No ingredients was found";
            has_error = true;
            return;
        }
        if (rcp->instructions.empty())
        {
            error = "No instructions was found";
            has_error = true;
            return;
        }
    }

    void MandatoryFields::set_servings_default_value(recipe *rcp)
    {
        if (rcp->servings.count == 0 && rcp->servings.descriptor == "")
        {
            rcp->servings.count = 1;
            rcp->servings.descriptor = "servings";
        }
    }

    void MandatoryFields::visit(recipe *rcp)
    {
        check_mandatory_fields(rcp);
        set_servings_default_value(rcp);
    }

#pragma endregion

#pragma region IngredientSorter implementation
    IngredientSorter::IngredientSorter(){

    };

    void IngredientSorter::visit(recipe *rcp)
    {
        std::vector<ingredient> ordered_ingredients;

        /* get a collection of ingredients in the ingredients list */
        std::unordered_map<std::string, ingredient> existing_ingredients;
        for (const auto &ingr : rcp->ingredients)
            existing_ingredients[ingr.name] = ingr;

        for (const auto &inst : rcp->instructions)
        {
            for (const auto &ingr : inst.ingredients)
            {

                /* determine if ingredient in instruction is in ingredient list */
                if (!map_contains(existing_ingredients, ingr.name))
                    continue;

                /* determine if ingredient in instruction is already indexed */
                bool contained = false;
                for (const auto &unique : ordered_ingredients)
                {
                    if (ingr.name == unique.name)
                        contained = true;
                }

                /* otherwise, add it */
                if (!contained)
                    ordered_ingredients.push_back(existing_ingredients[ingr.name]);
            }
        }

        /* override the ingredients in the ingredient list with the sorted array */
        for (size_t i = 0; i < ordered_ingredients.size(); i++)
        {
            rcp->ingredients[i] = ordered_ingredients[i];
        }
    }
#pragma endregion

    rcp_ret visit_all(recipe *rcp)
    {
        auto ac_vis = AmountConverter();
        auto in_vis = IngredientVerifier();
        auto mf_vis = MandatoryFields();
        auto is_vis = IngredientSorter();

        mf_vis.visit(rcp);
        if (mf_vis.has_error)
            return {{}, 1, " ManFld: " + mf_vis.error};

        ac_vis.visit(rcp);
        if (ac_vis.has_error)
            return {{}, 1, " AmtCon: " + ac_vis.error};

        in_vis.visit(rcp);
        if (in_vis.has_error)
            return {{}, 1, " IngVer: " + in_vis.error};

        is_vis.visit(rcp);

        return {rcp, 0, " Visitors: No error"};
    }

}