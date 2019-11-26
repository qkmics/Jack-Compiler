#include "CompilationEngine.hpp"
#include "SymbolTable.hpp"
CompilationEngine::CompilationEngine(string outputFile) : level(0), className("") {
	outputStream.open(outputFile);
}

CompilationEngine::~CompilationEngine() {
	outputStream.close();
}

void CompilationEngine::outputTerminal( JackTokenizer& jackTokenizer) {
	for (int i = 0;i < level;i++) {
		outputStream << "  ";
	}

	if (jackTokenizer.tokenType() == TokenType::KEYWORD) {
		outputStream << "<keyword> " << KeyWordString[int(jackTokenizer.keyWord())] << " </keyword>" << endl;
	}
	else if (jackTokenizer.tokenType() == TokenType::STRING_CONSTANT) {
		outputStream << "<stringConstant> " << jackTokenizer.stringVal() << " </stringConstant>" << endl;
	}
	else if (jackTokenizer.tokenType() == TokenType::INT_CONSTANT) {
		outputStream << "<integerConstant> " << jackTokenizer.intVal() << " </integerConstant>" << endl;
	}
	else if (jackTokenizer.tokenType() == TokenType::IDENTIFIER) {
		outputStream << "<identifier> " << jackTokenizer.identifier() << " </identifier>" << endl;
	}
	else if (jackTokenizer.tokenType() == TokenType::SYMBOL) {
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

void CompilationEngine::compileClass(JackTokenizer& jackTokenizer, SymbolTable& symbolTable) {
	outputNonTerminal("class", true);

	// class
	if (jackTokenizer.tokenType() == TokenType::KEYWORD && jackTokenizer.keyWord() == KeyWord::CLASS) {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// class name
	if (jackTokenizer.tokenType() == TokenType::IDENTIFIER) {
		// assign name of current class
		className = jackTokenizer.identifier();

		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// {
	if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == "{") {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// class var dec
	// static or field
	while (jackTokenizer.tokenType() == TokenType::KEYWORD &&
		(jackTokenizer.keyWord() == KeyWord::STATIC || jackTokenizer.keyWord() == KeyWord::FIELD)) {
		compileClassVarDec(jackTokenizer, symbolTable);
	}

	// subroutine dec
	while (jackTokenizer.tokenType() == TokenType::KEYWORD &&
		(jackTokenizer.keyWord() == KeyWord::CONSTRUCTOR || jackTokenizer.keyWord() == KeyWord::FUNCTION || jackTokenizer.keyWord() == KeyWord::METHOD)) {
		compileSubroutine(jackTokenizer, symbolTable);
	}

// }
if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == "}") {
	outputTerminal(jackTokenizer);
	jackTokenizer.advance();
}

outputNonTerminal("class", false);
}

void CompilationEngine::compileClassVarDec(JackTokenizer& jackTokenizer, SymbolTable& symbolTable) {
	outputNonTerminal("classVarDec", true);

	string name, type;
	IdentifierKind kind = IdentifierKind::UNKNOWN;

	//static or field
	if (jackTokenizer.tokenType() == TokenType::KEYWORD &&
		(jackTokenizer.keyWord() == KeyWord::STATIC || jackTokenizer.keyWord() == KeyWord::FIELD)) {

		if (jackTokenizer.keyWord() == KeyWord::STATIC)
			kind = IdentifierKind::STATIC;
		else if (jackTokenizer.keyWord() == KeyWord::FIELD)
			kind = IdentifierKind::FIELD;

		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// type
	// int, char or boolean
	if (jackTokenizer.tokenType() == TokenType::KEYWORD &&
		(jackTokenizer.keyWord() == KeyWord::INT || jackTokenizer.keyWord() == KeyWord::CHAR || jackTokenizer.keyWord() == KeyWord::BOOLEAN)) {

		if (jackTokenizer.keyWord() == KeyWord::INT)
			type = "int";
		else if (jackTokenizer.keyWord() == KeyWord::CHAR)
			type = "char";
		else if (jackTokenizer.keyWord() == KeyWord::BOOLEAN)
			type = "boolean";

		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}
	else if (jackTokenizer.tokenType() == TokenType::IDENTIFIER) {
		//class name

		type = jackTokenizer.identifier();

		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// var name
	if (jackTokenizer.tokenType() == TokenType::IDENTIFIER) {

		name = jackTokenizer.identifier();
		symbolTable.define(name, type, kind);

		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// ,varname
	while (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == ",") {
		// ,
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();

		// var name
		if (jackTokenizer.tokenType() == TokenType::IDENTIFIER) {

			name = jackTokenizer.identifier();
			symbolTable.define(name, type, kind);

			outputTerminal(jackTokenizer);
			jackTokenizer.advance();
		}
	}

	// ;
	if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == ";") {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}


	outputNonTerminal("classVarDec", false);
}

void CompilationEngine::compileSubroutine(JackTokenizer& jackTokenizer, SymbolTable& symbolTable) {
	outputNonTerminal("subroutineDec", true);

	// let symbol table know that a new subroutine started
	symbolTable.startSubroutine();

	// constructor or function or method
	if (jackTokenizer.tokenType() == TokenType::KEYWORD &&
		(jackTokenizer.keyWord() == KeyWord::CONSTRUCTOR ||
			jackTokenizer.keyWord() == KeyWord::FUNCTION ||
			jackTokenizer.keyWord() == KeyWord::METHOD)) {

		// for method and constructor, we need to add "this" as a argument
		if (jackTokenizer.keyWord() == KeyWord::CONSTRUCTOR || jackTokenizer.keyWord() == KeyWord::METHOD) {
			string name = "this";
			symbolTable.define(name,className,IdentifierKind::ARG);
		}

		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// type
	// void
	if (jackTokenizer.tokenType() == TokenType::KEYWORD && jackTokenizer.keyWord() == KeyWord::VOID) {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}
	else if (jackTokenizer.tokenType() == TokenType::KEYWORD &&
			(jackTokenizer.keyWord() == KeyWord::INT || jackTokenizer.keyWord() == KeyWord::CHAR || jackTokenizer.keyWord() == KeyWord::BOOLEAN)) {
		// int, char or boolean
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}
	else if (jackTokenizer.tokenType() == TokenType::IDENTIFIER) {
		//class name

		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// subroutine name
	if (jackTokenizer.tokenType() == TokenType::IDENTIFIER) {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// (
	if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == "(") {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// parameter list
	compileParameterList(jackTokenizer, symbolTable);

	// )
	if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == ")") {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// subroutine body
	outputNonTerminal("subroutineBody", true);
	// {
	if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == "{") {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// var dec *
	while (jackTokenizer.tokenType() == TokenType::KEYWORD && jackTokenizer.keyWord() == KeyWord::VAR) {
		compileVarDec(jackTokenizer, symbolTable);
	}
	
	compileStatements(jackTokenizer, symbolTable);

	// }
	if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == "}") {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}
	outputNonTerminal("subroutineBody", false);

	outputNonTerminal("subroutineDec", false);
}

void CompilationEngine::compileParameterList(JackTokenizer& jackTokenizer, SymbolTable& symbolTable) {
	outputNonTerminal("parameterList", true);
	
	string name, type;
	IdentifierKind kind = IdentifierKind::ARG;

	// type
	if (jackTokenizer.tokenType() == TokenType::KEYWORD &&
		(jackTokenizer.keyWord() == KeyWord::INT || jackTokenizer.keyWord() == KeyWord::CHAR || jackTokenizer.keyWord() == KeyWord::BOOLEAN)) {
		// int, char or boolean

		if (jackTokenizer.keyWord() == KeyWord::INT)
			type = "int";
		else if (jackTokenizer.keyWord() == KeyWord::CHAR)
			type = "char";
		else if (jackTokenizer.keyWord() == KeyWord::BOOLEAN)
			type = "boolean";

		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}
	else if (jackTokenizer.tokenType() == TokenType::IDENTIFIER) {
		//class name

		type = jackTokenizer.identifier();

		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// var name
	if (jackTokenizer.tokenType() == TokenType::IDENTIFIER) {

		name = jackTokenizer.identifier();
		symbolTable.define(name, type, kind);

		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// , type var name
	while (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == ",") {
		// ,
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
		
		// type
		if (jackTokenizer.tokenType() == TokenType::KEYWORD &&
			(jackTokenizer.keyWord() == KeyWord::INT || jackTokenizer.keyWord() == KeyWord::CHAR || jackTokenizer.keyWord() == KeyWord::BOOLEAN)) {
			// int, char or boolean

			if (jackTokenizer.keyWord() == KeyWord::INT)
				type = "int";
			else if (jackTokenizer.keyWord() == KeyWord::CHAR)
				type = "char";
			else if (jackTokenizer.keyWord() == KeyWord::BOOLEAN)
				type = "boolean";

			outputTerminal(jackTokenizer);
			jackTokenizer.advance();
		}
		else if (jackTokenizer.tokenType() == TokenType::IDENTIFIER) {
			//class name

			type = jackTokenizer.identifier();

			outputTerminal(jackTokenizer);
			jackTokenizer.advance();
		}

		// var name
		if (jackTokenizer.tokenType() == TokenType::IDENTIFIER) {

			name = jackTokenizer.identifier();
			symbolTable.define(name, type, kind);

			outputTerminal(jackTokenizer);
			jackTokenizer.advance();
		}
	}

	outputNonTerminal("parameterList", false);
}

void CompilationEngine::compileVarDec(JackTokenizer& jackTokenizer, SymbolTable& symbolTable) {
	outputNonTerminal("varDec", true);

	string name, type;
	IdentifierKind kind = IdentifierKind::VAR;

	// var
	if (jackTokenizer.tokenType() == TokenType::KEYWORD && jackTokenizer.keyWord() == KeyWord::VAR) {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// type
	// int, char or boolean
	if (jackTokenizer.tokenType() == TokenType::KEYWORD &&
		(jackTokenizer.keyWord() == KeyWord::INT || jackTokenizer.keyWord() == KeyWord::CHAR || jackTokenizer.keyWord() == KeyWord::BOOLEAN)) {
		
		if (jackTokenizer.keyWord() == KeyWord::INT)
			type = "int";
		else if (jackTokenizer.keyWord() == KeyWord::CHAR)
			type = "char";
		else if (jackTokenizer.keyWord() == KeyWord::BOOLEAN)
			type = "boolean";

		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}
	else if (jackTokenizer.tokenType() == TokenType::IDENTIFIER) {
		//class name

		type = jackTokenizer.identifier();

		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// var name
	if (jackTokenizer.tokenType() == TokenType::IDENTIFIER) {

		name = jackTokenizer.identifier();
		symbolTable.define(name, type, kind);

		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// , var name
	while (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == ",") {
		// ,
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
		
		// var name
		if (jackTokenizer.tokenType() == TokenType::IDENTIFIER) {

			name = jackTokenizer.identifier();
			symbolTable.define(name, type, kind);

			outputTerminal(jackTokenizer);
			jackTokenizer.advance();
		}

	}

	// ;
	if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == ";") {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	outputNonTerminal("varDec", false);
}

void CompilationEngine::compileStatements(JackTokenizer& jackTokenizer, SymbolTable& symbolTable) {
	outputNonTerminal("statements", true);

	// let or if or while or do or return
	while (jackTokenizer.tokenType() == TokenType::KEYWORD 
		  && (jackTokenizer.keyWord() == KeyWord::LET || jackTokenizer.keyWord() == KeyWord::IF 
	      || jackTokenizer.keyWord() == KeyWord::WHILE || jackTokenizer.keyWord() == KeyWord::DO 
		  || jackTokenizer.keyWord() == KeyWord::RETURN)) {
		if (jackTokenizer.keyWord() == KeyWord::LET) {
			compileLet(jackTokenizer, symbolTable);
		}
		else if (jackTokenizer.keyWord() == KeyWord::IF) {
			compileIf(jackTokenizer, symbolTable);
		}
		else if (jackTokenizer.keyWord() == KeyWord::WHILE) {
			compileWhile(jackTokenizer, symbolTable);
		}
		else if (jackTokenizer.keyWord() == KeyWord::DO) {
			compileDo(jackTokenizer, symbolTable);
		}
		else if (jackTokenizer.keyWord() == KeyWord::RETURN) {
			compileReturn(jackTokenizer, symbolTable);
		}
	}

	outputNonTerminal("statements", false);
}

void CompilationEngine::compileDo(JackTokenizer& jackTokenizer, SymbolTable& symbolTable) {
	outputNonTerminal("doStatement", true);
	// do
	if (jackTokenizer.tokenType() == TokenType::KEYWORD && jackTokenizer.keyWord() == KeyWord::DO) {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// subroutine call
	// subroutine name or class name or var name
	if (jackTokenizer.tokenType() == TokenType::IDENTIFIER) { 

		string name = jackTokenizer.identifier();

		if (symbolTable.kindOf(name) != IdentifierKind::UNKNOWN)
			cout << name << " " << symbolTable.typeOf(name) << " " << (int)symbolTable.kindOf(name) << " " << symbolTable.indexOf(name) << endl;
		
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// .subroutine name
	// .
	if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == ".") {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();

		// subroutine name
		if (jackTokenizer.tokenType() == TokenType::IDENTIFIER) {
			outputTerminal(jackTokenizer);
			jackTokenizer.advance();
		}
	}

	// ( expression list )
	// (
	if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == "(") {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();

		// expression list
		compileExpressionList(jackTokenizer, symbolTable);

		// )
		if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == ")") {
			outputTerminal(jackTokenizer);
			jackTokenizer.advance();
		}
	}

	// ;
	if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == ";") {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	outputNonTerminal("doStatement", false);
}

void CompilationEngine::compileLet(JackTokenizer& jackTokenizer, SymbolTable& symbolTable) {
	outputNonTerminal("letStatement", true);
	// let
	if (jackTokenizer.tokenType() == TokenType::KEYWORD && jackTokenizer.keyWord() == KeyWord::LET) {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// var name
	// identifier
	if (jackTokenizer.tokenType() == TokenType::IDENTIFIER) {
		
		string name = jackTokenizer.identifier();
		
		cout << name << " " << symbolTable.typeOf(name) << " " << (int)symbolTable.kindOf(name) << " " << symbolTable.indexOf(name) << endl;

		outputTerminal(jackTokenizer);

		jackTokenizer.advance();
	}

	// [
	if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == "[") {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();

		// expression
		compileExpression(jackTokenizer, symbolTable);

		// ]
		if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == "]") {
			outputTerminal(jackTokenizer);
			jackTokenizer.advance();
		}
	}
	
	// =
	if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == "=") {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// expression
	compileExpression(jackTokenizer, symbolTable);

	// ;
	if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == ";") {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	outputNonTerminal("letStatement", false);
}
void CompilationEngine::compileWhile(JackTokenizer& jackTokenizer, SymbolTable& symbolTable) {
	outputNonTerminal("whileStatement", true);

	// while
	if (jackTokenizer.tokenType() == TokenType::KEYWORD && jackTokenizer.keyWord() == KeyWord::WHILE) {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// (
	if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == "(") {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// expression
	compileExpression(jackTokenizer, symbolTable);

	// )
	if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == ")") {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// {
	if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == "{") {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// statements
	compileStatements(jackTokenizer, symbolTable);

	// }
	if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == "}") {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// ;
	if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == ";") {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	outputNonTerminal("whileStatement", false);
}

void CompilationEngine::compileReturn(JackTokenizer& jackTokenizer, SymbolTable& symbolTable) {
	outputNonTerminal("returnStatement", true);

	// return
	if (jackTokenizer.tokenType() == TokenType::KEYWORD && jackTokenizer.keyWord() == KeyWord::RETURN) {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	//expression
	if (jackTokenizer.tokenType() != TokenType::SYMBOL || jackTokenizer.symbol() != ";")
		compileExpression(jackTokenizer, symbolTable);

	// ;
	if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == ";") {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	outputNonTerminal("returnStatement", false);
}

void CompilationEngine::compileIf(JackTokenizer& jackTokenizer, SymbolTable& symbolTable) {

	outputNonTerminal("ifStatement", true);

	// if
	if (jackTokenizer.tokenType() == TokenType::KEYWORD && jackTokenizer.keyWord() == KeyWord::IF) {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// (
	if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == "(") {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// expression
	compileExpression(jackTokenizer, symbolTable);

	// )
	if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == ")") {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// {
	if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == "{") {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// statements
	compileStatements(jackTokenizer, symbolTable);

	// }
	if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == "}") {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}

	// else
	if (jackTokenizer.tokenType() == TokenType::KEYWORD && jackTokenizer.keyWord() == KeyWord::ELSE) {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();

		// {
		if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == "{") {
			outputTerminal(jackTokenizer);
			jackTokenizer.advance();
		}

		// statements
		compileStatements(jackTokenizer, symbolTable);

		// }
		if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == "}") {
			outputTerminal(jackTokenizer);
			jackTokenizer.advance();
		}
	}

	// ;
	if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == ";") {
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
void CompilationEngine::compileExpression(JackTokenizer& jackTokenizer, SymbolTable& symbolTable) {
	outputNonTerminal("expression", true);
	// term
	compileTerm(jackTokenizer, symbolTable);

	// op term
	while (jackTokenizer.tokenType() == TokenType::SYMBOL && isOp(jackTokenizer.symbol())) {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();

		compileTerm(jackTokenizer, symbolTable);
	}
	outputNonTerminal("expression", false);
}

void CompilationEngine::compileTerm(JackTokenizer& jackTokenizer, SymbolTable& symbolTable) {
	outputNonTerminal("term", true);

	// integer constant
	if (jackTokenizer.tokenType() == TokenType::INT_CONSTANT) {
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}
	else if (jackTokenizer.tokenType() == TokenType::STRING_CONSTANT) {	
		// string constant

		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}
	else if (jackTokenizer.tokenType() == TokenType::KEYWORD) {
		// keyWord constant

		outputTerminal(jackTokenizer);
		jackTokenizer.advance();
	}
	else if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == "(") {
		// (expression)
		// (
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();

		// expression
		compileExpression(jackTokenizer, symbolTable);

		// )
		if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == ")") {
			outputTerminal(jackTokenizer);
			jackTokenizer.advance();
		}

	}
	else if (jackTokenizer.tokenType() == TokenType::SYMBOL &&
		(jackTokenizer.symbol() == "-" || jackTokenizer.symbol() == "~")) {
		// unary operation term
		// unary operation
		outputTerminal(jackTokenizer);
		jackTokenizer.advance();

		// term
		compileTerm(jackTokenizer, symbolTable);
	}
	else if (jackTokenizer.tokenType() == TokenType::IDENTIFIER) {
		// var name or var name [ expression ] or subroutine call
		// if it is a subroutine call, it can be var name, class name, or subroutine name

		string name = jackTokenizer.identifier();

		if(symbolTable.kindOf(name)!= IdentifierKind::UNKNOWN)
			cout << name <<" "<< symbolTable.typeOf(name) << " "<<(int)symbolTable.kindOf(name) << " "<<symbolTable.indexOf(name) << endl;

		outputTerminal(jackTokenizer);
		jackTokenizer.advance();

		if (jackTokenizer.tokenType() == TokenType::SYMBOL) {
			// var name [ expression ] or subroutine call
	
			if(jackTokenizer.symbol() == "[") {
				// [ expression ]

				// [
				outputTerminal(jackTokenizer);
				jackTokenizer.advance();

				// expression
				compileExpression(jackTokenizer, symbolTable);

				// ]
				if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == "]") {
					outputTerminal(jackTokenizer);
					jackTokenizer.advance();
				}
			}
			else {
				// subroutine call

				// .subroutine name
				// .
				if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == ".") {
					outputTerminal(jackTokenizer);
					jackTokenizer.advance();

					// subroutine name
					if (jackTokenizer.tokenType() == TokenType::IDENTIFIER) {
						outputTerminal(jackTokenizer);
						jackTokenizer.advance();
					}
				}

				// ( expression list )
				// (
				if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == "(") {
					outputTerminal(jackTokenizer);
					jackTokenizer.advance();

					// expression list
					compileExpressionList(jackTokenizer, symbolTable);

					// )
					if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == ")") {
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

void CompilationEngine::compileExpressionList(JackTokenizer& jackTokenizer, SymbolTable& symbolTable) {
	outputNonTerminal("expressionList", true);

	// expression list is empty only if next token is )
	if (jackTokenizer.tokenType() != TokenType::SYMBOL || jackTokenizer.symbol() != ")") {
		// expression
		compileExpression(jackTokenizer, symbolTable);

		// ,expression
		while (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == ",") {
			//,
			outputTerminal(jackTokenizer);
			jackTokenizer.advance();

			// expression
			compileExpression(jackTokenizer, symbolTable);
		}
	}

	outputNonTerminal("expressionList", false);
}