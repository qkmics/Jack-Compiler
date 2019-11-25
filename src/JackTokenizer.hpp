#ifndef _JACK_TOKENIZER_
#define _JACK_TOKENIZER_
#include "Common.hpp"
class JackTokenizer {
public:
	JackTokenizer(string inputFile);
	~JackTokenizer();

	bool hasMoreTokens();

	// advance to get the next token
	void advance();

	// return type of currenct token
	enum class TokenType tokenType();

	// return value of current token
	enum class KeyWord keyWord();
	string symbol();
	string identifier();
	string intVal();
	string stringVal();

private:
	ifstream inputStream;

	enum class TokenType thisTokenType;

	// if thisTokenType is TokenType::KEYWORD, thisKeyWord is valid
	// else thisValue is valid
	enum class KeyWord thisKeyWord;
	string thisValue;
	
	// store the begining of the next token
	char buffer;

	// tools
	bool isKeyWord(const string& token);
	bool isNumber(char ch);
	bool isAlpha(char ch);
};


#endif