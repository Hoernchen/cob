#pragma once

#include <map>
#include <vector>
#include <iostream>
#include <string>

#include "lex.h"
#include "IVisitor.h"
//#include "CodegenVisitor.h"
//#include "ASTGraphVisitor.h"


using namespace std;

enum myTypes {T_INT,T_FLOAT,T_VOID};
static char* TypeNames[] ={"int","float","void"};

class Expression {
public:
	static int index;

	virtual ~Expression() {}
	virtual float getValue() const =0;
	virtual myTypes getType() const =0;
	virtual void accept(IVisitor* v) const =0;
	static int getIndex() {return index;};
    virtual bool isRet() { return false;};
    virtual bool isCond() { return false;};
};

class Variables {
    std::map<string,Expression *> * vars;
public:
	Variables() : vars(new map<string,Expression *>()) {};
	~Variables()  { delete vars;};
	void insertVar(string & name, Expression * val) {
		vars->insert(pair<string,Expression *>(name,val));
	}
	Expression * getValue(const string & name) const {
		map<string,Expression *>::const_iterator it = vars->find(name);
		if(it != vars->end())
			return it->second;
		else return 0;
	}
};

class parser {
    map<string,Variables *> * VarTables;
    Variables * vars;
	lexer * mylex;
	token currentToken;
	map<string,Expression *> * FuncTable;
	// float result;
	float operand;
    std::map<char, int> BinopPrecedence;

	//bool isVar(token &);
	//bool isNumber(token &);
	//bool isOperator(token &);
	//bool isAssignment(token &);
	//bool isEOL(token &);
	//bool isEnd(token &);
	token nextToken();
	float doMath(float);
	float resolveVar(string);
	float parentheses();
	float handleVar();
    int getPrecedence();
    void updateTable(string name) {
        Variables * temp = new Variables();
        VarTables->insert(make_pair(name,temp));
        this->vars=temp;
    }

	Expression *ParseExpression();
	Expression *ParsePrimary();
	Expression *ParseNumberExpr();
    Expression *ParseBinRHS(int prec, Expression *);
	Expression *ParseParenthesesExpr();
	Expression *ParseIdentifExpr();
	Expression *ParseFunctionCallExpr(string);
	Expression * ParseDefFunctionExpr();
	Expression * ParseParamExpr();
	Expression * ParsePackage();
	Expression * ParseDef();
	Expression * ParseBlockEx();
	Expression * ParseVarDec();
    Expression * ParseCondExpr();

	void addFunction(string & name,Expression * fun);


public:
	Expression * getFunction(string name);
	parser(char* path);
	~parser();
	token getNext(bool);
	Expression* parseFile();
};


class PackageEx : public Expression {
public:
	std::string name;
	vector<Expression *> * body;

	PackageEx(std::string name) : name(name), body(new vector<Expression *>()) {}
	~PackageEx()  { delete body; }
	void addLine(Expression * p) { if(p != 0) body->push_back(p); }
	myTypes getType() const override { return T_VOID; }
	float getValue() const override {
		return 0;
	}
	void accept(IVisitor* v) const override { v->visit(this); };
};

class ConditionalEx : public Expression {
public:

    Expression * LHS;
    Expression * RHS;
    Expression *body;
    Expression * elsedo;
    tokenType op;


    ConditionalEx(Expression *LHS, Expression *RHS, Expression *body, Expression *elsedo, tokenType op) : LHS(LHS), RHS(RHS), body(body), elsedo(elsedo), op(op) { cerr<<this->elsedo<<" "<<this->body<<endl;}
    ~ConditionalEx()  {}
    myTypes getType() const override { return T_VOID; }
    float getValue() const override {
        return 0;
    }
    virtual bool isCond() override { return true;};
    void accept(IVisitor* v) const override { v->visit(this); };
};


class NumberEx : public Expression {
public:
	float value;
    myTypes type;

    NumberEx(float val, myTypes type) : value(val), type(type) {};
    myTypes getType() const override { return type; }
    float getValue() const override {return value;}
    int getIntValue() const {return (int) value;}
	void accept(IVisitor* v) const override { v->visit(this); };
};

class VariableEx : public Expression {
public:
	const std::string name;
	Variables * vars;
	const myTypes type;
    const bool def;

