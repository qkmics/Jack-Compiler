#include "CompilationEngine.hpp"

CompilationEngine::CompilationEngine() : level(0), className("") ,lableNum(0), fieldNum(0){
}

CompilationEngine::~CompilationEngine() {
}

void CompilationEngine::compileClass(JackTokenizer& jackTokenizer, SymbolTable& symbolTable, VMWriter& vMWriter) {

	// class
	if (jackTokenizer.tokenType() == TokenType::KEYWORD && jackTokenizer.keyWord() == KeyWord::CLASS)
		jackTokenizer.advance();

	// class name
	if (jackTokenizer.tokenType() == TokenType::IDENTIFIER) {
		// assign name of current class
		className = jackTokenizer.identifier();

		jackTokenizer.advance();
	}

	// {
	if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == "{") 
		jackTokenizer.advance();

	// class var dec
	// static or field
	while (jackTokenizer.tokenType() == TokenType::KEYWORD &&
		(jackTokenizer.keyWord() == KeyWord::STATIC || jackTokenizer.keyWord() == KeyWord::FIELD)) {
		compileClassVarDec(jackTokenizer, symbolTable, vMWriter);
	}

	// subroutine dec
	while (jackTokenizer.tokenType() == TokenType::KEYWORD &&
		(jackTokenizer.keyWord() == KeyWord::CONSTRUCTOR || jackTokenizer.keyWord() == KeyWord::FUNCTION || jackTokenizer.keyWord() == KeyWord::METHOD)) {
		compileSubroutine(jackTokenizer, symbolTable, vMWriter);
	}

	// }
	if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == "}")
		jackTokenizer.advance();

}

void CompilationEngine::compileClassVarDec(JackTokenizer& jackTokenizer, SymbolTable& symbolTable, VMWriter& vMWriter) {

	string name, type;
	IdentifierKind kind = IdentifierKind::UNKNOWN;

	//static or field
	if (jackTokenizer.tokenType() == TokenType::KEYWORD &&
		(jackTokenizer.keyWord() == KeyWord::STATIC || jackTokenizer.keyWord() == KeyWord::FIELD)) {

		if (jackTokenizer.keyWord() == KeyWord::STATIC)
			kind = IdentifierKind::STATIC;
		else if (jackTokenizer.keyWord() == KeyWord::FIELD)
			kind = IdentifierKind::FIELD;

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

		jackTokenizer.advance();
	}
	else if (jackTokenizer.tokenType() == TokenType::IDENTIFIER) {
		//class name

		type = jackTokenizer.identifier();

		jackTokenizer.advance();
	}

	// var name
	if (jackTokenizer.tokenType() == TokenType::IDENTIFIER) {

		name = jackTokenizer.identifier();
		symbolTable.define(name, type, kind);

		if (kind == IdentifierKind::FIELD)
			this->fieldNum++;

		jackTokenizer.advance();
	}

	// ,varname
	while (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == ",") {
		// ,
		jackTokenizer.advance();

		// var name
		if (jackTokenizer.tokenType() == TokenType::IDENTIFIER) {

			name = jackTokenizer.identifier();
			symbolTable.define(name, type, kind);

			jackTokenizer.advance();
		}

		if (kind == IdentifierKind::FIELD)
			this->fieldNum++;
	}

	// ;
	if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == ";") 
		jackTokenizer.advance();
}

