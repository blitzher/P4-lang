

namespace epicr
{

    class Lexer
    {
    private:
        /* data */
    public:
        Lexer(std::string filename);
        ~Lexer();
        std::string next_token();
    };

}