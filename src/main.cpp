#include <iostream>

#include "parser.h"

int Expression::index =0;


int main(int argc, char* argv[]) {
    if(argc<2) {
        cerr<<"No input file given"<<endl;
        return -1;
    }
    int id=0;
	parser * myParse=new parser(argv[1]);
    myParse->parseFile(id);
	return 0;
}