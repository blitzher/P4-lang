#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <map>
#include <limits>
#include <cmath>
#include <sstream>
#include <filesystem>
#include <csetjmp>

#pragma region Debug macros

#define HAS_PLUS 1     // 0b0000 0001
#define HAS_ASTERIX 2  // 0b0000 0010
#define HAS_QMARK 4    // 0b0000 0100
#define ASSUME_1_NUM 8 // 0b0000 1000
#define ASSUME_REST 16 // 0b0001 0000

#pragma endregion

namespace epicr
{

    typedef char ingredient_arg;
    typedef unsigned int uint;
#pragma region Recipe Data

    typedef enum epicr_html_style
    {
        E_HTML_BASIC,
        E_HTML_FANCY
    } epicr_html_style;

    typedef struct amount_s
    {
        double number;
        bool is_relative_amount;
        std::string relative_amount;
        std::string unit;
        bool is_uncountable;
    } amount;

    typedef struct ingredient_s
    {
        std::string name;
        amount amount;
        bool is_ingredient_ref;
        bool is_optional;
    } ingredient;

    typedef struct instruction_word_s
    {
        std::string spelling;
        bool is_amount;
        amount value;
    } instruction_word;

    typedef struct instruction_s
    {
        std::vector<ingredient> ingredients;
        std::vector<std::string> kitchenware;
        std::vector<ingredient> yields;
        std::vector<instruction_word> body;
    } instruction;

    typedef struct servings_s
    {
        int count;
        std::string descriptor;
    } servings;

    typedef struct time_s
    {
        std::string prep_time;
        std::string cook_time;
        std::string total_time;
    } time;

    typedef struct recipe_s
    {
        std::string title;
        std::string description;
        servings servings;
        time time;
        std::vector<std::string> kitchenware;
        std::vector<ingredient> nutrients;
        std::vector<std::string> tags;
        std::vector<ingredient> ingredients;
        std::vector<instruction> instructions;
    } recipe;
#pragma endregion

    enum epicr_token_type
    {
        E_TT_WORD,
        E_TT_COLON,
        E_TT_COMMA,
        E_TT_NUMBER,
        E_TT_PARENS_OPEN,   /* ( */
        E_TT_PARENS_CLOSE,  /* ) */
        E_TT_BRACKET_OPEN,  /* [ */
        E_TT_BRACKET_CLOSE, /* ] */
        E_TT_PLUS,          /* + */
        E_TT_ASTERIX,       /* * */
        E_TT_QUESTION_MARK, /* ? */
        E_TT_BLANK,
        E_TT_NEWLINE,
        E_TT_EOF,
    };

    enum epicr_unit_type
    {
        E_UT_WEIGHT,
        E_UT_VOLUME,
        E_UT_LENGTH,
        E_UT_TEMPERATURE
    };

    enum epicr_unit_system
    {
        E_US_NONE,
        E_US_METRIC,
        E_US_IMPERIAL
    };

    typedef struct cmd_args_s
    {
        std::string input_filepath;
        epicr_html_style choosen_style;
        std::string output_filepath;
        epicr_unit_system unit_system;

    } cmd_args;

    extern cmd_args clargs;

    extern std::map<epicr_unit_type, std::vector<std::string>> units_in_type;
    extern std::map<std::string, std::vector<std::string>> unit_aliases;
    extern std::map<epicr_unit_system, std::vector<std::string>> units_in_system;

    std::string token_to_string(epicr_token_type);

    typedef struct epicr_token_s
    {
        std::string word;
        epicr_token_type type;
        uint uid;
        uint line;

    } epicr_token;

    class Lexer
    {
    private:
        std::istream &istream;
        uint token_count;
        uint line_num;
        bool ready;
        bool can_return_pre_eof_token;
        epicr_token pre_eof_token;
        bool is_peaking;

    public:
        Lexer();
        Lexer(std::ifstream &file);
        Lexer(std::istream &file);

        /* Return whether or not the Lexer is ready to yield tokens */
        bool DEBUG_MODE;
        bool is_ready();

        /* Return the next token, and moving the reader */
        epicr_token next_token();
        /* Return the next `n` tokens as an array */
        std::vector<epicr_token> next_token(int n);
        /* Return the next non blank, non new line token */
        epicr_token next_non_blank_token();
        epicr_token_type token_type(std::string stoken);
        /* Peek the next token */
        epicr_token peek_token();
        /* Peek the `n` 'th token. Undefined for `n=0` */
        epicr_token peek_token(int n);
        /* Peek the next non-blank token */
        epicr_token peek_non_blank_token();
        /* Peek the `n` 'th non-blank token. Undefined for `n=0`*/
        epicr_token peek_non_blank_token(int n);
    };

