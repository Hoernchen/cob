#pragma once

#include <map>
#include "llvm/Config/llvm-config.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/Support/raw_ostream.h"

#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"

#include "IVisitor.h"
using namespace llvm;

class CodegenVisitor: public IVisitor{
	Value* v;
    Module *mod;
    IRBuilder<> *builder;
    map<string,Value *> VarTable;
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
	Value* operator()() {return v;};
};
