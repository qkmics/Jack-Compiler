#include "SymbolTable.hpp"

SymbolTable::SymbolTable() {
	staticIndex = 0;
	fieldIndex = 0;
	argIndex = 0;
	varIndex = 0;
}

SymbolTable::~SymbolTable() {

}

// start to compile a new subroutine
// clean current subroutineTable, build a new one
void SymbolTable::startSubroutine() {
	
	// clear the current subroutine table
	subroutineTable.clear();

	// reset all the indices
	staticIndex = 0;
	fieldIndex = 0;
	argIndex = 0;
	varIndex = 0;
}

// one identifier definition
void SymbolTable::define(const string& name, const string& type, IdentifierKind kind) {
	TableItem tableItem;
	tableItem.type = type;
	tableItem.kind = kind;
	if (kind == IdentifierKind::STATIC) {
		tableItem.index = staticIndex;
		staticIndex++;
	}
	else if (kind == IdentifierKind::FIELD) {
		tableItem.index = fieldIndex;
		fieldIndex++;
	}
	else if (kind == IdentifierKind::ARG) {
		tableItem.index = argIndex;
		argIndex++;
	}
	else if (kind == IdentifierKind::VAR) {
		tableItem.index = varIndex;
		varIndex++;
	}
	if (kind == IdentifierKind::STATIC || kind == IdentifierKind::FIELD) {
		classTable[name] = tableItem;
	}
	else if (kind == IdentifierKind::ARG || kind == IdentifierKind::VAR) {
		subroutineTable[name] = tableItem;
	}
}

// get attributes of a identifier using name
IdentifierKind SymbolTable::kindOf(const string& name) {
	if (subroutineTable.find(name) != subroutineTable.end()) {
		return subroutineTable[name].kind;
	}
	else if (classTable.find(name) != classTable.end()) {
		return classTable[name].kind;
	}
	else
		return IdentifierKind::UNKNOWN;
}

string SymbolTable::typeOf(const string& name) {
	if (subroutineTable.find(name) != subroutineTable.end()) {
		return subroutineTable[name].type;
	}
	else if (classTable.find(name) != classTable.end()) {
		return classTable[name].type;
	}
	else {
		cout << "Error";
	}
}

int SymbolTable::indexOf(const string& name) {
	if (subroutineTable.find(name) != subroutineTable.end()) {
		return subroutineTable[name].index;
	}
	else if (classTable.find(name) != classTable.end()) {
		return classTable[name].index;
	}
	else {
		cout << "Error";
	}
}