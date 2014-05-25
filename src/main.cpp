#include <iostream>

#include "parser.h"
#include "ASTGraphVisitor.h"
#include "CodegenVisitor.h"
#include "TCheckVisitor.h"

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
        Module *mod=new Module("GoTest",getGlobalContext());
        CodegenVisitor cvis(mod);
        TCheckVisitor tvis;
        top->accept(&vis);
        top->accept(&tvis);
        top->accept(&cvis);
        mod->dump();
        return true;
    }
	return 0;
}