void CompilationEngine::compileSubroutine(JackTokenizer& jackTokenizer, SymbolTable& symbolTable, VMWriter& vMWriter) {

	// store type of current subroutine
	KeyWord subroutineType = KeyWord::FUNCTION;

	// let symbol table know that a new subroutine started
	symbolTable.startSubroutine();

	// constructor or function or method
	if (jackTokenizer.tokenType() == TokenType::KEYWORD &&
		(jackTokenizer.keyWord() == KeyWord::CONSTRUCTOR ||
			jackTokenizer.keyWord() == KeyWord::FUNCTION ||
			jackTokenizer.keyWord() == KeyWord::METHOD)) {

		// for method, we need to add "this" as a argument
		if (jackTokenizer.keyWord() == KeyWord::METHOD) {
			string name = "this";
			symbolTable.define(name,className,IdentifierKind::ARG);
		}
		
		// note the type of this subroutine
		if (jackTokenizer.keyWord() == KeyWord::CONSTRUCTOR) {
			subroutineType = KeyWord::CONSTRUCTOR;
		}
		else if (jackTokenizer.keyWord() == KeyWord::METHOD) {
			subroutineType = KeyWord::METHOD;
		}
		jackTokenizer.advance();
	}

	// type
	if (jackTokenizer.tokenType() == TokenType::KEYWORD && jackTokenizer.keyWord() == KeyWord::VOID) {
		// void
		jackTokenizer.advance();
	}
	else if (jackTokenizer.tokenType() == TokenType::KEYWORD &&
			(jackTokenizer.keyWord() == KeyWord::INT || jackTokenizer.keyWord() == KeyWord::CHAR || jackTokenizer.keyWord() == KeyWord::BOOLEAN)) {
		// int, char or boolean
		jackTokenizer.advance();
	}
	else if (jackTokenizer.tokenType() == TokenType::IDENTIFIER) {
		//class name
		jackTokenizer.advance();
	}

	string subroutineName;

	// subroutine name
	if (jackTokenizer.tokenType() == TokenType::IDENTIFIER) {
		subroutineName = jackTokenizer.identifier();

		jackTokenizer.advance();
	}

	// (
	if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == "(")
		jackTokenizer.advance();

	// parameter list
	compileParameterList(jackTokenizer, symbolTable, vMWriter);

	// )
	if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == ")")
		jackTokenizer.advance();

	// subroutine body
	// {
	if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == "{")
		jackTokenizer.advance();

	int nLocals = 0;

	// var dec *
	while (jackTokenizer.tokenType() == TokenType::KEYWORD && jackTokenizer.keyWord() == KeyWord::VAR)
		compileVarDec(jackTokenizer, symbolTable, vMWriter, nLocals);
		
	// write function
	vMWriter.writeFunction(this->className + "." + subroutineName, nLocals);

	if (subroutineType == KeyWord::CONSTRUCTOR) {
		// for constructor, need to do "call Memory.alloc 1" at first

		//push number of field variable in current class	
		vMWriter.writePush(Segment::CONST, this->fieldNum);
		//call Memory.alloc 1
		vMWriter.writeCall("Memory.alloc", 1);
		//pop pointer 0
		vMWriter.writePop(Segment::POINTER, 0);
	}
	else if (subroutineType == KeyWord::METHOD) {
		// for methods, need to assign the value of "this" first

		//push argument 0
		vMWriter.writePush(Segment::ARG, 0);
		//pop pointer 0
		vMWriter.writePop(Segment::POINTER, 0);
	}

	compileStatements(jackTokenizer, symbolTable, vMWriter);

	// }
	if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == "}")
		jackTokenizer.advance();
}

void CompilationEngine::compileParameterList(JackTokenizer& jackTokenizer, SymbolTable& symbolTable, VMWriter& vMWriter) {

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

		jackTokenizer.advance();
	}
	else if (jackTokenizer.tokenType() == TokenType::IDENTIFIER) {
		//class name

		type = jackTokenizer.identifier();

		jackTokenizer.advance();
	}

	// var name
	if (jackTokenizer.tokenType() == TokenType::IDENTIFIER) {

		name = jackTokenizer.identifier();
		symbolTable.define(name, type, kind);
		
		jackTokenizer.advance();
	}

	// , type var name
	while (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == ",") {
		// ,
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

			jackTokenizer.advance();
		}
		else if (jackTokenizer.tokenType() == TokenType::IDENTIFIER) {
			//class name

			type = jackTokenizer.identifier();

			jackTokenizer.advance();
		}

		// var name
		if (jackTokenizer.tokenType() == TokenType::IDENTIFIER) {

			name = jackTokenizer.identifier();
			symbolTable.define(name, type, kind);
		
			jackTokenizer.advance();
		}
	}
}

