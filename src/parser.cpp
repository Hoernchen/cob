#include <iostream>
#include <map>
#include <string>


#include "parser.h"
#include "ASTGraphVisitor.h"

parser::parser(char* path) : vars(new Variables()), mylex(new lexer()), FuncTable(new map<string,Expression *>()) {
	mylex->openFile(path);
}

Expression * parser::getFunction(string name) {
    map<string,Expression *>::iterator it = FuncTable->find(name);
    if(it != FuncTable->end())
        return it->second;
    else return 0;
}

void parser::addFunction(string & name,Expression * fun) {
    if(fun) {
         FuncTable->insert(pair<string,Expression *>(name,fun));
    }
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

Expression * parser::ParseVarDec() {
    mylex->getNext(false); // Load name
    if(mylex->readLast().ty() == WORD) {
        string name=mylex->readLast().str();
        mylex->getNext(false); // Read type
        string type=mylex->readLast().str();
        if(type != "int" && type != "float") return 0;
        mylex->getNext(false);
        if(mylex->readLast().ty() == ASSIGNMENT) {
            mylex->getNext(false); // Prime ParseExpression
            Expression * temp=ParseExpression();
            if(temp) {
                myTypes ty;
                if(type=="int") ty=T_INT;
                if(type=="float") ty=T_FLOAT;
                vars->insertVar(name,temp);
                return new VariableEx(name,vars,ty);
            }
            else {
                cerr<<"var <name> <type> must be followed by assignment"<<endl;
                return 0;
            }
        }
        else return 0;
    }
}

Expression * parser::ParseIdentifExpr() {
    if(mylex->readLast().str() == "return") {
        mylex->getNext(false);
        return new ReturnEx(ParsePrimary());
    }
    std::string first=mylex->readLast().str();
    if(first == "var") {
        return ParseVarDec();
    }
    else if(mylex->getNext(false).ty() == OPEN) {
        return ParseFunctionCallExpr(first);
    }
    else if(mylex->readLast().ty() == DEFINITION) {
        mylex->getNext(false); // Prime ParseExpression
        Expression * temp=ParseExpression();
        if(temp) {
            myTypes ty=temp->getType();
            vars->insertVar(first,temp);
            return new VariableEx(first,vars,ty);
        }
        else {
            cerr<<"Implicit variable declaration must be followed by definition"<<endl;
            return 0;
        }
    }
    Expression *temp=vars->getValue(first);
    if(temp)
        return new VariableEx(first,vars,vars->getValue(first)->getType());
    else return 0;
}

Expression * parser::ParseFunctionCallExpr(string name) {
    if(!getFunction(name)) {
		cerr<<"Error at line "<< mylex->readLast().ln() << " : Function "<<name<<" does not exist"<<endl;
        exit(0);
    }
    Expression * param=ParseParenthesesExpr();
    if(param) return new FunctionCallEx(name,param,this);
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
    myTypes type=T_VOID;
    name=mylex->readLast().str();
    Expression *param=ParseParamExpr();
    mylex->getNext(false);
    if(mylex->readLast().ty() == WORD) {
        if(mylex->readLast().str() == "float") type=T_FLOAT;
        else if(mylex->readLast().str() == "int") type=T_INT;
        else {
            cerr<<"Invalid return type for function "<<name<<endl;
            return 0;
        }
        mylex->getNext(false);
    }
    if(mylex->readLast().ty() != CURLOPEN) {
        cerr<<"{} block expected after function definition"<<endl;
        return 0;
    }
    Expression * body=ParseBlockEx();
    if(body) {
        Expression * temp = new FunctionDefEx(name,param,body,type);
        addFunction(name,temp);
        return temp;
    }
    else return 0;
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
    token tok=mylex->readLast();
    while(tok.ty() == EOL) {
        tok=getNext(false);
    }

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
        tok=getNext(false);
    }
    while(tok.ty() == EOL);
    if(mylex->readLast().str() != "def") {
        cerr<<"def expected"<<endl;
        return 0;
    }
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
    token tok;
    do {
        tok=getNext(false);
    }
    while(tok.ty() == EOL);
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
		ASTGraphVisitor vis;
        ex->accept(&vis);
        return true;
    }
    return false;
}


