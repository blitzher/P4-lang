#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <fstream>
#include <algorithm>

namespace epicr
{

    enum epicr_token_type
    {
        WORD,
        BRACKET_OPEN,
        BRACKET_CLOSE,
        NUMBER,

    };

    typedef struct epicr_token_s
    {
        std::string s;

    } epicr_token;

    class Lexer
    {
    private:
        std::ifstream &istream;
        std::queue<std::string> token_peeklog;
        bool ready;

    public:
        Lexer();
        Lexer(std::ifstream &file);
        ~Lexer();

        bool is_ready();
        /* Return whether or not the Lexer is ready to yield tokens */

        /* Return the next token, and moving the reader */
        std::string next_token();
        /* Return the next token, without dropping it */
        std::string peek();
    };

    std::ifstream open_file(std::string filename);
}