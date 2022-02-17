#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <queue>

namespace epicr
{

    class Lexer
    {
    private:
        std::ifstream& istream;
        std::queue<std::string> token_backlog;
    public:
        Lexer();
        Lexer(std::ifstream &file);
        ~Lexer();

        bool open_file(std::string filename);
        /* Return whether or not the Lexer is ready to yield tokens */
        bool ready;
        /* Return the next token, and moving the reader */
        std::string next_token();
        /* Return the next token, without dropping it */
        std::string peek();
    };

}