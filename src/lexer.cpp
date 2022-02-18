
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

    string Lexer::next_token()
    {
        /* Handle peeking */
        if (token_peeklog.size() > 0)
        {
            string stoken = token_peeklog.front();
            token_peeklog.pop();
            return stoken;
        }

        /* Check if the file stream is ended */
        if (istream.eof())
        {
            ready = false;
            return "EOF";
        }

        vector<char> vtoken;

        char ch;
        /* Read characters and break into tokens */
        while (istream.get(ch))
        {
            /* we don't want long runs of blanks */
            while (vtoken.size() == 0 && (ch == ' ' || ch == '\n'))
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

        return stoken;
    }

    bool Lexer::is_ready()
    {
        return ready;
    }

    string Lexer::peek()
    {
        if (token_peeklog.size() > 0)
        {
        }
        return "";
    }

    /* Destructure, close the input stream */
    Lexer::~Lexer()
    {
        istream.close();
    }

#pragma endregion

    ifstream open_file(string filename)
    {
        ifstream file{filename};

        if (!file.is_open())
        {
            cout << "File " << filename << " could not be opened!" << endl;
        }
        else
        {
            cout << "Opened file " << filename << endl;
        }

        return file;
    }

}