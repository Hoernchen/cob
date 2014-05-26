#pragma once

#include <map>
//#include "llvm/Config/llvm-config.h"
//#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/Support/raw_ostream.h"

//#include "llvm/IR/Constants.h"
//#include "llvm/IR/Type.h"
//#include "llvm/IR/Instructions.h"
//#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"

#include "IVisitor.h"
using namespace llvm;

struct varTVal {
    Value * val;
    myTypes type;
    varTVal(Value *pval, myTypes ptype) : val(pval), type(ptype) {}
    varTVal() {}
};

class CodegenVisitor: public IVisitor{
    Function * currentFunc;
	Value* v;
    Module *mod;
    IRBuilder<> *builder;
    map<string,varTVal> VarTable;
public:
    CodegenVisitor(Module * mod);
	virtual void visit( const PackageEx* v) override;
	virtual void visit( const NumberEx* v) override;
	virtual void visit( const VariableEx* v) override;
	virtual void visit( const ReturnEx* v) override;
	virtual void visit( const BinaryExprEx* v) override;
	virtual void visit( const ParamEx* v) override;
	virtual void visit( const BlockEx* v) override;
	virtual void visit( const FunctionDefEx* v) override;
	virtual void visit( const FunctionCallEx* v) override;
	Value* operator()() {return v;}
	Function* curFunc() {return currentFunc;}
	void curFunc(Function* newfunc) { currentFunc = newfunc;}
	void dumpfn(){
		
		cerr << endl;

		int asz = curFunc()->arg_size();
		cerr << "fn: " << curFunc()->getName().str() << " argc: " << asz << endl;
		if(asz){
			for(auto &i: curFunc()->getArgumentList()){
				cerr << "\targ:" << i.getName().str() << " type: " << i.getType() << endl;
			}
		}

		cerr << "vars:" << endl;
		for(auto &v : VarTable)
			cerr << "\t" << v.first << " " << v.second.type << endl;

		cerr << endl;
	}
};