void CompilationEngine::compileVarDec(JackTokenizer& jackTokenizer, SymbolTable& symbolTable, VMWriter& vMWriter, int& varCount) {

	string name, type;
	IdentifierKind kind = IdentifierKind::VAR;

	// var
	if (jackTokenizer.tokenType() == TokenType::KEYWORD && jackTokenizer.keyWord() == KeyWord::VAR) {
		
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
		
		jackTokenizer.advance();
	}
	else if (jackTokenizer.tokenType() == TokenType::IDENTIFIER) {
		//class name

		type = jackTokenizer.identifier();
		
		jackTokenizer.advance();
	}

	// var name
	if (jackTokenizer.tokenType() == TokenType::IDENTIFIER) {
		varCount++;
		
		name = jackTokenizer.identifier();
		symbolTable.define(name, type, kind);
	
		jackTokenizer.advance();
	}

	// , var name
	while (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == ",") {
		// ,
		
		jackTokenizer.advance();
		
		// var name
		if (jackTokenizer.tokenType() == TokenType::IDENTIFIER) {

			name = jackTokenizer.identifier();
			symbolTable.define(name, type, kind);
			
			jackTokenizer.advance();
		}
		varCount++;
	}

	// ;
	if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == ";")
		
		jackTokenizer.advance();
}

void CompilationEngine::compileStatements(JackTokenizer& jackTokenizer, SymbolTable& symbolTable, VMWriter& vMWriter) {

	// let or if or while or do or return
	while (jackTokenizer.tokenType() == TokenType::KEYWORD 
		  && (jackTokenizer.keyWord() == KeyWord::LET || jackTokenizer.keyWord() == KeyWord::IF 
	      || jackTokenizer.keyWord() == KeyWord::WHILE || jackTokenizer.keyWord() == KeyWord::DO 
		  || jackTokenizer.keyWord() == KeyWord::RETURN)) {
		if (jackTokenizer.keyWord() == KeyWord::LET) {
			compileLet(jackTokenizer, symbolTable, vMWriter);
		}
		else if (jackTokenizer.keyWord() == KeyWord::IF) {
			compileIf(jackTokenizer, symbolTable, vMWriter);
		}
		else if (jackTokenizer.keyWord() == KeyWord::WHILE) {
			compileWhile(jackTokenizer, symbolTable, vMWriter);
		}
		else if (jackTokenizer.keyWord() == KeyWord::DO) {
			compileDo(jackTokenizer, symbolTable, vMWriter);
		}
		else if (jackTokenizer.keyWord() == KeyWord::RETURN) {
			compileReturn(jackTokenizer, symbolTable, vMWriter);
		}
	}
}

void CompilationEngine::compileDo(JackTokenizer& jackTokenizer, SymbolTable& symbolTable, VMWriter& vMWriter) {
	
	int argNume = 0;
	
	// do
	if (jackTokenizer.tokenType() == TokenType::KEYWORD && jackTokenizer.keyWord() == KeyWord::DO) {
		
		jackTokenizer.advance();
	}

	string name,className, subroutineName;
	int nArgs = 0;

	// subroutine call
	// subroutine name or class name or var name
	if (jackTokenizer.tokenType() == TokenType::IDENTIFIER) { 

		// var name or class name or subroutine name
		name = jackTokenizer.identifier();

		// if it is a var name, this is a method call
		// push this var to the stack as the first argument
		if (symbolTable.kindOf(name) != IdentifierKind::UNKNOWN) {
			nArgs = 1;
			vMWriter.writePush(identifierKindToSeg(symbolTable.kindOf(name)), symbolTable.indexOf(name));
			className = symbolTable.typeOf(name);
		}	
		
		
		jackTokenizer.advance();
	}

	// .subroutine name
	// .
	if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == ".") {
		
		jackTokenizer.advance();

		// className.subroutineName()
		if(className.empty())
			className = name;

		// subroutine name
		if (jackTokenizer.tokenType() == TokenType::IDENTIFIER) {

			subroutineName = jackTokenizer.identifier();

			
			jackTokenizer.advance();
		}
	}

	// no explicit class name
	// it is also a method call, push this var to the stack as the first argument
	if (className.empty()) {
		nArgs = 1;
		vMWriter.writePush(Segment::POINTER, 0);

		className = this->className;
		subroutineName = name;
	}

	// ( expression list )
	// (
	if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == "(") {
		
		jackTokenizer.advance();

		// count the number of expressions
		int expressionCount = 0;

		// expression list
		compileExpressionList(jackTokenizer, symbolTable, vMWriter, expressionCount);

		nArgs += expressionCount;

		// )
		if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == ")")
			jackTokenizer.advance();
	}

	// ;
	if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == ";")
		jackTokenizer.advance();

	// write function or method call
	vMWriter.writeCall(className + "." + subroutineName, nArgs);

	// the return value will not be used in  a do statement
	vMWriter.writePop(Segment::TEMP, 0);
}

