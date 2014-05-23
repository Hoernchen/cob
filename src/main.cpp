#include <iostream>

#include "llvm/Config/llvm-config.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/Support/raw_ostream.h"

#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"


#include "parser.h"




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