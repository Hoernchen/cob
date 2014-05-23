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

class Expression {
public:
	static int index;

	virtual ~Expression() {}
	virtual float getValue() const =0;
	virtual myTypes getType() const =0;
	virtual void accept(IVisitor* v) const =0;
	static int getIndex() {return index;};
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
	Variables * vars;
	lexer * mylex;
	token currentToken;
	map<string,Expression *> * FuncTable;
	// float result;
	float operand;

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

	Expression *ParseExpression();
	Expression *ParsePrimary();
	Expression *ParseNumberExpr();
	Expression *ParseBinRHS(Expression *);
	Expression *ParseParenthesesExpr();
	Expression *ParseIdentifExpr();
	Expression *ParseFunctionCallExpr(string);
	Expression * ParseDefFunctionExpr();
	Expression * ParseParamExpr();
	Expression * ParsePackage();
	Expression * ParseDef();
	Expression * ParseBlockEx();
	Expression * ParseVarDec();

	void addFunction(string & name,Expression * fun);


public:
	Expression * getFunction(string name);
	parser(char* path);
	~parser();
	token getNext(bool);
	bool parseFile(int &);
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


class NumberEx : public Expression {
public:
	float value;

	NumberEx(float val) : value(val) {};
	myTypes getType() const override { return T_FLOAT; }
	float getValue() const override {return value;}
	void accept(IVisitor* v) const override { v->visit(this); };
};

class VariableEx : public Expression {
public:
	std::string name;
	Variables * vars;
	myTypes type;

	VariableEx(std::string varname, Variables * var,myTypes type) : name(varname), vars(var),type(type) {}
	myTypes getType() const override { return type; }
	float getValue() const override {
		return vars->getValue(name)->getValue(); // Resolve to a number
	}
	void accept(IVisitor* v) const override { v->visit(this); };
};

class ReturnEx : public Expression {
public:
	Expression *val;

	ReturnEx(Expression * val) : val(val) {}
	myTypes getType() const override { return val->getType(); }
	float getValue() const override {
		return val->getValue(); // Resolve
	}
	void accept(IVisitor* v) const override { v->visit(this); };
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
	myTypes getType() const override { return T_VOID; }
	float getValue() const override {return 0;}
	void accept(IVisitor* v) const override { v->visit(this); };
};


class FunctionDefEx : public Expression {
public:
	myTypes type;
	string name;
	float value;
	Expression *param;
	Expression * body;

	FunctionDefEx(string p_name,Expression *p_param,Expression * body, myTypes type) : name(p_name),value(0),param(p_param),body(body),type(type) {};
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
