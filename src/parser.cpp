#include <iostream>
#include <map>
#include <string>
#include <stdexcept>

#include "parser.h"

parser::parser() : vars(new map<string,float>()), mylex(new lexer()) {
	char in[] = "testinput.txt";
	mylex->openFile(in);
}

parser::~parser() {
	if(vars) delete vars;
	delete mylex;
}

//bool parser::isVar(token & p) {
//	if(p.type==VAR) return true;
//	else return false;
//}
//
//bool parser::isNumber(token & p) {
//	if(p.type==NUMBER) return true;
//	else return false;
//}
//
//bool parser::isOperator(token & p) {
//	if(p.type==OPERATOR) return true;
//	else return false;
//}
//
//bool parser::isAssignment(token & p) {
//	if(p.type==ASSIGNMENT) return true;
//	else return false;
//}
//
//bool parser::isEOL(token & p) {
//	if(p.type==EOL) return true;
//	else return false;
//}
//
//bool parser::isEnd(token & p) {
//	if(p.type==END) return true;
//	else return false;
//}

token parser::getNext(bool forceNextLine=false) {
	try {
		return mylex->getNext(forceNextLine);
	}
	catch(runtime_error ex) {
		cout<<ex.what()<<endl;
	}
}

float parser::resolveVar(string name) {
	map<string,float>::iterator it = vars->find(name);
	if(it != vars->end()) {
		return it->second;
	}
	else throw runtime_error("Variable does not exist");
}

float parser::handleVar() {
	token var=mylex->readLast();
	getNext();
	if(mylex->readLast().type==EOL) return resolveVar(var.text);
	if(mylex->readLast().type==ASSIGNMENT) {
		if(getNext().type==NUMBER) {
			float val=stof(mylex->readLast().text);
			vars->insert(pair<string,float>(var.text,val));
			if(getNext().type != EOL) throw runtime_error("Assignment must be followed by end of line");
			return val;
		}
		else throw runtime_error(":= must be followed by a number");
	}
	else if(mylex->readLast().type==OPERATOR) {
		return doMath(resolveVar(var.text));
	}
	return resolveVar(var.text);
}

// Expects ( to be current token
float parser::parentheses() {
	// cout<<"Parentheses"<<endl;
	float res;
	token left=getNext(); // Left hand side of next expression
	if(left.type != NUMBER && left.type != OPEN && left.type != VAR) throw runtime_error("Opening bracket must be followed by number, expression resolving to number or a variable");
	if(left.type==OPEN) { // New expression in parentheses
		float temp=parentheses();
		getNext(); // Get Operator for doMath
		res=doMath(temp);
	}
	else {
		getNext(); // Get operator
		if(left.type==NUMBER) res=doMath(stof(left.text));
		else if(left.type==VAR) res=doMath(resolveVar(left.text));
	}
	
	if(mylex->readLast().type != CLOSE) throw runtime_error("Brackets must be closed");
	// if(mylex->readLast().type==OPERATOR) return doMath(res);
	return res;
}

// Expects operator to be current token
float parser::doMath(float left) {
	// cout<<"Do Math"<<endl;
	float right;
	float result;
	if(mylex->readLast().type==EOL || mylex->readLast().type == CLOSE) return left; // Single value to be returned, like var or 42
	token op=mylex->readLast(); // Operator is the first thing we get from lexer
	if(op.type==OPERATOR) {
		token rhs=getNext();
		if(rhs.type==NUMBER) {
			right=stof(rhs.text);
		}
		else if(rhs.type==OPEN) {
			right=parentheses();
		}
		else if(rhs.type==VAR) {
			right=resolveVar(rhs.text);
		}
		else throw runtime_error("Operator must be followed by number or expression in parentheses");
		switch(op.text[0]) {
			case '+':
				result=left+right;
				break;
			case '-':
				result=left-right;
				break;
			case '*':
				result=left*right;
				break;
			case '/':
				result=left/right;
				break;
		}
		getNext(); // Get token which comes after rhs
		if(mylex->readLast().type==OPERATOR) return doMath(result); // If it is an operator, recurse
		else return result; // Else we are done
	}
	else throw runtime_error("Not a valid expression");
}

void parser::processLine() {
	float temp;
	token tok=getNext();
	try {
		switch(tok.type) {
			case VAR:
				cout<<handleVar()<<endl;
				break;
			case NUMBER:
				getNext(); // Fetch operator
				cout<<doMath(stof(tok.text))<<endl;
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
				temp=parentheses();
				getNext();
				cout<<doMath(temp)<<endl;

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
	catch(runtime_error ex) {
		cout<<ex.what()<<endl;
		cout<<"LOADING NEW LINE"<<endl;
		mylex->getNext(true);
	}
}



int main() {
	parser * myParse=new parser();
	while(true) 
	myParse->processLine();
	return 0;
}
