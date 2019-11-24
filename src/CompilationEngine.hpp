#ifndef _COMPILATION_ENGINE_
#define _COMPILATION_ENGINE_
#include "Common.hpp"
#include "JackTokenizer.hpp"
class CompilationEngine {
public:
	CompilationEngine(string outputFile);
	~CompilationEngine();

	void compileClass(JackTokenizer& jackTokenizer);

private:
	ofstream outputStream;
	
	void compileClassVarDec(JackTokenizer& jackTokenizer);
	void compileSubroutine(JackTokenizer& jackTokenizer);
	void compileParameterList(JackTokenizer& jackTokenizer);
	void compileVarDec(JackTokenizer& jackTokenizer);
	void compileStatements(JackTokenizer& jackTokenizer);
	void compileDo(JackTokenizer& jackTokenizer);
	void compileLet(JackTokenizer& jackTokenizer);
	void compileWhile(JackTokenizer& jackTokenizer);
	void compileReturn(JackTokenizer& jackTokenizer);
	void compileIf(JackTokenizer& jackTokenizer);
	void compileExpression(JackTokenizer& jackTokenizer);
	void compileTerm(JackTokenizer& jackTokenizer);
	void compileExpressionList(JackTokenizer& jackTokenizer);

	// output current token
	void outputTerminal(JackTokenizer& jackTokenizer);
	void outputNonTerminal(string nonTerminal, bool beginOrEnd);
	bool isOp(string symbol);

	int level;
};


#endif