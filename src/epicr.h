#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <limits>

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

    typedef struct amount_s
    {
        double number;
        bool isRelativeAmount;
        std::string relativeAmount;
        std::string unit;
        bool isUncountable;
    } amount;

    typedef struct ingredient_s
    {
        std::string name;
        amount amount;
        bool isIngredientRef;
        bool isOptional;
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
        ETT_WORD,
        ETT_COLON,
        ETT_COMMA,
        ETT_NUMBER,
        ETT_PARENS_OPEN,   /* ( */
        ETT_PARENS_CLOSE,  /* ) */
        ETT_BRACKET_OPEN,  /* [ */
        ETT_BRACKET_CLOSE, /* ] */
        ETT_CURLY_OPEN,    /* { */
        ETT_CURLY_CLOSE,   /* } */
        ETT_PLUS,          /* + */
        ETT_ASTERIX,       /* * */
        ETT_QUESTION_MARK, /* ? */
        ETT_BLANK,
        ETT_NEWLINE,
        ETT_EOF,
    };

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
        void ParseInstructionHeaderWith(instruction *singleInstruction);
        void ParseInstructionHeaderUsing(instruction *singleInstruction);
        void ParseInstructionBody(instruction *currentInstruction);
        void ParseInstructionYield(instruction *singleInstruction);
        /* Read an ingredient from the current position */
        ingredient ReadIngredient(ingredient_arg);
        /* Read an amount from the current position */
        amount ReadAmount(ingredient_arg);
        /*Read words and blanks from the current position, then returns the word, with right spaces stripped
        accepts a boolean as input stating whether or not it can read numbers as well*/
        std::string ReadWords(bool);
        /*reads the seperator (comma) if there are more elements in the field. Otherwise stay at the start of the next field
        returns 1 if something went wrong, otherwise returns 0*/
        int ReadSeperatorOrWaitAtNextField();
        epicr_token ctoken;
        epicr_token utoken;

    public:
        bool DEBUG_MODE;
        bool error;
        std::string error_message;
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
        public:
            std::string error;
            bool has_error;
            void visit(recipe);
        };

        class IngredientVerifier : public Visitor
        {
        private:
            std::unordered_map<std::string, ingredient> symbols;
            std::unordered_map<std::string, ingredient> original_symbols;
            bool ingredients_compatible(ingredient a, ingredient b);

        public:
            IngredientVerifier();
            void visit(recipe);
        };
    }

    void compress(std::string filepath);
    void decompress(std::string filepath);
    /*returns a new string with all chars in the input string in lowercase*/
    std::string to_lower(std::string);
    /*returns a new string where all types of spaces to right is stripped from the input string */
    std::string strip_spaces_right(std::string);

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
    parse_ret parse_recipe_silent(std::string filename);
}