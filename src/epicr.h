#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <fstream>
#include <algorithm>

namespace epicr
{

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