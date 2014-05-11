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
		 cerr<< "fail at line:" << t.ln() <<endl;
	return t;
}

Expression * parser::ParseNumberExpr() {
    Expression *res=new NumberEx(stof(mylex->readLast().str()));
	mylex->getNext(false);
	return res;
}

Expression * parser::ParseIdentifExpr() {
    if(mylex->readLast().str() == "return") {
        mylex->getNext(false);
        return new ReturnEx(ParsePrimary());
    }
    std::string name=mylex->readLast().str();
    mylex->getNext(false);
    if(mylex->readLast().ty() == ASSIGNMENT) {
        mylex->getNext(false); // Prime ParseExpression
        Expression * temp=ParseExpression();
        if(temp) {
            vars->insertVar(name,temp);
            return new VariableEx(name,vars);
        }
        else return 0;
    }
    else if(mylex->readLast().ty() == OPEN) {
        return ParseFunctionCallExpr(name);
    }
    return new VariableEx(name,vars);
}

Expression * parser::ParseFunctionCallExpr(string name) {
    Expression * param=ParseParenthesesExpr();
    if(param) return new FunctionCallEx(name,param);
    else return 0; // Not a valid function call
}

Expression * parser::ParseParamExpr() {
    string type;
    string name;
    mylex->getNext(false);
    if(mylex->readLast().ty() != OPEN) return 0;

    mylex->getNext(false);
    if(mylex->readLast().ty() == WORD) {
        type=mylex->readLast().str();
        mylex->getNext(false);
        if(mylex->readLast().ty() == WORD) {
            name=mylex->readLast().str();
            mylex->getNext(false); // Only eat token when parameter pair is complete
        }
        else return 0; // Invalid parameter pair
    }

    if(mylex->readLast().ty() != CLOSE) return 0;

    else {
        cerr<<"Got Param"<<endl;
        return new ParamEx(name,type);
    }
    return 0;
}

Expression * parser::ParseBlockEx() {
    BlockEx * temp=new BlockEx();
    do {
        mylex->getNext(false);
        temp->addLine(ParsePrimary());
    }
    while(mylex->readLast().ty() != CURLCLOSE && mylex->readLast().ty() != END);
    if(mylex->readLast().ty() != CURLCLOSE) return 0;
    return temp;
}

Expression * parser::ParseDefFunctionExpr() {
    string name;
    name=mylex->readLast().str();
    Expression *param=ParseParamExpr();
    mylex->getNext(false);
    if(mylex->readLast().ty() != CURLOPEN) {
        cerr<<"{} block expected after function definition"<<endl;
        return 0;
    }
    Expression * body=ParseBlockEx();
    if(body) return new FunctionDefEx(name,param,body);
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
        case WORD:
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

Expression * parser::ParseDef() {
    token tok;
    do {
        tok=getNext();
    }
    while(tok.ty() == EOL);
    if(mylex->readLast().str() != "def") {
        cerr<<"def expected"<<endl;
        return 0;
    }
    cerr<<"Yay, def"<<endl;
    mylex->getNext(false); // Prime ParseDefFunctionExpr
	switch(tok.ty()) {
        case WORD:
            return ParseDefFunctionExpr();
			break;
        default:
            return 0;
	}
}

Expression * parser::ParsePackage() {
    mylex->getNext(false);
    if(mylex->readLast().str() != "package") return 0;
    mylex->getNext(false);
    if(mylex->readLast().ty() != WORD) return 0;
    string name=mylex->readLast().str();
    PackageEx * temp=new PackageEx(name);
    Expression * ex;
    do {
        mylex->getNext(false);
        ex=ParseDef();
        temp->addLine(ex);
    }
    while(ex);
    return temp;
}

bool parser::parseFile(int & id) {
    Expression * ex = ParsePackage();
    if(ex) {
        ex->graph(id,id);
        return true;
    }
    return false;
}

int main(int argc, char* argv[]) {
    if(argc<2) {
        cerr<<"No input file given"<<endl;
        return -1;
    }
    int id=0;
	parser * myParse=new parser(argv[1]);
    myParse->parseFile(id);
	return 0;
}
