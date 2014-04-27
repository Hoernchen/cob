#include <map>
#include "lex.h"

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
	
	public:
	parser(char* path);
	~parser();
	token getNext(bool);
	void processLine();
};