
#include "./epicr.h"

using namespace std;

/* Declare struct for comparing equality between chars,
 * used in the macro below. Useful to allow `any_of` builtin
 * function to find chars in an array of chars */
struct compare
{
    char key;
    compare(char const& i) : key(i) {}

    bool operator()(char const& i)
    {
        return (i == key);
    }
};

#define CH_IS_NUM(ch) ((ch >= '0' && ch <= '9') || ch == '.')

#define CH_V_CONTAINS(ARR, VALUE) \
    (any_of(ARR.begin(), ARR.end(), compare(VALUE)))

namespace epicr
{
    vector<char> token_breakers = { ' ', '\n', 0x0d, ',', ':', '(', ')', '[', ']', '{', '}', '?', '+', '*' };

#pragma region Lexer implementation

    Lexer::Lexer(ifstream& file) : istream(file)
    {
        line_num = 1;
        token_count = 0;
        ready = file.is_open();
    }
    Lexer::Lexer(std::istream& stream) : istream(stream)
    {
        line_num = 1;
        token_count = 0;
        ready = !stream.eof();
    }

    epicr_token Lexer::next_token()
    {

        /* Check if the file stream is ended */
        if (istream.eof() || !ready)
        {
            ready = false;
            return { "EOF", ETT_EOF, token_count, line_num };
        }

        vector<char> vtoken;

        char ch;
        bool is_numeric = false;
        /* Read characters and break into tokens */
        while (istream.get(ch) && !istream.eof())
        {
            if (CH_V_CONTAINS(token_breakers, ch))
            {
                if ((ch == ' ' || ch == '\n' || ch == 0x0d) && vtoken.size() == 0)
                {
                    do
                    {
                        if (ch == 0xd)
                            istream.get(ch);

                        vtoken.push_back(ch);
                        istream.get(ch);
                    } while (!istream.eof() && (
                        ch == vtoken[0] || /* repeating space and LF endings */
                        (vtoken[0] == 0xa && ch == 0xd))); /* CRLF endings */

                    if (vtoken[0] == '\n')
                    {
                        line_num += vtoken.size();
                    }

                    /* since we encountered a non-blank, step back once */
                    if (!istream.eof()) /* can't seek when at EOF */
                                         /* seek from current position */
                        istream.seekg(-1, ios_base::cur);
                    break;
                }
                else if (vtoken.size() == 0)
                {
                    vtoken.push_back(ch);
                    break;
                }
                istream.seekg(-1, ios_base::cur);
                break;
            }

            /* if we found a non-numeric, we step back and break,
               returning the numeric token
            */
            if (is_numeric && !CH_IS_NUM(ch))
            {
                if (!istream.eof()) /* can't seek when at EOF */
                    istream.seekg(-1, ios_base::cur);
                break;
            }
            if (CH_IS_NUM(ch) && vtoken.size() == 0 && ch != '.')
            {
                is_numeric = true;
            }

            vtoken.push_back(ch);
        }

        string stoken(vtoken.begin(), vtoken.end());
        if (stoken.size() == 0)
            return next_token();

        epicr_token token{ stoken, token_type(stoken), token_count++, line_num };


        return token;
    }

    std::vector<epicr_token> Lexer::next_token(int n)
    {
        vector<epicr_token> tokens;
        for (int i = 0; i < n; i++)
        {
            tokens.push_back(next_token());
        }
        return tokens;
    }

    epicr_token Lexer::next_non_blank_token()
    {
        epicr_token next;
        do
        {
            next = next_token();
        } while (next.type == ETT_BLANK || next.type == ETT_NEWLINE);
        return next;
    }

    epicr_token_type Lexer::token_type(string stoken)
    {
        if (stoken.size() == 1)
        {
            char charr = stoken[0];
            switch (charr)
            {
            case ',':
                return ETT_COMMA;
            case ':':
                return ETT_COLON;
            case '(':
                return ETT_PARENS_OPEN;
            case ')':
                return ETT_PARENS_CLOSE;
            case '[':
                return ETT_BRACKET_OPEN;
            case ']':
                return ETT_BRACKET_CLOSE;
            case '+':
                return ETT_PLUS;
            case '*':
                return ETT_ASTERIX;
            case '?':
                return ETT_QUESTION_MARK;
            }
        }

        /* Check if the word is a blank or newline*/
        bool is_blank = true;
        bool is_newline = true;
        for (size_t i = 0; i < stoken.size(); i++)
        {
            if (is_newline && (stoken[i] != '\n' && stoken[i] != 0xd))
                is_newline = false;
            if (is_blank && stoken[i] != ' ')
            {
                is_blank = false;
            }
            if (!is_blank && !is_newline)
                break;
        }
        if (is_blank)
            return ETT_BLANK;
        if (is_newline)
            return ETT_NEWLINE;

        /* Check if the word is a numeric */
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
            if (!CH_IS_NUM(ch))
            {
                is_numeric = false;
                break;
            }
        }
        return (is_numeric) ? ETT_NUMBER : ETT_WORD;
    }

    epicr_token Lexer::peek_token(int amnt)
    {
        size_t offset = 0;
        uint line_offset = 0;
        epicr_token token;
        for (int i = 0; i < amnt; i++)
        {
            token = next_token();
            offset += token.word.size();
            if (token.type == ETT_NEWLINE)
                line_offset += token.word.size();
        }
        token_count -= amnt;
        line_num -= line_offset;
        /* retract the header by the width of the read tokens */
        if (!istream.eof())
            istream.seekg(-offset, ios_base::cur);
        return token;
    }

    epicr_token Lexer::peek_token()
    {
        return peek_token(1);
    }

    epicr_token Lexer::peek_non_blank_token(int amnt)
    {
        int non_blank_count = 0;
        epicr_token token;
        size_t offset = 0;
        size_t line_offset = 0;

        while (non_blank_count < amnt)
        {
            token = next_token();
            offset += token.word.size();

            if (token.type == ETT_NEWLINE)
                line_offset += token.word.size();

            if (token.type != ETT_BLANK && token.type != ETT_NEWLINE)
                non_blank_count++;
        }

        token_count -= amnt;
        line_num -= line_offset;
        if (!istream.eof())
        /* seekg is used seek an arbitary position in the file. 
           offset is the amount of chars read since start. 
           offset makes it possible to backtrack to the place we were at before and it is used in Lexer::peek_token
           ios_base is the input output stream
           cur is the current position in stream   */
            istream.seekg(-offset, ios_base::cur);

        return token;
    }

    epicr_token Lexer::peek_non_blank_token()
    {
        return peek_non_blank_token(1);
    }

    bool Lexer::is_ready()
    {
        return ready;
    }

#pragma endregion

}