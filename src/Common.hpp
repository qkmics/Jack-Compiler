#ifndef _COMMON_
#define _COMMON_
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <memory>

using namespace std;

enum class TokenType { KEYWORD, SYMBOL, INT_CONSTANT, STRING_CONSTANT, IDENTIFIER };

enum class KeyWord {
	CLASS, CONSTRUCTOR, FUNCTION, METHOD, FIELD
	, STATIC, VAR, INT, CHAR, BOOLEAN
	, VOID, TRUE, FALSE, NULL_PTR, THIS
	, LET, DO, IF, ELSE, WHILE, RETURN
};

enum class IdentifierKind { STATIC, FIELD, ARG, VAR, UNKNOWN };

enum class Segment { CONST, ARG, LOCAL, STATIC, THIS, THAT, POINTER, TEMP };

const vector<string> SegmentString = {
	"constant","argument","local","static"
	,"this","that","pointer","temp"
};

enum class Command { ADD, SUB, NEG, EQ, GT, LT, AND, OR, NOT };

const vector<string> CommandString = {
	"add","sub","neg","eq"
	,"gt","lt","and","or","not"
};

const vector<string> KeyWordString = {
	"class","constructor","function","method","field"
	,"static","var","int","char","boolean"
	,"void","true","false","null","this"
	,"let","do","if","else","while"
	,"return"
};
#endif

