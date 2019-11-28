#ifndef _COMPILATION_ENGINE_
#define _COMPILATION_ENGINE_
#include "Common.hpp"
#include "JackTokenizer.hpp"
#include "SymbolTable.hpp"
#include "VMWriter.hpp"

class CompilationEngine {
public:
	CompilationEngine();
	~CompilationEngine();

	// compile a .jack file
	void compileClass(JackTokenizer& jackTokenizer, SymbolTable& symbolTable, VMWriter& vMWriter);

private:
	
	void compileClassVarDec(JackTokenizer& jackTokenizer, SymbolTable& symbolTable, VMWriter& vMWriter);
	void compileSubroutine(JackTokenizer& jackTokenizer, SymbolTable& symbolTable, VMWriter& vMWriter);
	void compileParameterList(JackTokenizer& jackTokenizer, SymbolTable& symbolTable, VMWriter& vMWriter);
	void compileVarDec(JackTokenizer& jackTokenizer, SymbolTable& symbolTable, VMWriter& vMWriter, int& varCount);
	void compileStatements(JackTokenizer& jackTokenizer, SymbolTable& symbolTable, VMWriter& vMWriter);
	void compileDo(JackTokenizer& jackTokenizer, SymbolTable& symbolTable, VMWriter& vMWriter);
	void compileLet(JackTokenizer& jackTokenizer, SymbolTable& symbolTable, VMWriter& vMWriter);
	void compileWhile(JackTokenizer& jackTokenizer, SymbolTable& symbolTable, VMWriter& vMWriter);
	void compileReturn(JackTokenizer& jackTokenizer, SymbolTable& symbolTable, VMWriter& vMWriter);
	void compileIf(JackTokenizer& jackTokenizer, SymbolTable& symbolTable, VMWriter& vMWriter);
	void compileExpression(JackTokenizer& jackTokenizer, SymbolTable& symbolTable, VMWriter& vMWriter);
	void compileTerm(JackTokenizer& jackTokenizer, SymbolTable& symbolTable, VMWriter& vMWriter);
	void compileExpressionList(JackTokenizer& jackTokenizer, SymbolTable& symbolTable, VMWriter& vMWriter, int& expressionCount);

	// return if the input symbol is an operation
	bool isOp(string symbol);

	// find the corresponding segment of given Identifier kind
	Segment identifierKindToSeg(IdentifierKind kind);

	// find the corresponding command of given operation
	Command operationToCommand(string op);

	// find the corresponding constant of given keyWord constant
	int keyWordConstToConst(KeyWord keyWordConst);
	
	// name of current class
	string className;

	// control output format
	int level;

	// to distinguish different lable
	int lableNum;

	// number of field in the class
	int fieldNum;
};

#endif