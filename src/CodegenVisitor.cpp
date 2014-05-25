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

CodegenVisitor::CodegenVisitor(Module * mod) : mod(mod), builder(new IRBuilder<>(getGlobalContext())) {}

void CodegenVisitor::visit( const PackageEx* v) {
    for(auto i : *v->body) {
        i->accept(this);
    }
}

void CodegenVisitor::visit( const NumberEx* v) {
    this->v = ConstantFP::get(getGlobalContext(), APFloat(v->getValue()));
};

void CodegenVisitor::visit(const VariableEx* v) {
    if(VarTable[v->name]) {
        this->v = VarTable[v->name];
    }

    else if(v->getEx()) {
        v->getEx()->accept(this);
        VarTable[v->name]=this->v;
    }

    else {
        cerr<<"Variable "<<v->name<<" undefined"<<endl;
        this->v=0;
    }
}

void CodegenVisitor::visit( const ReturnEx* v) {
    v->val->accept(this);
    this->v = builder->CreateRet(this->v);
}

void CodegenVisitor::visit( const BinaryExprEx* v) {
    v->LHS->accept(this);
    Value *rval = this->v;
    v->RHS->accept(this);
    Value *lval = this->v;
    if(!rval || !lval) {
        this->v=0;
    }
    else switch(v->OP) {
    case '+':
        this->v=builder->CreateFAdd(lval,rval,"addtmp");
        break;
    case '-':
        this->v=builder->CreateFSub(lval,rval,"subtmp");
        break;
    case '*':
        this->v=builder->CreateFMul(lval,rval,"multmp");
        break;
    case '/':
        this->v=builder->CreateFDiv(lval,rval,"divtmp");
        break;
    default:
        this->v=0;
    }
};

void CodegenVisitor::visit( const ParamEx* v) { };

void CodegenVisitor::visit( const BlockEx* v) {
    for(auto i : *v->body) {
        i->accept(this);
    }
}

void CodegenVisitor::visit( const FunctionDefEx* v) {
    VarTable.clear(); // New scope
    FunctionType *ft;
    Type * t_ret;
    vector<Type *> params;

    if(v->param->getType() == T_FLOAT) {
        t_ret=Type::getFloatTy(getGlobalContext());
        params = vector<Type *>((v->param ? 1 : 0),Type::getFloatTy(getGlobalContext()));
    }
    else if(v->param->getType() == T_INT) {
        t_ret=Type::getInt32Ty(getGlobalContext());
        params = vector<Type *>((v->param ? 1 : 0),Type::getInt32Ty(getGlobalContext()));
    }
    ft = FunctionType::get(t_ret,params,false);
    Function *F = Function::Create(ft, Function::ExternalLinkage, v->name, mod);
    if(v->param) {
        string name=((ParamEx *) v->param)->name;
        F->arg_begin()->setName(name);
        VarTable[name]=F->arg_begin();
    }

    BasicBlock * bb=BasicBlock::Create(getGlobalContext(),"entry",F);
    builder->SetInsertPoint(bb);
    v->body->accept(this); // Generate code from Body
    this->v=F;
}


void CodegenVisitor::visit( const FunctionCallEx* v) {
    Function *called=mod->getFunction(v->name);
    if(!called) {
        cerr<<"Function "<<v->name<<" does not exist";
        this->v=0;
        return;
    }
    if(v->param && called->arg_size() != 1) {
        cerr<<"Function "<<v->name<<" requires one argument";
        this->v=0;
        return;
    }
    vector<Value*> args;
    v->param->accept(this);
    args.push_back(this->v);
    this->v=builder->CreateCall(called, args, "calltmp");
};
