#include <iostream>
#include <map>
#include <string>


#include "parser.h"

parser::parser(char* path) : mylex(new lexer()), FuncTable(new map<string,Expression *>()), VarTables(new map<string,Variables *>()) {

    BinopPrecedence['+'] = 10;
    BinopPrecedence['-'] = 10;
    BinopPrecedence['*'] = 20;
    BinopPrecedence['/'] = 20;

    mylex->openFile(path);
}

int parser::getPrecedence() {
    if(mylex->readLast().ty() != OPERATOR) return -1;
    else return BinopPrecedence[mylex->readLast().str()[0]];
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
	delete mylex;
}

token parser::getNext(bool forceNextLine=false) {
	token t = mylex->getNext(forceNextLine);
	 if(!t())
		 cerr<< "fail at line:" << t.ln() <<endl;
	return t;
}

Expression * parser::ParseNumberExpr() {
    myTypes type;
    if(mylex->readLast().ty() == TOK_FLOAT) type=T_FLOAT;
    if(mylex->readLast().ty() == TOK_INT) type=T_INT;
    Expression *res=new NumberEx(stof(mylex->readLast().str()),type);
	mylex->getNext(false);
	return res;
}

Expression * parser::ParseVarDec() {
    mylex->getNext(false); // Load name

    if(mylex->readLast().ty() != WORD)
		return 0;

    string name=mylex->readLast().str();
    mylex->getNext(false); // Read type
    string type=mylex->readLast().str();

    if(type != "int" && type != "float")
		return 0;

    mylex->getNext(false);
    if(mylex->readLast().ty() != ASSIGNMENT)
		return 0;

    mylex->getNext(false); // Prime ParseExpression
    Expression * temp=ParseExpression();
    if(temp) {
        myTypes ty;
        if(type=="int") ty=T_INT;
        if(type=="float") ty=T_FLOAT;
        vars->insertVar(name,temp);
        return new VariableEx(name,vars,ty,true);
    }
    else {
        cerr<<"var <name> <type> must be followed by assignment"<<endl;
        exit(0);
    }
}

Expression * parser::ParseIdentifExpr() {
    if(mylex->readLast().str() == "return") {
        mylex->getNext(false);
        return new ReturnEx(ParseExpression());
    }

    // Conditional Block
    if(mylex->readLast().str() == "if") {
        return ParseCondExpr();
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
            return new VariableEx(first,vars,ty,true);
        }
        else {
            cerr<<"Implicit variable declaration must be followed by definition"<<endl;
            exit(0);
        }
    }
    Expression *temp=vars->getValue(first);
    return new VariableEx(first,vars,temp ? temp->getType() : T_VOID);
}

Expression * parser::ParseFunctionCallExpr(string name) {
    if(!getFunction(name)) {
		cerr<<"Error at line "<< mylex->readLast().ln() << " : Function "<<name<<" does not exist"<<endl;
        exit(0);
    }
    Expression * param=ParseParenthesesExpr();
    if(param) {

        return new FunctionCallEx(name,param,this);
    }
    else return 0; // Not a valid function call
}

Expression * parser::ParseParamExpr() {
    string type;
    string name;
    mylex->getNext(false);
    if(mylex->readLast().ty() != OPEN)
		return 0;

    mylex->getNext(false);
    if(mylex->readLast().ty() == WORD) {
        name=mylex->readLast().str();
        mylex->getNext(false);
        if(mylex->readLast().ty() == WORD) {
            type=mylex->readLast().str();
            mylex->getNext(false); // Only eat token when parameter pair is complete
        }
        else return 0; // Invalid parameter pair
    }

    if(mylex->readLast().ty() != CLOSE)
		return 0;

    else {
		myTypes t;
		if(type == "float")
			t=T_FLOAT;
		else if(type == "int")
			t=T_INT;
		else if(type == "")
			t=T_VOID;
		else{
			cerr << __FILE__ <<":"<<__LINE__ << ": huh type?" << endl;
			exit(0);
		}

        vars->insertVar(name,new NumberEx(0x0badcafe,t));
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

    updateTable(name);

    Expression *param=ParseParamExpr();
    mylex->getNext(false);
    if(mylex->readLast().ty() == WORD) {
        if(mylex->readLast().str() == "float") type=T_FLOAT;
        else if(mylex->readLast().str() == "int") type=T_INT;
        else {
            cerr<<"Invalid return type for function "<<name<<endl;
            exit(0);
        }
        mylex->getNext(false);
    }
    if(mylex->readLast().ty() != CURLOPEN) {
        cerr<<"{} block expected after function definition"<<endl;
        exit(0);
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
    return ParseBinRHS(0,lhs);
}

Expression * parser::ParseBinRHS(int prec, Expression *LHS) {
	while(true) {
        int curPrec = getPrecedence();
        if(curPrec < prec || mylex->readLast().ty() != OPERATOR) return LHS;

        char Operator=mylex->readLast().str()[0];

        mylex->getNext(false);
		Expression *rhs=ParsePrimary();
        if(rhs==0) {
            cerr<<"No expression after operator"<<endl;
            return 0;
        }
        if(curPrec < getPrecedence()) {
            rhs = ParseBinRHS(curPrec+1, rhs);
            if(!rhs) return 0;
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
		exit(0);
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
        case TOK_INT:
			return ParseNumberExpr();
			break;
        case TOK_FLOAT:
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


// FIXME
Expression * parser::ParseCondExpr() {

    Expression * thendo=0;
    Expression *elsedo=0;

    mylex->getNext(false);

    Expression *LHS=ParsePrimary();
    if(!LHS) {
        cerr<<"Missing LHS in condition"<<endl;
        exit(0);
    }

    if(mylex->readLast().ty() != EQ && mylex->readLast().ty() != GT) {
        cerr<<"Only <= and > are allowed in condition"<<endl;
        exit(0);
    }
    tokenType op=mylex->readLast().ty();
    mylex->getNext(false);
    Expression *RHS=ParsePrimary();
    if(!RHS) {
        cerr<<"Missing RHS in condition"<<endl;
        exit(0);
    }

    if(mylex->readLast().ty() == CURLOPEN) {
        thendo=ParseBlockEx();
        mylex->getNext(false);
        // if(thendo)
            // return new ConditionalEx(LHS,RHS,thendo,op);
    }
    else {
        cerr<<"Condition must be followed by body in {}"<<endl;
        exit(0);
    }

    // Else block?
    while(mylex->readLast().ty() == EOL) mylex->getNext(false);

    if(mylex->readLast().ty() == WORD && mylex->readLast().str() == "else") {
        cerr << "else block..."<<endl;
        mylex->getNext(false);
        if(mylex->readLast().ty() != CURLOPEN) {
            cerr<<"Else statement must be followed by body in {}"<<endl;
            exit(0);
        }
        elsedo=ParseBlockEx();
        mylex->getNext(false);
    }
    if(thendo)
        return new ConditionalEx(LHS,RHS,thendo,elsedo,op);
    else {
        cerr<<"Wat"<<endl;
        exit(0);
    }
}

Expression * parser::ParsePackage() {
    token tok;
    do {
        tok=getNext(false);
    }
    while(tok.ty() == EOL);
    if(mylex->readLast().str() != "package")
		return 0;

    mylex->getNext(false);
    if(mylex->readLast().ty() != WORD)
		return 0;

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

Expression* parser::parseFile() {
    Expression * ex = ParsePackage();
    return ex;
}