void CompilationEngine::compileLet(JackTokenizer& jackTokenizer, SymbolTable& symbolTable, VMWriter& vMWriter) {
	/*
	arr [exp1] = exp2
	
	push arr
	(vm code for exp1)
	add
	(vm code for exp2)
	pop temp 0
	pop pointer 1
	push temp 0
	pop that 0
	*/
	bool isArray = false;
	string name;
	IdentifierKind kind = IdentifierKind::UNKNOWN;
	int index = -1;

	// let
	if (jackTokenizer.tokenType() == TokenType::KEYWORD && jackTokenizer.keyWord() == KeyWord::LET) {
		
		jackTokenizer.advance();
	}

	// var name
	// identifier
	if (jackTokenizer.tokenType() == TokenType::IDENTIFIER) {
		
		name = jackTokenizer.identifier();
		kind = symbolTable.kindOf(name);
		index = symbolTable.indexOf(name);

		jackTokenizer.advance();
	}

	// [
	if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == "[") {

		// it is an array
		isArray = true;

		// push arr
		vMWriter.writePush(identifierKindToSeg(kind),index);

		jackTokenizer.advance();

		// expression
		compileExpression(jackTokenizer, symbolTable, vMWriter);

		// ]
		if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == "]")			
			jackTokenizer.advance();

		// add
		vMWriter.writeArithmetic(Command::ADD);
	}
	
	// =
	if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == "=")
		jackTokenizer.advance();

	// expression
	compileExpression(jackTokenizer, symbolTable, vMWriter);

	// ;
	if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == ";")	
		jackTokenizer.advance();

	if (isArray) {
		
		// pop temp 0
		vMWriter.writePop(Segment::TEMP, 0);

		// pop pointer 1
		vMWriter.writePop(Segment::POINTER, 1);

		//push temp 0
		vMWriter.writePush(Segment::TEMP, 0);

		//pop that 0
		vMWriter.writePop(Segment::THAT, 0);	
	}
	else {

		// pop varName
		vMWriter.writePop(identifierKindToSeg(kind), index);
	}
}

void CompilationEngine::compileWhile(JackTokenizer& jackTokenizer, SymbolTable& symbolTable, VMWriter& vMWriter) {
	/*
	while (expression)		    label L1
		statements				compiled (expression)
								not
								if-goto L2
								compiled (statements)
								goto L1
								label L2
	*/

	// store current lableNum
	int thisLableNum = lableNum;
	
	// increase lable number
	lableNum++;

	// label L1
	vMWriter.writeLabel(className + "L1" + to_string(thisLableNum));

	// while
	if (jackTokenizer.tokenType() == TokenType::KEYWORD && jackTokenizer.keyWord() == KeyWord::WHILE) {
		
		jackTokenizer.advance();
	}

	// (
	if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == "(")		
		jackTokenizer.advance();

	// expression
	compileExpression(jackTokenizer, symbolTable, vMWriter);

	// )
	if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == ")")		
		jackTokenizer.advance();

	// not
	vMWriter.writeArithmetic(Command::NOT);

	// if - goto L2
	vMWriter.writeIf(className + "L2" + to_string(thisLableNum));

	// {
	if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == "{")
		jackTokenizer.advance();

	// statements
	compileStatements(jackTokenizer, symbolTable, vMWriter);

	// }
	if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == "}")	
		jackTokenizer.advance();

	// ;
	if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == ";")	
		jackTokenizer.advance();

	// goto L1
	vMWriter.writeGoto(className + "L1" + to_string(thisLableNum));

	// label L2
	vMWriter.writeLabel(className + "L2" + to_string(thisLableNum));
}

