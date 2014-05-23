#include "llvm/Config/llvm-config.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/Support/raw_ostream.h"

#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

#include "parser.h"
#include "CodegenVisitor.h"


using namespace llvm;

void CodegenVisitor::visit( const PackageEx* v) { };
void CodegenVisitor::visit( const NumberEx* v) { this->v = ConstantFP::get(getGlobalContext(), APFloat(v->getValue())); };
void CodegenVisitor::visit( const VariableEx* v) { };
void CodegenVisitor::visit( const ReturnEx* v) { };
void CodegenVisitor::visit( const BinaryExprEx* v) { };
void CodegenVisitor::visit( const ParamEx* v) { };
void CodegenVisitor::visit( const BlockEx* v) { };
void CodegenVisitor::visit( const FunctionDefEx* v) { };
void CodegenVisitor::visit( const FunctionCallEx* v) { };