    class Parser
    {
    private:
        bool silent;
        Lexer *lexer;
        void ParseTitle(recipe *);
        void ParseDescription(recipe *);
        void ParseServings(recipe *);
        void ParseNutrients(recipe *);
        void ParseIngredients(recipe *);
        void ParseKitchenware(recipe *);
        void ParseTags(recipe *);
        void ParseTime(recipe *);
        void ParseInstructions(recipe *);
        void ParseInstructionHeaderWith(instruction *single_instruction);
        void ParseInstructionHeaderUsing(instruction *single_instruction);
        void ParseInstructionBody(instruction *current_instruction);
        void ParseInstructionYield(instruction *single_instruction);
        /* Read an ingredient from the current position */
        ingredient ReadIngredient(ingredient_arg);
        /*Read words and blanks from the current position, then returns the word, with right spaces stripped
        accepts a boolean as input stating whether or not it can read numbers as well*/
        std::string ReadWords(bool, bool);
        /* Read an amount from the current position */
        amount ReadAmount(ingredient_arg arg);
        /*predicate used in the readWords function to determine the allowed token types that can be read*/
        bool ReadWordsPredicate(epicr_token_type, bool, bool);
        /*reads the seperator (comma) if there are more elements in the field. Otherwise stay at the start of the next field
        if no seperator or next field is read, an error is thrown*/
        void ReadSeperatorOrWaitAtNextField(std::string);

        epicr_token ctoken;
        epicr_token utoken;

    public:
        bool DEBUG_MODE;
        bool has_error;
        std::string original_amount;
        std::string error;
        epicr_token error_token;
        recipe Parse();
        void silence(bool);
        Parser(Lexer *lexer_r);
        ~Parser();
    };

    namespace visitor
    {

        class Visitor
        {
        private:
            // cmd_args
        public:
            std::string error;
            bool has_error;
            void visit(recipe *);
        };

        class IngredientVerifier : public Visitor
        {
        private:
            std::unordered_map<std::string, ingredient> symbols;
            std::unordered_map<std::string, ingredient> original_symbols;
            bool ingredients_compatible(ingredient a, ingredient b);

        public:
            void visit(recipe *);
            IngredientVerifier();
        };

        class AmountConverter : public Visitor
        {
        private:
            bool is_convertable(std::string);
            std::string standardize(std::string);
            void scale_amount(amount *amnt, epicr_unit_system system);

        public:
            void visit(recipe *);
            AmountConverter();
        };

        const double G_TO_OZ = 0.035;
        const double KG_TO_LBS = 2.2046;
        const double L_TO_QT = 1.057;
        const double ML_TO_FLOZ = 0.0338;
        const double DL_TO_FLOZ = 3.38;
        const double M_TO_FEET = 3.28;
        const double CM_TO_INCH = 0.39370079;
        const double MM_TO_INCH = 0.039;

        inline double C_TO_F(double deg)
        {
            return deg * 9 / 5 + 32;
        }
        inline double F_TO_C(double deg)
        {
            return (deg - 32) * 5 / 9;
        }

    }

    void compress(std::string filepath);
    void decompress(std::string filepath);
    /*returns a new string with all chars in the input string in lowercase*/
    std::string to_lower(std::string);
    /*returns a new string where all types of spaces to right is stripped from the input string */
    std::string strip_spaces_right(std::string);
    /* converts a double to a string - also rounds to the nearest 2 decimals */
    std::string double_to_string(double);
    /* converts an amount to a printable string */
    std::string amount_to_string(amount);
    /*return whether or not an ingredient with that name exists in the unordered map*/
    bool ingredient_in_map(std::string, std::unordered_map<std::string, ingredient>);
    /* Print the contents of a token to stdout */
    void print_token(epicr_token);

    std::ifstream open_file(std::string filename);

    bool generate_html(recipe, std::string filename);

    typedef struct parse_ret_s
    {
        recipe recipe;
        bool has_err;
        std::string err;
    } parse_ret;

    parse_ret parse_recipe(std::string filename);
    parse_ret parse_recipe(cmd_args);
    parse_ret parse_recipe_silent(std::string filename);
    parse_ret parse_string_silent(std::string str);

    /* Command line argument related declarations */
    void parse_cmd_args(int argc, char **argv);
}