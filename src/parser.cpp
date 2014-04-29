#include <iostream>
#include <map>
#include <string>
#include <stdexcept>

#include "parser.h"

parser::parser(char* path) : vars(new Variables()), mylex(new lexer()) {
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
    Expression *res=new NumberEx(stof(mylex->readLast().str()));
	mylex->getNext(false);
	return res;
}


Expression * parser::ParseIdentifExpr() {
    std::string varname=mylex->readLast().str();
	mylex->getNext(false);
    if(mylex->readLast().ty() == ASSIGNMENT) {
		mylex->getNext(false); // Prime ParseExpression
		Expression * temp=ParseExpression();
		if(temp) {
			vars->insertVar(varname,temp->getValue());
			return new VariableEx(varname,vars);
		}
		else return 0;
	}
    return new VariableEx(varname,vars);
}

Expression * parser::ParseExpression() {
	Expression *lhs=ParsePrimary();
	if(lhs==0) return 0;
	return ParseBinRHS(lhs);
}

Expression * parser::ParseBinRHS(Expression *LHS) {
	while(true) {
        char Operator=mylex->readLast().str()[0];
        if(mylex->readLast().ty() != OPERATOR) return LHS;
		mylex->getNext(false);
		Expression *rhs=ParsePrimary();
        if(rhs==0) {
            cerr<<"No expression after operator"<<endl;
            return 0;
        }
		LHS=new BinaryExprEx(Operator,LHS,rhs);
	}
}

Expression * parser::ParseParenthesesExpr() {
	mylex->getNext(false);
    Expression *ex=ParseExpression();
	if(ex==0) return 0;
    if(mylex->readLast().ty() != CLOSE) {
        cerr<<"Close parentheses plz"<<endl;
		return 0;
	}
	mylex->getNext(false);
	return ex;
}

Expression *parser::ParsePrimary() {
    switch(mylex->readLast().ty()) {
		case NUMBER:
			return ParseNumberExpr();
			break;
		case VAR:
			return ParseIdentifExpr();
			break;
		case OPEN:
			return ParseParenthesesExpr();
			break;
        default:
            break;
	}
    return 0;
}

bool parser::processLine() {
	float temp;
	token tok=getNext();
	Expression *ex;
    static int id=0;
	switch(tok.ty()) {
		case VAR:
			ex=ParseExpression();
            //if(ex) cout<<"Result: "<<ex->getValue()<<endl;
            if(ex) ex->graph(id,id,true);
			break;
		case NUMBER:
			ex=ParseExpression();
            // if(ex) cout<<"Result: "<<ex->getValue()<<endl;
            if(ex) ex->graph(id,id,true);
			break;
		case ASSIGNMENT:
            cerr<<"Line must not start with assignment operator"<<endl;
			mylex->newLine();
			break;
		case OPERATOR:
            cerr<<"Line must not start with operator"<<endl;
			mylex->newLine();
			break;
		case OPEN:
			ex=ParseExpression();
            //if(ex) cout<<"Result: "<<ex->getValue()<<endl;
            if(ex) ex->graph(id,id,true);
			break;
		case CLOSE:
            cerr<<"Line must not start with closing bracket"<<endl;
			mylex->newLine();
			break;
		case END:
            return false;
			break;
	}
    return true;
}

int main(int argc, char* argv[]) {
	parser * myParse=new parser(argv[1]);
    cout<<"graph parse_out {"<<endl;
    while(myParse->processLine());
    cout<<"}"<<endl;
	return 0;
}
