#ifndef _SYMBOL_TABLE_
#define _SYMBOL_TABLE_
#include "Common.hpp"
#include <unordered_map>

struct TableItem {
	string type;
	IdentifierKind kind;
	int index;
};

class SymbolTable {
public:
	SymbolTable();
	~SymbolTable();

	// start to compile a new subroutine
	// clean current subroutineTable, build a new one
	void startSubroutine();

	// one identifier definition
	void define(const string& name,const string& type,IdentifierKind kind);

	// get attributes of a identifier using name
	IdentifierKind kindOf(const string& name);
	string typeOf(const string& name);
	int indexOf(const string& name);

private:
	unordered_map<string, TableItem> classTable;
	unordered_map<string, TableItem> subroutineTable;

	// the index of identifiers and the offset in segments
	int staticIndex;
	int fieldIndex;
	int argIndex;
	int varIndex;
};

#endif