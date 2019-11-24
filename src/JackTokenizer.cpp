#include "JackTokenizer.hpp"

/* Open the input stream and get ready to tokenize it 
*/
JackTokenizer::JackTokenizer(string inputFile) {
	inputStream.open(inputFile);

	thisTokenType = KEYWORD;
	thisKeyWord = LET;
	thisValue = "";
	buffer = ' ';
	advance();
}

JackTokenizer::~JackTokenizer() {
	inputStream.close();
}

bool JackTokenizer::hasMoreTokens() {
	return !inputStream.eof();
}

enum TokenType JackTokenizer::tokenType() {
	return thisTokenType;
}

bool JackTokenizer::isNumber(char ch) {
	return (ch >= '0' && ch <= '9');
}
bool JackTokenizer::isAlpha(char ch) {
	return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

void JackTokenizer::advance() {
	string nextToken = "";
	char nextChar = buffer;
	while (nextChar == ' ' || nextChar == '\t' || nextChar == '\n' && hasMoreTokens()) {
		inputStream.get(nextChar);
	}
	if (hasMoreTokens()) {
		if (nextChar == '/') {
			// comment
			
			nextToken.push_back(nextChar);
			inputStream.get(nextChar);
			if (nextChar == '/') {
				// //

				nextToken.push_back(nextChar);
				while (nextChar != '\n') {
					inputStream.get(nextChar);
					//cout << nextChar;
				}
				advance();
			}
			else if (nextChar == '*') {
				// /**/

				nextToken.push_back(nextChar);
				while (true) {
					while (nextChar != '*') {
						inputStream.get(nextChar);
					}
					while (nextChar == '*') {
						// point to the last *

						inputStream.get(nextChar);
					}
					if (nextChar == '/') {
						//end of comment
						break;
					}
					else {
						inputStream.get(nextChar);
					}
				}
				advance();
			}
			else {
				thisTokenType = SYMBOL;
				thisValue = nextToken;
				buffer = nextChar;
			}
		}
		else
		{
			if (isNumber(nextChar)) {
				// int const

				while (isNumber(nextChar)) {
					nextToken.push_back(nextChar);
					inputStream.get(nextChar);
				}

				thisTokenType = INT_CONSTANT;
				thisValue = nextToken;
			}
			else if (nextChar == '"') {
				// string const

				inputStream.get(nextChar);
				while (nextChar != '"') {
					nextToken.push_back(nextChar);
					inputStream.get(nextChar);
				}

				// this line is vital, because current "nextChar" is "
				inputStream.get(nextChar);

				thisTokenType = STRING_CONSTANT;
				thisValue = nextToken;
			}
			else if (isAlpha(nextChar) || nextChar == '_') {
				// keyword or identifier 

				while (isAlpha(nextChar) || nextChar == '_' || isNumber(nextChar)) {
					nextToken.push_back(nextChar);
					inputStream.get(nextChar);
				}

				if (isKeyWord(nextToken))
				{
					//keyword 

					thisTokenType = KEYWORD;
				}
				else
				{
					//identifier

					thisValue = nextToken;
					thisTokenType = IDENTIFIER;
				}
			}
			else {
				// symbol 

				nextToken.push_back(nextChar);
				inputStream.get(nextChar);

				thisTokenType = SYMBOL;
				thisValue = nextToken;
			}

			buffer = nextChar;
		}
	}
}
 bool JackTokenizer::isKeyWord(const string& token) {
	for (int index = 0; index < Keywords.size(); index++) {
		if (token == Keywords[index]) {
			thisKeyWord = (enum KeyWord)index;
			return true;
		}
	}
	return false;
}
enum KeyWord JackTokenizer::keyWord() {
	return thisKeyWord;
}
string JackTokenizer::symbol() {
	return thisValue;
}
string JackTokenizer::identifier() {
	return thisValue;
}
string JackTokenizer::intVal() {
	return thisValue;
}
string JackTokenizer::stringVal() {
	return thisValue;
}