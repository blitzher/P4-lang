
#include "./epicr.h"

using namespace std;

struct compare
{
    char key;
    compare(char const &i) : key(i) {}

    bool operator()(char const &i)
    {
        return (i == key);
    }
};

#define CH_V_CONTAINS(ARR, VALUE) \
    (any_of(ARR.begin(), ARR.end(), compare(VALUE)))

namespace epicr
{
    vector<char> brackets = {'(', ')', '[', ']', '{', '}'};
    vector<char> token_breakers = {' ', ',', ':', '(', ')', '[', ']', '{', '}'};

#pragma region Lexer implementation

    Lexer::Lexer(ifstream &file) : istream(file)
    {
        cout << "Lexer says hi!" << endl;

        // try to open the file
        ready = istream.is_open();
    }

    epicr_token Lexer::next_token()
    {
        /* Handle peeking */
        if (token_peeklog.size() > 0)
        {
            string stoken = token_peeklog.front();
            token_peeklog.pop();
            return {stoken, token_type(stoken)};
        }

        /* Check if the file stream is ended */
        if (istream.eof())
        {
            ready = false;
            return {"EOF", ETT_EOF};
        }

        vector<char> vtoken;

        char ch;
        /* Read characters and break into tokens */
        while (istream.get(ch))
        {
            /* we don't want long runs of blanks */
            while (!istream.eof() && vtoken.size() == 0 && (ch == ' ' || ch == '\n'))
            {
                istream.get(ch);
            }

            if (CH_V_CONTAINS(token_breakers, ch))
            {
                if (ch != ' ' && ch != '\n')
                {
                    string breaker_token;
                    breaker_token.push_back(ch);
                    token_peeklog.push(breaker_token);
                }
                break;
            }

            if (ch != '\n' && ch != 0)
                vtoken.push_back(ch);
        }

        string stoken(vtoken.begin(), vtoken.end());
        if (stoken.size() == 0)
        {
            return next_token();
        }

        return {stoken, token_type(stoken)};
    }

    epicr_token_type Lexer::token_type(string stoken)
    {

        if (stoken == "(" || stoken == "[" || stoken == "{")

            return ETT_BRACKET_OPEN;

        else if (stoken == ")" || stoken == "]" || stoken == "}")
            return ETT_BRACKET_CLOSE;

        bool is_numeric = true;
        char ch;
        for (size_t i = 0; i < stoken.size(); i++)
        {
            ch = stoken[i];
            if (i == 0 && ch == '.')
            {
                is_numeric = false;
                break;
            }
            /* If the word is not 0-9 and not '.', it isn't a number */
            if ((ch < 48 || ch > 57) && ch != '.')
            {
                is_numeric = false;
                break;
            }
        }
        return (is_numeric) ? ETT_NUMBER : ETT_WORD;
    }

    bool Lexer::is_ready()
    {
        return ready;
    }

    /* Destructure, close the input stream */
    Lexer::~Lexer()
    {
        istream.close();
    }

#pragma endregion

}