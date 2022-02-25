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

    enum epicr_token_type
    {
        ETT_WORD,
        ETT_COLON,
        ETT_NUMBER,
        ETT_BRACKET_OPEN,
        ETT_BRACKET_CLOSE,
        ETT_BLANK,
        ETT_NEWLINE,
        ETT_EOF,
    };

    std::string token_to_string(epicr_token_type);

    typedef struct epicr_token_s
    {
        std::string word;
        epicr_token_type type;

    } epicr_token;

    class Lexer
    {
    private:
        std::istream &istream;
        std::queue<std::string> token_peeklog;
        bool ready;

    public:
        Lexer();
        Lexer(std::ifstream &file);
        Lexer(std::istream &file);

        /* Return whether or not the Lexer is ready to yield tokens */
        bool is_ready();

        /* Return the next token, and moving the reader */
        epicr_token next_token();
        /* Return the next non blank, non new line token */
        epicr_token next_non_blank_token();
        epicr_token_type token_type(std::string stoken);
        /* Return the next token, without dropping it */
        epicr_token peek();
    };

    class Parser
    {
    private:
        Lexer *lexer;

    public:
        bool error;
        std::string error_message;
        Parser(Lexer *lexer_r);
        ~Parser();
    };

    void compress(std::string filepath);
    void decompress(std::string filepath);

    std::ifstream open_file(std::string filename);
}