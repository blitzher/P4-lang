#include "epicr.h"

using namespace std;

/* Remove or outcomment when not debugging */
#define DEBUG 1

int main(int argc, char **argv)
{

  cout << "Command line arguments:" << endl;

  for (int i = 0; i < argc; i++)
  {
    cout << i << ": " << argv[i] << endl;
  }

  ifstream file = epicr::open_file(argv[1]);

  {
    epicr::Lexer myLexer = epicr::Lexer(file);

    int token_count = 0;
    while (myLexer.is_ready())
    {
      epicr::epicr_token token = myLexer.next_token();
      string type = epicr::token_to_string(token.type);

      if (DEBUG)
      {
        if (token.type != epicr::ETT_BLANK && token.type != epicr::ETT_NEWLINE)
          printf("%3i %-18s: %s\n", token_count, type.c_str(), token.word.c_str());
        else
          printf("%3i %s %i\n", token_count, type.c_str(), (int)token.word.size());
      }

      token_count++;
    }
  }

  return 0;
}