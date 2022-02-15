#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using namespace std;

namespace epicr
{

    class Lexer
    {
    private:
        /* data */
    public:
        Lexer(string filename);
        ~Lexer();
        string next_token();
    };

    Lexer::Lexer(string filename)
    {
        std::cout << "Lexer says hi!" << std::endl;

        // try to open the file
        ifstream istream{filename};

        char ch;
        int chcount = 0;

        if (istream.is_open())
        {
            while (istream.get(ch))
            {
                cout << ch;
                chcount++;
            }
            istream.close();
            cout << endl;
        }
        else
        {
            cout << "Unable to open " << filename << endl;
        }

        cout << "Read " << chcount << " characters" << endl;
    }

    string Lexer::next_token()
    {
        vector<char> token;

        return "Yo";
    }

    Lexer::~Lexer()
    {
    }

}