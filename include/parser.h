#include <map>
#include "lex.h"

class Expression {
	public:
	virtual ~Expression() {};
	virtual float getValue()=0;
};

class NumberEx : public Expression {
	float value;
	public:
	NumberEx(float val) : value(val) {};
	float getValue() {return value;}
};

class VariableEx : public Expression {
	std::string varname;
	public:
	VariableEx(std::string var) : varname(var) {}
	float getValue() {return 0;}  // Variable resolving goes here
};

class BinaryExprEx : public Expression {
	char OP;
	Expression *LHS;
	Expression *RHS;
	public:
	BinaryExprEx(char op, Expression *lhs, Expression * rhs) : OP(op), LHS(lhs), RHS(rhs) {}
	float getValue() {
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
};

class parser {
	std::map<string,float> * vars;
	lexer * mylex;
	token currentToken;
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
	
	public:
	parser(char* path);
	~parser();
	token getNext(bool);
	void processLine();
};
