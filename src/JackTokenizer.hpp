#ifndef _JACK_TOKENIZER_
#define _JACK_TOKENIZER_
#include "Common.hpp"
class JackTokenizer {
public:
	JackTokenizer(string inputFile);
	~JackTokenizer();

	bool hasMoreTokens();

	enum TokenType tokenType();

	void advance();

	enum KeyWord keyWord();

	string symbol();
	string identifier();
	string intVal();
	string stringVal();

private:
	ifstream inputStream;
	enum KeyWord thisKeyWord;
	enum TokenType thisTokenType;
	string thisValue;
	char buffer;

	bool isKeyWord(const string& token);
	bool isNumber(char ch);
	bool isAlpha(char ch);
};


#endif