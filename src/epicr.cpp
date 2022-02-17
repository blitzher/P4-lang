

#include "./epicr.h"

using namespace std;

namespace epicr
{
    
#pragma region Lexer class implementation

    Lexer::Lexer() :istream((ifstream*)nullptr)
    {}

    Lexer::Lexer(ifstream& file) : istream(file)
    {
        std::cout << "Lexer says hi!" << std::endl;

        // try to open the file
        ready = istream.is_open();
    }

    string Lexer::next_token()
    {
        if (token_backlog.size() > 0) {
            string stoken = token_backlog.front();
            token_backlog.pop();
            return stoken;
        }

        vector<char> vtoken;

        char ch;        

        /* Read characters */
        while (istream.get(ch)) {
            vtoken.push_back(ch);

        }

        string stoken(vtoken.begin(), vtoken.end());
        return stoken;
    }

    string Lexer::peek()
    {
        if (token_backlog.size() > 0) {

        }
        return "";
    }

    /* Destructure, close the input stream */
    Lexer::~Lexer()
    {
        istream.close();
    }
#pragma endregion

}