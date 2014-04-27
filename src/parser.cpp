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

//bool parser::isVar(token & p) {
//	if(p.ty()==VAR) return true;
//	else return false;
//}
//
//bool parser::isNumber(token & p) {
//	if(p.ty()==NUMBER) return true;
//	else return false;
//}
//
//bool parser::isOperator(token & p) {
//	if(p.ty()==OPERATOR) return true;
//	else return false;
//}
//
//bool parser::isAssignment(token & p) {
//	if(p.ty()==ASSIGNMENT) return true;
//	else return false;
//}
//
//bool parser::isEOL(token & p) {
//	if(p.ty()==EOL) return true;
//	else return false;
//}
//
//bool parser::isEnd(token & p) {
//	if(p.ty()==END) return true;
//	else return false;
//}

token parser::getNext(bool forceNextLine=false) {
	token t = mylex->getNext(forceNextLine);
	 if(!t())
		 cout<< "fail at line:" << t.ln() <<endl;
	return t;
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
	if(mylex->readLast().ty()==EOL) return resolveVar(var.str());
	if(mylex->readLast().ty()==ASSIGNMENT) {
		if(getNext().ty()==NUMBER) {
			float val=stof(mylex->readLast().str());
			vars->insert(pair<string,float>(var.str(),val));
			if(getNext().ty() != EOL) throw runtime_error("Assignment must be followed by end of line");
			return val;
		}
		else throw runtime_error(":= must be followed by a number");
	}
	else if(mylex->readLast().ty()==OPERATOR) {
		return doMath(resolveVar(var.str()));
	}
	return resolveVar(var.str());
}

// Expects ( to be current token
float parser::parentheses() {
	// cout<<"Parentheses"<<endl;
	float res;
	token left=getNext(); // Left hand side of next expression
	if(left.ty() != NUMBER && left.ty() != OPEN && left.ty() != VAR) throw runtime_error("Opening bracket must be followed by number, expression resolving to number or a variable");
	if(left.ty()==OPEN) { // New expression in parentheses
		float temp=parentheses();
		getNext(); // Get Operator for doMath
		res=doMath(temp);
	}
	else {
		getNext(); // Get operator
		if(left.ty()==NUMBER) res=doMath(stof(left.str()));
		else if(left.ty()==VAR) res=doMath(resolveVar(left.str()));
	}
	
	if(mylex->readLast().ty() != CLOSE) throw runtime_error("Brackets must be closed");
	// if(mylex->readLast().ty()==OPERATOR) return doMath(res);
	return res;
}

// Expects operator to be current token
float parser::doMath(float left) {
	// cout<<"Do Math"<<endl;
	float right;
	float result;
	if(mylex->readLast().ty()==EOL || mylex->readLast().ty() == CLOSE) return left; // Single value to be returned, like var or 42
	token op=mylex->readLast(); // Operator is the first thing we get from lexer
	if(op.ty()==OPERATOR) {
		token rhs=getNext();
		if(rhs.ty()==NUMBER) {
			right=stof(rhs.str());
		}
		else if(rhs.ty()==OPEN) {
			right=parentheses();
		}
		else if(rhs.ty()==VAR) {
			right=resolveVar(rhs.str());
		}
		else throw runtime_error("Operator must be followed by number or expression in parentheses");
		switch(op.str()[0]) {
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
		if(mylex->readLast().ty()==OPERATOR) return doMath(result); // If it is an operator, recurse
		else return result; // Else we are done
	}
	else throw runtime_error("Not a valid expression");
}

void parser::processLine() {
	float temp;
	token tok=getNext();
	try {
		switch(tok.ty()) {
			case VAR:
				cout<<handleVar()<<endl;
				break;
			case NUMBER:
				getNext(); // Fetch operator
				cout<<doMath(stof(tok.str()))<<endl;
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



int main(int argc, char* argv[]) {
	parser * myParse=new parser(argv[1]);
	while(true) 
	myParse->processLine();
	return 0;
}