void CompilationEngine::compileReturn(JackTokenizer& jackTokenizer, SymbolTable& symbolTable, VMWriter& vMWriter) {
	// return
	if (jackTokenizer.tokenType() == TokenType::KEYWORD && jackTokenizer.keyWord() == KeyWord::RETURN) {
		
		jackTokenizer.advance();
	}

	//expression
	if (jackTokenizer.tokenType() != TokenType::SYMBOL || jackTokenizer.symbol() != ";")
		compileExpression(jackTokenizer, symbolTable, vMWriter);
	else {
		// for subroutine does not return a value, push 0 to the stack
		// this is because the protocal of our stack machine

		vMWriter.writePush(Segment::CONST, 0);
	}
		
	// ;
	if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == ";") {
		
		jackTokenizer.advance();
	}

	vMWriter.writeReturn();
}

void CompilationEngine::compileIf(JackTokenizer& jackTokenizer, SymbolTable& symbolTable, VMWriter& vMWriter) {
	/*
	if (expression)					compiled(expression)
		statements1					not
	else   							if - goto L1
		statements2					compiled(statements1)
									goto L2
									label L1
									compiled(statements2)
									label L2
	*/

	// store current lableNum
	int thisLableNum = lableNum;

	// increase lable number
	lableNum++;

	// if
	if (jackTokenizer.tokenType() == TokenType::KEYWORD && jackTokenizer.keyWord() == KeyWord::IF) {
		jackTokenizer.advance();
	}

	// (
	if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == "(")	
		jackTokenizer.advance();

	// expression
	compileExpression(jackTokenizer, symbolTable, vMWriter);

	// )
	if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == ")")	
		jackTokenizer.advance();

	// not
	vMWriter.writeArithmetic(Command::NOT);

	// if - goto L1
	vMWriter.writeIf(className + "L1" + to_string(thisLableNum));

	// {
	if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == "{")	
		jackTokenizer.advance();

	// statements
	compileStatements(jackTokenizer, symbolTable, vMWriter);

	// }
	if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == "}")	
		jackTokenizer.advance();

	// goto L2
	vMWriter.writeGoto(className + "L2" + to_string(thisLableNum));

	// label L1
	vMWriter.writeLabel(className + "L1" + to_string(thisLableNum));

	// else
	if (jackTokenizer.tokenType() == TokenType::KEYWORD && jackTokenizer.keyWord() == KeyWord::ELSE) {		
		jackTokenizer.advance();

		// {
		if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == "{")	
			jackTokenizer.advance();

		// statements
		compileStatements(jackTokenizer, symbolTable, vMWriter);

		// }
		if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == "}")	
			jackTokenizer.advance();
	}

	// ;
	if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == ";") 	
		jackTokenizer.advance();

	// label L2
	vMWriter.writeLabel(className + "L2" + to_string(thisLableNum));
}

bool CompilationEngine::isOp(string symbol)
{
	return (symbol == "+" || symbol == "-" || symbol == "*" || symbol == "/" || symbol == "&"
		|| symbol == "|" || symbol == "<" || symbol == ">" || symbol == "=");
}

