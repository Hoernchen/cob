#include <iostream>
#include <map>
#include <string>
#include <stdexcept>

#include "parser.h"

parser::parser(char* path) : vars(new map<string,float>()), mylex(new lexer()) {
	mylex->openFile(path);
}

parser::~parser() {
	if(vars) delete vars;
	delete mylex;
}

token parser::getNext(bool forceNextLine=false) {
	token t = mylex->getNext(forceNextLine);
	 if(!t())
		 cout<< "fail at line:" << t.ln() <<endl;
	return t;
}

Expression * parser::ParseNumberExpr() {
	Expression *res=new NumberEx(stof(mylex->readLast().text));
	mylex->getNext(false);
	return res;
}

Expression * parser::ParseExpression() {
	Expression *lhs=ParsePrimary();
	if(lhs==0) return 0;
	return ParseBinRHS(lhs);
}

Expression * parser::ParseBinRHS(Expression *LHS) {
	while(true) {
		char Operator=mylex->readLast().text[0];
		if(mylex->readLast().type != OPERATOR) return LHS;
		mylex->getNext(false);
		Expression *rhs=ParsePrimary();
		if(rhs==0) return 0;
		LHS=new BinaryExprEx(Operator,LHS,rhs);
	}
}

Expression * parser::ParseParenthesesExpr() {
	mylex->getNext(false);
	Expression *ex=ParseExpression();
	if(ex==0) return 0;
	if(mylex->readLast().type != CLOSE) {
		cout<<"Close parentheses plz"<<endl;
		return 0;
	}
	mylex->getNext(false);
	return ex;
}

Expression *parser::ParsePrimary() {
	switch(mylex->readLast().type) {
		case NUMBER:
			return ParseNumberExpr();
			break;
		case VAR:
			// return ParseIdentifExpr();
			break;
		case OPEN:
			return ParseParenthesesExpr();
			break;
		default:
			return 0;
	}
}

void parser::processLine() {
	float temp;
	token tok=getNext();
	Expression *ex;
	switch(tok.ty()) {
		case VAR:
			// cout<<handleVar()<<endl;
			break;
		case NUMBER:
			ex=ParseExpression();
			if(ex) cout<<"Result: "<<ex->getValue()<<endl;
			break;
		case ASSIGNMENT:
			cout<<"Line must not start with assignment operator"<<endl;
			mylex->newLine();
			break;
		case OPERATOR:
			cout<<"Line must not start with operator"<<endl;
			mylex->newLine();
			break;
		case OPEN:
			ex=ParseExpression();
			if(ex) cout<<"Result: "<<ex->getValue()<<endl;
			break;
		case CLOSE:
			cout<<"Line must not start with closing bracket"<<endl;
			mylex->newLine();
			break;
		case END:
			cout<<"END OF FILE REACHED"<<endl;
			exit(0);
			break;
	}
}

int main(int argc, char* argv[]) {
	parser * myParse=new parser(argv[1]);
	while(true) 
	myParse->processLine();
	return 0;
}
