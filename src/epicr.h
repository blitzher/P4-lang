#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <fstream>
#include <algorithm>

#pragma region Debug macros

#ifdef DEBUG
#define DEBUG_ERR(x) (std::cerr << (x))
#define DEBUG_OUT(x) (std::cout << (x))
#define DEBUG_OUT_F(x) (printf(x))
#else
#define DEBUG_ERR(x)
#define DEBUG_OUT(x)
//... etc
#endif

#pragma endregion

namespace epicr
{
    typedef unsigned int uint;
#pragma region Recipe Data
    typedef struct ingredient_s
    {
        std::string name;
        double amount;
        std::string relativeAmount;
        std::string unit;
        bool isUncountable;
        bool isIngredientRef;
        bool isOptional;
    } ingredient;

    typedef struct instruction_word_s
    {
        std::string word;
        bool is_amount;
        double amount;
        std::string unit;

        bool is_ingredient_ref;
    } instruction_word;

    typedef struct instruction_s
    {
        std::vector<ingredient> ingredients;
        std::vector<std::string> kitchenware;
        std::vector<ingredient> yields;
        std::vector<instruction_word> body;
    } instruction;

    typedef struct amount_s
    {
        int count;
        std::string descriptor;
    } amount_s;

    typedef struct recipe_s
    {
        std::string title;
        std::string description;
        amount_s amount;
        std::string time; /* expand implementation */
        std::vector<std::string> kitchenware;
        std::vector<ingredient> nutrients;
        std::vector<std::string> tags;
        std::vector<ingredient> ingredients;   /* missing SOME implementation */
        std::vector<instruction> instructions; /* missing implementation */
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
        Lexer *lexer;
        void ParseTitle(recipe *);
        void ParseDescription(recipe *);
        void ParseAmount(recipe *);
        void ParseNutrients(recipe *);
        void ParseIngredients(recipe *);
        void ParseKitchenware(recipe *);
        void ParseTags(recipe *);
        void ParseTime(recipe *);
        void ParseInstructions(recipe *);
        void ParseInstructionHeaderWith(instruction *singleInstruction);
        void ParseInstructionHeaderUsing(instruction *singleInstruction);
        void ParseInstructionBody(recipe *rcp,instruction *currentInstruction);
        void ParseInstructionYield(instruction *singleInstruction);
        std::string ReadWords();
        void ReadRelativeAmount(ingredient *currentIngredient);
        void ReadAmount(ingredient *currentIngredient);
        epicr_token ctoken;
        epicr_token utoken;

    public:
        bool error;
        std::string error_message;
        epicr_token error_token;
        recipe Parse();
        Parser(Lexer *lexer_r);
        ~Parser();
    };

    void compress(std::string filepath);
    void decompress(std::string filepath);

    std::string to_lower(std::string);

    /* Print the contents of a token to stdout */
    void print_token(epicr_token);

    std::ifstream open_file(std::string filename);

    bool generate_html(recipe, std::string filename);
}