void CompilationEngine::compileExpression(JackTokenizer& jackTokenizer, SymbolTable& symbolTable, VMWriter& vMWriter) {
	// term
	compileTerm(jackTokenizer, symbolTable, vMWriter);

	string op;

	// op term
	while (jackTokenizer.tokenType() == TokenType::SYMBOL && isOp(jackTokenizer.symbol())) {
		op = jackTokenizer.symbol();

		jackTokenizer.advance();

		compileTerm(jackTokenizer, symbolTable, vMWriter);

		if (op == "*") {
			vMWriter.writeCall("Math.multiply", 2);
		}
		else if (op == "/") {
			vMWriter.writeCall("Math.divide", 2);
		}
		else {
			// other binary operations
			vMWriter.writeArithmetic(operationToCommand(op));
		}
	}
}

void CompilationEngine::compileTerm(JackTokenizer& jackTokenizer, SymbolTable& symbolTable, VMWriter& vMWriter) {
	if (jackTokenizer.tokenType() == TokenType::INT_CONSTANT) {
		// integer constant

		// push this constant
		vMWriter.writePush(Segment::CONST, stoi(jackTokenizer.intVal()));

		
		jackTokenizer.advance();
	}
	else if (jackTokenizer.tokenType() == TokenType::STRING_CONSTANT) {	
		// string constant

		// push length
		vMWriter.writePush(Segment::CONST, jackTokenizer.stringVal().size());

		// call String.new(length)
		vMWriter.writeCall("String.new", 1);

		for (char c : jackTokenizer.stringVal()) {
			
			// push charactor
			vMWriter.writePush(Segment::CONST, c);

			// call  String.appendchar(nextChar), the first argument is string address
			vMWriter.writeCall("String.appendChar", 2);
		}

		
		jackTokenizer.advance();
	}
	else if (jackTokenizer.tokenType() == TokenType::KEYWORD) {
		// keyWord constant
		
		if (jackTokenizer.keyWord() == KeyWord::THIS) {
			// this
			vMWriter.writePush(Segment::POINTER, 0);
		}
		else if (jackTokenizer.keyWord() == KeyWord::FALSE || jackTokenizer.keyWord() == KeyWord::NULL_PTR) {
			// false, null
			vMWriter.writePush(Segment::CONST, 0);
		}
		else if (jackTokenizer.keyWord() == KeyWord::TRUE) {
			// true
			vMWriter.writePush(Segment::CONST, 1);
			vMWriter.writeArithmetic(Command::NEG);
		}
			
		jackTokenizer.advance();
	}
	else if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == "(") {
		// (expression)
		
		// (
		
		jackTokenizer.advance();

		// expression
		compileExpression(jackTokenizer, symbolTable, vMWriter);

		// )
		if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == ")") {
			
			jackTokenizer.advance();
		}
	}
	else if (jackTokenizer.tokenType() == TokenType::SYMBOL &&
		(jackTokenizer.symbol() == "-" || jackTokenizer.symbol() == "~")) {
		// unary operation term

		string unaryOp = jackTokenizer.symbol();

		// unary operation
		
		jackTokenizer.advance();

		// term
		compileTerm(jackTokenizer, symbolTable, vMWriter);

		if (unaryOp == "-")
			vMWriter.writeArithmetic(Command::NEG);
		else 
			vMWriter.writeArithmetic(Command::NOT);
	}
	else if (jackTokenizer.tokenType() == TokenType::IDENTIFIER) {
		// var name or var name [ expression ] or subroutine call
		// if it is a subroutine call, it can be var name, class name, or subroutine name

		string name = jackTokenizer.identifier(), className, subroutineName;
		bool isArray = false;
		IdentifierKind kind = IdentifierKind::UNKNOWN;
		int index = -1, nArgs = 0;

		// var name or var name [ expression ] or method call
		// push this var to the stack
		if (symbolTable.kindOf(name) != IdentifierKind::UNKNOWN) {
			nArgs = 1;
			vMWriter.writePush(identifierKindToSeg(symbolTable.kindOf(name)), symbolTable.indexOf(name));
			className = symbolTable.typeOf(name);
		}
		
		jackTokenizer.advance();

		if (jackTokenizer.tokenType() == TokenType::SYMBOL) {
			// var name [ expression ] or subroutine call
	
			if(jackTokenizer.symbol() == "[") {
				// [ expression ]

				// [
				
				jackTokenizer.advance();

				// expression
				compileExpression(jackTokenizer, symbolTable, vMWriter);

				// ]
				if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == "]") {
					
					jackTokenizer.advance();
				}

				// add
				vMWriter.writeArithmetic(Command::ADD);

				// pop pointer 1
				vMWriter.writePop(Segment::POINTER, 1);
		
				// push that 0
				vMWriter.writePush(Segment::THAT, 0);
			}
			else if(jackTokenizer.symbol() == "."){
				// subroutine call

				// .subroutine name
				// .
				if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == ".") {
					
					jackTokenizer.advance();

					// className.subroutineName()
					if (className.empty())
						className = name;

					// subroutine name
					if (jackTokenizer.tokenType() == TokenType::IDENTIFIER) {

						subroutineName = jackTokenizer.identifier();

						jackTokenizer.advance();
					}
				}

				// no explicit class name
				if (className.empty()) {
					className = this->className;
					subroutineName = name;
				}

				// ( expression list )
				// (
				if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == "(") {
					
					jackTokenizer.advance();

					// count the number of expressions
					int expressionCount = 0;

					// expression list
					compileExpressionList(jackTokenizer, symbolTable, vMWriter, expressionCount);

					// )
					if (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == ")") {
						
						jackTokenizer.advance();
					}

					nArgs += expressionCount;
				}
			
				// write function or method call
				vMWriter.writeCall(className + "." + subroutineName, nArgs);
			}

		}
	}
}

