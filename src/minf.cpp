#include "./epicr.h"

using namespace std;

/*
	Store some known data and their sizes in memory,
	and omit the headers, such as "title:",
	since all recipes will contain these.
 */
typedef struct min_rcp_format_s
{
	int title_length;
	string title_string;
	int description_length;
	string description_string;
	int amount_length;
	string amount_string;
	int tags_length;
	string tags_string;

} min_rcp_format;

void compress(string filepath)
{
	/* Perform compression with huffman */
}

void decompress(string filepath)
{
	/* Decompress with huffman */
}
