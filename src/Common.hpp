#ifndef _COMMON_
#define _COMMON_
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <memory>

using namespace std;

enum TokenType {KEYWORD,SYMBOL,INT_CONSTANT,STRING_CONSTANT,IDENTIFIER};
enum KeyWord {CLASS,CONSTRUCTOR,FUNCTION,METHOD,FIELD
	,STATIC,VAR,INT,CHAR,BOOLEAN
	,VOID,TRUE,FALSE,NULL_PTR,THIS
	,LET,DO,IF,ELSE,WHILE
	,RETURN
};
//extern const int size;

//extern vector<string> Keywords;

const vector<string> Keywords = {
	"class","constructor","function","method","field"
	,"static","var","int","char","boolean"
	,"void","true","false","null","this"
	,"let","do","if","else","while"
	,"return"
};
#endif