void CompilationEngine::compileExpressionList(JackTokenizer& jackTokenizer, SymbolTable& symbolTable, VMWriter& vMWriter,int& expressionCount) {
	// expression list is empty only if next token is )
	if (jackTokenizer.tokenType() != TokenType::SYMBOL || jackTokenizer.symbol() != ")") {
		// expression
		compileExpression(jackTokenizer, symbolTable, vMWriter);

		// increase expression count
		expressionCount++;

		// ,expression
		while (jackTokenizer.tokenType() == TokenType::SYMBOL && jackTokenizer.symbol() == ",") {
			//,
			
			jackTokenizer.advance();

			// expression
			compileExpression(jackTokenizer, symbolTable, vMWriter);

			// increase expression count
			expressionCount++;
		}
	}
}

Segment CompilationEngine::identifierKindToSeg(IdentifierKind kind) {
	Segment seg = Segment::ARG;;
	if (kind == IdentifierKind::ARG) {
		seg = Segment::ARG;
	}
	else if (kind == IdentifierKind::FIELD) {
		seg = Segment::THIS;
	}
	else if (kind == IdentifierKind::STATIC) {
		seg = Segment::STATIC;
	}
	else if (kind == IdentifierKind::VAR) {
		seg = Segment::LOCAL;
	}
	else if (kind == IdentifierKind::UNKNOWN) {
		cout << "kind is unknown" << endl;
	}
	return seg;
}

Command CompilationEngine::operationToCommand(string op) {
	Command command = Command::ADD;;

	if (op == "+") {
		command = Command::ADD;
	}
	else if (op == "-") {
		command = Command::SUB;
	}
	else if (op == "&") {
		command = Command::AND;
	}
	else if (op == "|") {
		command = Command::OR;
	}
	else if (op == "<") {
		command = Command::LT;
	}
	else if (op == ">") {
		command = Command::GT;
	}
	else if (op == "=") {
		command = Command::EQ;
	}
	else {
		cout << "operationToCommand" << " " << op << " not valid" << endl;
	}
	return command;
}

int CompilationEngine::keyWordConstToConst(KeyWord keyWordConst) {
	int constVal = 0;

	if(keyWordConst == KeyWord::TRUE) {
		constVal = -1;
	}
	else if (keyWordConst == KeyWord::FALSE) {
		constVal = 0;
	}
	else if (keyWordConst == KeyWord::NULL_PTR) {
		constVal = 0;
	}
	else {
		cout << "keyWordConst not valid " << endl;
	}

	return constVal;
}