    VariableEx(std::string varname, Variables * var,myTypes type,bool def=false) : name(varname), vars(var),type(type),def(def) {}
	myTypes getType() const override { return type; }
	float getValue() const override {
        Expression * temp=vars->getValue(name);
        if(temp)
			return temp->getValue(); // Resolve to a number
        else
			return 0;
	}
    Expression * getEx() const { return vars->getValue(name); }
	void accept(IVisitor* v) const override { v->visit(this); };
};

class ReturnEx : public Expression {
public:
	Expression *val;

	ReturnEx(Expression * val) : val(val) {}
	myTypes getType() const override { return val->getType(); }
	float getValue() const override {
        if(val) return val->getValue(); // Resolve
        else return 0;
	}
	void accept(IVisitor* v) const override { v->visit(this); };
    virtual bool isRet() override { return true;};
};


class BinaryExprEx : public Expression {
public:
	char OP;
	Expression *LHS;
	Expression *RHS;

	BinaryExprEx(char op, Expression *lhs, Expression * rhs) : OP(op), LHS(lhs), RHS(rhs) {}
	myTypes getType() const override { return LHS->getType(); }
	float getValue() const override {
		if(RHS == 0 || LHS == 0) return 0;
		switch(OP) {
		case '+':
			return LHS->getValue()+RHS->getValue();
			break;
		case '-':
			return LHS->getValue()-RHS->getValue();
			break;
		case '*':
			return LHS->getValue()*RHS->getValue();
			break;
		case '/':
			return LHS->getValue()/RHS->getValue();
			break;
		}
		return 0;
	}
	void accept(IVisitor* v) const override { v->visit(this); };
};


class ParamEx : public Expression {
public:
	std::string type;
	std::string name;

	ParamEx(std::string name, std::string type) : type(type), name(name) {}
	myTypes getType() const override {
		if(type=="int") return T_INT;
		if(type=="float") return T_FLOAT;
        else return T_VOID;
	}
	float getValue() const override {
		return 0;
	}
	void accept(IVisitor* v) const override { v->visit(this); };
};

class BlockEx : public Expression {
public:
	vector<Expression *> * body;

    BlockEx() : body(new vector<Expression *>()) {};
    ~BlockEx() { delete body; };
    void addLine(Expression * p) { if(p != 0) body->push_back(p); }
    vector<ReturnEx *> getReturn() {
        vector<ReturnEx*> vr;
        for(auto i : *body) {
            if(i->isRet()) vr.push_back((ReturnEx *) i);
            else if(i->isCond()) {
                BlockEx * innerbody=(BlockEx *) ((ConditionalEx *) i)->body;
                for(auto j : *(innerbody->body)) {
                    if(j->isRet()) vr.push_back((ReturnEx *) j);
                }
                BlockEx * elsebody=(BlockEx *) ((ConditionalEx *) i)->elsedo;
                if(elsebody) {
                    for(auto j : *(elsebody->body)) {
                        if(j->isRet()) vr.push_back((ReturnEx *) j);
                    }
                }
            }
        }
        return vr;
    }

	myTypes getType() const override { return T_VOID; }
	float getValue() const override {return 0;}
	void accept(IVisitor* v) const override { v->visit(this); };
};


class FunctionDefEx : public Expression {
public:
    Variables * vars;
	myTypes type;
	string name;
	float value;
	Expression *param;
	Expression * body;

    FunctionDefEx(string p_name,Expression *p_param,Expression * body, myTypes type) : vars(new Variables()), type(type), name(p_name), value(0), param(p_param), body(body) {};
	myTypes getType() const override { return type; }
	float getValue() const override {return value;}
	void accept(IVisitor* v) const override { v->visit(this); };
};

class FunctionCallEx : public Expression {
public:
	string name;
	float value;
	Expression *param;
	parser * p;

	FunctionCallEx(string p_name,Expression *p_param, parser * p) : name(p_name),value(0),param(p_param),p(p) {};
	myTypes getType() const override {
		Expression * temp = p->getFunction(name);
		if(temp) return temp->getType();
		else return T_VOID;
	}

	float getValue() const override {return value;}
	void accept(IVisitor* v) const override { v->visit(this); };
};
