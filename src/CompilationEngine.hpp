#ifndef _COMPILATION_ENGINE_
#define _COMPILATION_ENGINE_
#include "Common.hpp"
#include "JackTokenizer.hpp"
#include "SymbolTable.hpp"

class CompilationEngine {
public:
	CompilationEngine(string outputFile);
	~CompilationEngine();

	// compile a .jack file
	void compileClass(JackTokenizer& jackTokenizer, SymbolTable& symbolTable);

private:
	ofstream outputStream;
	
	void compileClassVarDec(JackTokenizer& jackTokenizer, SymbolTable& symbolTable);
	void compileSubroutine(JackTokenizer& jackTokenizer, SymbolTable& symbolTable);
	void compileParameterList(JackTokenizer& jackTokenizer, SymbolTable& symbolTable);
	void compileVarDec(JackTokenizer& jackTokenizer, SymbolTable& symbolTable);
	void compileStatements(JackTokenizer& jackTokenizer, SymbolTable& symbolTable);
	void compileDo(JackTokenizer& jackTokenizer, SymbolTable& symbolTable);
	void compileLet(JackTokenizer& jackTokenizer, SymbolTable& symbolTable);
	void compileWhile(JackTokenizer& jackTokenizer, SymbolTable& symbolTable);
	void compileReturn(JackTokenizer& jackTokenizer, SymbolTable& symbolTable);
	void compileIf(JackTokenizer& jackTokenizer, SymbolTable& symbolTable);
	void compileExpression(JackTokenizer& jackTokenizer, SymbolTable& symbolTable);
	void compileTerm(JackTokenizer& jackTokenizer, SymbolTable& symbolTable);
	void compileExpressionList(JackTokenizer& jackTokenizer, SymbolTable& symbolTable);

	// output current terminal
	void outputTerminal(JackTokenizer& jackTokenizer);
	void outputNonTerminal(string nonTerminal, bool beginOrEnd);
	bool isOp(string symbol);

	// name of current class
	string className;

	// control output format
	int level;
};


#endif