#include "CompilationEngine.hpp"

CompilationEngine::CompilationEngine(string outputFile) : level(0) {
	outputStream.open(outputFile);
}

CompilationEngine::~CompilationEngine() {
	outputStream.close();
}

void CompilationEngine::outputTerminal( JackTokenizer& jackTokenizer) {
	for (int i = 0;i < level;i++) {
		outputStream << "  ";
	}

	if (jackTokenizer.tokenType() == KEYWORD) {
		outputStream << "<keyword> " << Keywords[jackTokenizer.keyWord()] << " </keyword>" << endl;
	}
	else if (jackTokenizer.tokenType() == STRING_CONSTANT) {
		outputStream << "<stringConstant> " << jackTokenizer.stringVal() << " </stringConstant>" << endl;
	}
	else if (jackTokenizer.tokenType() == INT_CONSTANT) {
		outputStream << "<integerConstant> " << jackTokenizer.intVal() << " </integerConstant>" << endl;
	}
	else if (jackTokenizer.tokenType() == IDENTIFIER) {
		outputStream << "<identifier> " << jackTokenizer.identifier() << " </identifier>" << endl;
	}
	else if (jackTokenizer.tokenType() == KEYWORD) {
		outputStream << "<keyword> " << jackTokenizer.keyWord() << " </keyword>" << endl;
	}
	else if (jackTokenizer.tokenType() == SYMBOL) {
		string output = jackTokenizer.symbol();
		if (jackTokenizer.symbol() == "<")
			output = "&lt;";
		else if (jackTokenizer.symbol() == ">")
			output = "&gt;";
		else if (jackTokenizer.symbol() == "\"")
			output = "&quot;";
		else if (jackTokenizer.symbol() == "&")
			output = "&amp;";

		outputStream << "<symbol> " << output << " </symbol>" << endl;
	}
}
void CompilationEngine::outputNonTerminal(string nonTerminal, bool beginOrEnd) {
	// update level
	if(beginOrEnd == false)
		level--;

	for (int i = 0;i < level;i++) {
		outputStream << "  ";
	}
	if (beginOrEnd == true) {
		outputStream << "<" << nonTerminal << ">" << endl;
	}
	else
		outputStream << "</" << nonTerminal << ">" << endl;
	
	// update level
	if (beginOrEnd == true)
		level++;
	
}
void CompilationEngine::compileClass(JackTokenizer& jackTokenizer) {
	outputNonTerminal("class", true);

	// class
	if (jackTokenizer.tokenType() == KEYWORD && jackTokenizer.keyWord() == CLASS) {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// class name
	if (jackTokenizer.tokenType() == IDENTIFIER) {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// {
	if (jackTokenizer.tokenType() == SYMBOL && jackTokenizer.symbol() == "{") {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// class var dec
	// static or field
	while (jackTokenizer.tokenType() == KEYWORD &&
		(jackTokenizer.keyWord() == STATIC || jackTokenizer.keyWord() == FIELD)) {
		compileClassVarDec(jackTokenizer);
	}

	// subroutine dec
	while (jackTokenizer.tokenType() == KEYWORD &&
		(jackTokenizer.keyWord() == CONSTRUCTOR || jackTokenizer.keyWord() == FUNCTION || jackTokenizer.keyWord() == METHOD)) {
		compileSubroutine(jackTokenizer);
	}

	// }
	if (jackTokenizer.tokenType() == SYMBOL && jackTokenizer.symbol() == "}") {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	outputNonTerminal("class", false);
}

void CompilationEngine::compileClassVarDec(JackTokenizer& jackTokenizer) {
	outputNonTerminal("classVarDec", true);

	//static or field
	if (jackTokenizer.tokenType() == KEYWORD &&
		(jackTokenizer.keyWord() == STATIC || jackTokenizer.keyWord() == FIELD)) {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// type
	// int, char or boolean
	if (jackTokenizer.tokenType() == KEYWORD &&
		(jackTokenizer.keyWord() == INT || jackTokenizer.keyWord() == CHAR || jackTokenizer.keyWord() == BOOLEAN)) {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}
	else if (jackTokenizer.tokenType() == IDENTIFIER) { 
		//class name

		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// var name
	if (jackTokenizer.tokenType() == IDENTIFIER) {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// ,varname
	while (jackTokenizer.tokenType() == SYMBOL && jackTokenizer.symbol() == ",") {
		// ,
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();

		// var name
		if (jackTokenizer.tokenType() == IDENTIFIER) {
			outputTerminal(jackTokenizer);
			jackTokenizer.advance();
		}
	}

	// ;
	if (jackTokenizer.tokenType() == SYMBOL && jackTokenizer.symbol() == ";") {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	outputNonTerminal("classVarDec", false);
}
void CompilationEngine::compileSubroutine(JackTokenizer& jackTokenizer) {
	outputNonTerminal("subroutineDec", true);

	// constructor or function or method
	if (jackTokenizer.tokenType() == KEYWORD &&
		(jackTokenizer.keyWord() == CONSTRUCTOR || jackTokenizer.keyWord() == FUNCTION || jackTokenizer.keyWord() == METHOD)) {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// type
	// void
	if (jackTokenizer.tokenType() == KEYWORD && jackTokenizer.keyWord() == VOID) {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}
	else if (jackTokenizer.tokenType() == KEYWORD &&
			(jackTokenizer.keyWord() == INT || jackTokenizer.keyWord() == CHAR || jackTokenizer.keyWord() == BOOLEAN)) {
		// int, char or boolean
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}
	else if (jackTokenizer.tokenType() == IDENTIFIER) {
		//class name

		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// subroutine name
	if (jackTokenizer.tokenType() == IDENTIFIER) {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// (
	if (jackTokenizer.tokenType() == SYMBOL && jackTokenizer.symbol() == "(") {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// parameter list
	compileParameterList(jackTokenizer);

	// )
	if (jackTokenizer.tokenType() == SYMBOL && jackTokenizer.symbol() == ")") {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// subroutine body
	outputNonTerminal("subroutineBody", true);
	// {
	if (jackTokenizer.tokenType() == SYMBOL && jackTokenizer.symbol() == "{") {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// var dec *
	while (jackTokenizer.tokenType() == KEYWORD && jackTokenizer.keyWord() == VAR) {
		compileVarDec(jackTokenizer);
	}
	
	compileStatements(jackTokenizer);

	// }
	if (jackTokenizer.tokenType() == SYMBOL && jackTokenizer.symbol() == "}") {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}
	outputNonTerminal("subroutineBody", false);

	outputNonTerminal("subroutineDec", false);
}

void CompilationEngine::compileParameterList(JackTokenizer& jackTokenizer) {
	outputNonTerminal("parameterList", true);
	
	// type
	if (jackTokenizer.tokenType() == KEYWORD &&
		(jackTokenizer.keyWord() == INT || jackTokenizer.keyWord() == CHAR || jackTokenizer.keyWord() == BOOLEAN)) {
		// int, char or boolean

		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}
	else if (jackTokenizer.tokenType() == IDENTIFIER) {
		//class name

		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// var name
	if (jackTokenizer.tokenType() == IDENTIFIER) {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// , type var name
	while (jackTokenizer.tokenType() == SYMBOL && jackTokenizer.symbol() == ",") {
		// ,
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
		
		// type
		if (jackTokenizer.tokenType() == KEYWORD &&
			(jackTokenizer.keyWord() == INT || jackTokenizer.keyWord() == CHAR || jackTokenizer.keyWord() == BOOLEAN)) {
			// int, char or boolean

			outputTerminal(jackTokenizer);
			jackTokenizer.advance();
		}
		else if (jackTokenizer.tokenType() == IDENTIFIER) {
			//class name

			outputTerminal(jackTokenizer);
			jackTokenizer.advance();
		}

		// var name
		if (jackTokenizer.tokenType() == IDENTIFIER) {
			outputTerminal(jackTokenizer);
			jackTokenizer.advance();
		}
	}

	outputNonTerminal("parameterList", false);
}

void CompilationEngine::compileVarDec(JackTokenizer& jackTokenizer) {
	outputNonTerminal("varDec", true);

	// var
	if (jackTokenizer.tokenType() == KEYWORD && jackTokenizer.keyWord() == VAR) {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// type
	// int, char or boolean
	if (jackTokenizer.tokenType() == KEYWORD &&
		(jackTokenizer.keyWord() == INT || jackTokenizer.keyWord() == CHAR || jackTokenizer.keyWord() == BOOLEAN)) {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}
	else if (jackTokenizer.tokenType() == IDENTIFIER) {

		//class name
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// var name
	if (jackTokenizer.tokenType() == IDENTIFIER) {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// , var name
	while (jackTokenizer.tokenType() == SYMBOL && jackTokenizer.symbol() == ",") {
		// ,
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
		
		// var name
		if (jackTokenizer.tokenType() == IDENTIFIER) {
			outputTerminal(jackTokenizer);
			jackTokenizer.advance();
		}

	}

	// ;
	if (jackTokenizer.tokenType() == SYMBOL && jackTokenizer.symbol() == ";") {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	outputNonTerminal("varDec", false);
}

void CompilationEngine::compileStatements(JackTokenizer& jackTokenizer) {
	outputNonTerminal("statements", true);

	// let or if or while or do or return
	while (jackTokenizer.tokenType() == KEYWORD 
		  && (jackTokenizer.keyWord() == LET || jackTokenizer.keyWord() == IF 
	      || jackTokenizer.keyWord() == WHILE || jackTokenizer.keyWord() == DO 
		  || jackTokenizer.keyWord() == RETURN)) {
		if (jackTokenizer.keyWord() == LET) {
			compileLet(jackTokenizer);
		}
		else if (jackTokenizer.keyWord() == IF) {
			compileIf(jackTokenizer);
		}
		else if (jackTokenizer.keyWord() == WHILE) {
			compileWhile(jackTokenizer);
		}
		else if (jackTokenizer.keyWord() == DO) {
			compileDo(jackTokenizer);
		}
		else if (jackTokenizer.keyWord() == RETURN) {
			compileReturn(jackTokenizer);
		}
	}

	outputNonTerminal("statements", false);
}

void CompilationEngine::compileDo(JackTokenizer& jackTokenizer) {
	outputNonTerminal("doStatement", true);
	// do
	if (jackTokenizer.tokenType() == KEYWORD && jackTokenizer.keyWord() == DO) {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// subroutine call
	// subroutine name or class name or var name
	if (jackTokenizer.tokenType() == IDENTIFIER) { 
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// .subroutine name
	// .
	if (jackTokenizer.tokenType() == SYMBOL && jackTokenizer.symbol() == ".") {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();

		// subroutine name
		if (jackTokenizer.tokenType() == IDENTIFIER) {
			outputTerminal(jackTokenizer);
			jackTokenizer.advance();
		}
	}

	// ( expression list )
	// (
	if (jackTokenizer.tokenType() == SYMBOL && jackTokenizer.symbol() == "(") {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();

		// expression list
		compileExpressionList(jackTokenizer);

		// )
		if (jackTokenizer.tokenType() == SYMBOL && jackTokenizer.symbol() == ")") {
			outputTerminal(jackTokenizer);
			jackTokenizer.advance();
		}
	}

	// ;
	if (jackTokenizer.tokenType() == SYMBOL && jackTokenizer.symbol() == ";") {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	outputNonTerminal("doStatement", false);
}

void CompilationEngine::compileLet(JackTokenizer& jackTokenizer) {
	outputNonTerminal("letStatement", true);
	// let
	if (jackTokenizer.tokenType() == KEYWORD && jackTokenizer.keyWord() == LET) {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// var name
	// identifier
	if (jackTokenizer.tokenType() == IDENTIFIER) {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// [
	if (jackTokenizer.tokenType() == SYMBOL && jackTokenizer.symbol() == "[") {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();

		// expression
		compileExpression(jackTokenizer);

		// ]
		if (jackTokenizer.tokenType() == SYMBOL && jackTokenizer.symbol() == "]") {
			outputTerminal(jackTokenizer);
			jackTokenizer.advance();
		}
	}
	
	// =
	if (jackTokenizer.tokenType() == SYMBOL && jackTokenizer.symbol() == "=") {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// expression
	compileExpression(jackTokenizer);

	// ;
	if (jackTokenizer.tokenType() == SYMBOL && jackTokenizer.symbol() == ";") {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	outputNonTerminal("letStatement", false);
}
void CompilationEngine::compileWhile(JackTokenizer& jackTokenizer) {
	outputNonTerminal("whileStatement", true);

	// while
	if (jackTokenizer.tokenType() == KEYWORD && jackTokenizer.keyWord() == WHILE) {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// (
	if (jackTokenizer.tokenType() == SYMBOL && jackTokenizer.symbol() == "(") {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// expression
	compileExpression(jackTokenizer);

	// )
	if (jackTokenizer.tokenType() == SYMBOL && jackTokenizer.symbol() == ")") {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// {
	if (jackTokenizer.tokenType() == SYMBOL && jackTokenizer.symbol() == "{") {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// statements
	compileStatements(jackTokenizer);

	// }
	if (jackTokenizer.tokenType() == SYMBOL && jackTokenizer.symbol() == "}") {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// ;
	if (jackTokenizer.tokenType() == SYMBOL && jackTokenizer.symbol() == ";") {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	outputNonTerminal("whileStatement", false);
}

void CompilationEngine::compileReturn(JackTokenizer& jackTokenizer) {
	outputNonTerminal("returnStatement", true);

	// return
	if (jackTokenizer.tokenType() == KEYWORD && jackTokenizer.keyWord() == RETURN) {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	//expression
	if (jackTokenizer.tokenType() != SYMBOL || jackTokenizer.symbol() != ";")
		compileExpression(jackTokenizer);

	// ;
	if (jackTokenizer.tokenType() == SYMBOL && jackTokenizer.symbol() == ";") {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	outputNonTerminal("returnStatement", false);
}

void CompilationEngine::compileIf(JackTokenizer& jackTokenizer) {

	outputNonTerminal("ifStatement", true);

	// if
	if (jackTokenizer.tokenType() == KEYWORD && jackTokenizer.keyWord() == IF) {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// (
	if (jackTokenizer.tokenType() == SYMBOL && jackTokenizer.symbol() == "(") {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// expression
	compileExpression(jackTokenizer);

	// )
	if (jackTokenizer.tokenType() == SYMBOL && jackTokenizer.symbol() == ")") {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// {
	if (jackTokenizer.tokenType() == SYMBOL && jackTokenizer.symbol() == "{") {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// statements
	compileStatements(jackTokenizer);

	// }
	if (jackTokenizer.tokenType() == SYMBOL && jackTokenizer.symbol() == "}") {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// else
	if (jackTokenizer.tokenType() == KEYWORD && jackTokenizer.keyWord() == ELSE) {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();

		// {
		if (jackTokenizer.tokenType() == SYMBOL && jackTokenizer.symbol() == "{") {
			outputTerminal(jackTokenizer);
			jackTokenizer.advance();
		}

		// statements
		compileStatements(jackTokenizer);

		// }
		if (jackTokenizer.tokenType() == SYMBOL && jackTokenizer.symbol() == "}") {
			outputTerminal(jackTokenizer);
			jackTokenizer.advance();
		}
	}

	// ;
	if (jackTokenizer.tokenType() == SYMBOL && jackTokenizer.symbol() == ";") {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	outputNonTerminal("ifStatement", false);
}
bool CompilationEngine::isOp(string symbol)
{
	return (symbol == "+" || symbol == "-" || symbol == "*" || symbol == "/" || symbol == "&"
		|| symbol == "|" || symbol == "<" || symbol == ">" || symbol == "=");
}
void CompilationEngine::compileExpression(JackTokenizer& jackTokenizer) {
	outputNonTerminal("expression", true);
	// term
	compileTerm(jackTokenizer);

	// op term
	while (jackTokenizer.tokenType() == SYMBOL && isOp(jackTokenizer.symbol())) {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();

		compileTerm(jackTokenizer);
	}
	outputNonTerminal("expression", false);
}

void CompilationEngine::compileTerm(JackTokenizer& jackTokenizer) {
	outputNonTerminal("term", true);

	// integer constant
	if (jackTokenizer.tokenType() == INT_CONSTANT) {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}
	else if (jackTokenizer.tokenType() == STRING_CONSTANT) {	
		// string constant

		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}
	else if (jackTokenizer.tokenType() == KEYWORD) {
		// keyWord constant

		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}
	else if (jackTokenizer.tokenType() == SYMBOL && jackTokenizer.symbol() == "(") {
		// (expression)
		// (
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();

		// expression
		compileExpression(jackTokenizer);

		// )
		if (jackTokenizer.tokenType() == SYMBOL && jackTokenizer.symbol() == ")") {
			outputTerminal(jackTokenizer);
			jackTokenizer.advance();
		}

	}
	else if (jackTokenizer.tokenType() == SYMBOL &&
		(jackTokenizer.symbol() == "-" || jackTokenizer.symbol() == "~")) {
		// unary operation term
		// unary operation
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();

		// term
		compileTerm(jackTokenizer);
	}
	else if (jackTokenizer.tokenType() == IDENTIFIER) {
		// var name or var name [ expression ] or subroutine call
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();

		if (jackTokenizer.tokenType() == SYMBOL) {
			// var name [ expression ] or subroutine call
			// [ expression ]
			// [
			if(jackTokenizer.symbol() == "[") {
				
				// [
				outputTerminal(jackTokenizer);
				jackTokenizer.advance();

				// expression
				compileExpression(jackTokenizer);

				// ]
				if (jackTokenizer.tokenType() == SYMBOL && jackTokenizer.symbol() == "]") {
					outputTerminal(jackTokenizer);
					jackTokenizer.advance();
				}
			}
			else {
				//subroutine call
				// .subroutine name
				//.
				if (jackTokenizer.tokenType() == SYMBOL && jackTokenizer.symbol() == ".") {
					outputTerminal(jackTokenizer);
					jackTokenizer.advance();

					// subroutine name
					if (jackTokenizer.tokenType() == IDENTIFIER) {
						outputTerminal(jackTokenizer);
						jackTokenizer.advance();
					}
				}

				// ( expression list )
				// (
				if (jackTokenizer.tokenType() == SYMBOL && jackTokenizer.symbol() == "(") {
					outputTerminal(jackTokenizer);
					jackTokenizer.advance();

					// expression list
					compileExpressionList(jackTokenizer);

					// )
					if (jackTokenizer.tokenType() == SYMBOL && jackTokenizer.symbol() == ")") {
						outputTerminal(jackTokenizer);
						jackTokenizer.advance();
					}
				}
			}

		}
		else {
			//var name
		}
	}
	outputNonTerminal("term", false);
}

void CompilationEngine::compileExpressionList(JackTokenizer& jackTokenizer) {
	outputNonTerminal("expressionList", true);

	// expression list is empty only if next token is )
	if (jackTokenizer.tokenType() != SYMBOL || jackTokenizer.symbol() != ")") {
		// expression
		compileExpression(jackTokenizer);

		// ,expression
		while (jackTokenizer.tokenType() == SYMBOL && jackTokenizer.symbol() == ",") {
			//,
			outputTerminal(jackTokenizer);
			jackTokenizer.advance();

			// expression
			compileExpression(jackTokenizer);
		}
	}

	outputNonTerminal("expressionList", false);
}