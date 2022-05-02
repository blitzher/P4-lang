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

#define EOF_TOKEN_OBJECT { "EOF", E_TT_EOF, token_count, line_num }

namespace epicr
{
    vector<char> token_breakers = { ' ', '\n', 0x0d, ',', ':', '(', ')', '[', ']', '{', '}', '?', '+', '*' };

#pragma region Lexer implementation

    void Lexer::init() {
        line_num = 1;
        token_count = 0;
        can_return_pre_eof_token = false;
        pre_eof_index = 0;
        is_peaking = false;
    }

    Lexer::Lexer(ifstream& file) : istream(file)
    {
        ready = file.is_open();
        init();
    }

    Lexer::Lexer(std::istream& stream) : istream(stream)
    {
        ready = !stream.eof();
        init();
    }

    epicr_token Lexer::next_token()
    {
        // std::cout << can_return_pre_eof_token << ready << "getting token\n";
        // print_token(pre_eof_token);

        /* Check if the file stream is ended */
        if (!ready)
        {
            return EOF_TOKEN_OBJECT;
        }
        else if (istream.eof() && can_return_pre_eof_token)
        {
            if (pre_eof_index < pre_eof_tokens.size())
            {
                epicr_token pre_eof_token = pre_eof_tokens[pre_eof_index++];
                return pre_eof_token;
            }
            else
            {
                can_return_pre_eof_token = false;
                ready = false;
                return EOF_TOKEN_OBJECT;
            }

        }
        else if (istream.eof())
        {
            ready = false;
            return EOF_TOKEN_OBJECT;
        }

        vector<char> vtoken;

        char ch;
        bool is_numeric = false;
        /* Read characters and break into tokens */
        while (istream.get(ch) && !istream.eof())
        {
            /* interpret tabs as spaces */
            if (ch == '\t') ch = ' ';

            if (CH_V_CONTAINS(token_breakers, ch))
            {
                /*                              ascii value 13=        if token size
                          space       newline   carriage return           is 0 */
                if ((ch == ' ' || ch == '\n' || ch == 0x0d) && vtoken.size() == 0)
                {
                    do
                    {
                        if (ch == 0xd)
                        {
                            istream.get(ch); /* gets the next char in the stream */
                        }
                        vtoken.push_back(ch);
                        istream.get(ch);
                    } while (!istream.eof() && (ch == vtoken[0] ||                 /* repeating space and LF endings */
                        (vtoken[0] == 0xa && ch == 0xd))); /* CRLF endings (0xa is newline) [Windows] */

                    if (vtoken[0] == '\n')
                    {
                        line_num += vtoken.size();
                    }

                    /* since we encountered a non-blank, step back once */
                    if (!istream.eof()) /* can't seek when at EOF */
                    {
                        istream.seekg(-1, ios_base::cur); /* seek from current position */
                    }
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

        /* store most recent token, in case we were peaking, and hit eof */
        if (is_peaking)
        {
            pre_eof_tokens.push_back(token);
            if (istream.eof())
                can_return_pre_eof_token = true;
        }
        else if (!pre_eof_tokens.empty()) {
            pre_eof_tokens.clear();
        }
        else if (istream.eof())
        {
            ready = false;
        }
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
        } while (next.type == E_TT_BLANK || next.type == E_TT_NEWLINE);
        return next;
    }

    epicr_token_type Lexer::token_type(string stoken)
    {
        if (stoken.size() == 1)
        {
            switch (stoken[0])
            {
            case ',':
                return E_TT_COMMA;
            case ':':
                return E_TT_COLON;
            case '(':
                return E_TT_PARENS_OPEN;
            case ')':
                return E_TT_PARENS_CLOSE;
            case '[':
                return E_TT_BRACKET_OPEN;
            case ']':
                return E_TT_BRACKET_CLOSE;
            case '+':
                return E_TT_PLUS;
            case '*':
                return E_TT_ASTERIX;
            case '?':
                return E_TT_QUESTION_MARK;
            }
        }

        /* Check if the word is a blank or newline*/
        if (stoken[0] == ' ')
            return E_TT_BLANK;
        if (stoken[0] == '\n')
            return E_TT_NEWLINE;

        /* Check if the word is a numeric */
        bool is_numeric = true;
        bool found_comma = false;
        char ch;
        for (size_t i = 0; i < stoken.size(); i++)
        {
            ch = stoken[i];
            if (ch == '.')
            {
                if (!found_comma)
                    found_comma = true;
                else
                {
                    is_numeric = false;
                    break;
                }
            }

            /* If the word is not 0-9 and not '.', it isn't a number */
            if (!CH_IS_NUM(ch))
            {
                is_numeric = false;
                break;
            }
        }
        return (is_numeric) ? E_TT_NUMBER : E_TT_WORD;
    }

    epicr_token Lexer::peek_token(int amnt)
    {
        size_t offset = 0;
        uint line_offset = 0;
        epicr_token token;
        is_peaking = true;
        int before_peek_eof_index = pre_eof_index;


        for (int i = 0; i < amnt; i++)
        {
            token = next_token();
            offset += token.word.size();
            if (token.type == E_TT_NEWLINE)
                line_offset += token.word.size();
        }
        token_count -= amnt;
        line_num -= line_offset;
        /* retract the header by the width of the read tokens */

        // printf("can_return...:%i eof_flag:%i ready:%i\n", can_return_pre_eof_token, istream.eof(), ready);
        pre_eof_index = before_peek_eof_index;
        is_peaking = false;
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
        is_peaking = true;

        while (non_blank_count < amnt)
        {
            token = next_token();
            offset += token.word.size();

            if (token.type == E_TT_NEWLINE)
                line_offset += token.word.size();

            if (token.type != E_TT_BLANK && token.type != E_TT_NEWLINE)
                non_blank_count++;
        }

        token_count -= amnt;
        line_num -= line_offset;
        is_peaking = false;
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