#include <iostream>

#include "parser.h"
#include "ASTGraphVisitor.h"

int Expression::index =0;

int main(int argc, char* argv[]) {
    if(argc<2) {
        cerr<<"No input file given"<<endl;
        return -1;
    }

	parser * myParse=new parser(argv[1]);
    Expression* top = myParse->parseFile();
	if(top) {
		ASTGraphVisitor vis;
        top->accept(&vis);
        return true;
    }
	return 0;
}