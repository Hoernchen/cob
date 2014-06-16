//#include "llvm/Config/llvm-config.h"
//#include "llvm/Bitcode/ReaderWriter.h"
//#include "llvm/Support/raw_ostream.h"

//#include "llvm/IR/Constants.h"
//#include "llvm/IR/DerivedTypes.h"
//#include "llvm/IR/Instructions.h"
//#include "llvm/IR/LLVMContext.h"
//#include "llvm/IR/Module.h"

#include "parser.h"
#include "CodegenVisitor.h"


using namespace llvm;

CodegenVisitor::CodegenVisitor(Module * mod) : mod(mod), builder(new IRBuilder<>(getGlobalContext())) {}

void CodegenVisitor::visit( const PackageEx* v) {
    for(auto i : *v->body) {
        i->accept(this);
    }
}

void CodegenVisitor::visit( const ConditionalEx* v) {
    Function *CurrentFunc = builder->GetInsertBlock()->getParent();
    BasicBlock *ThenBlock = BasicBlock::Create(getGlobalContext(), "then", CurrentFunc);
    BasicBlock *ElseBlock = BasicBlock::Create(getGlobalContext(), "else");
    BasicBlock *MergeBlock = BasicBlock::Create(getGlobalContext(), "ifcont");

    v->LHS->accept(this);
    Value *LHS=this->v;
    v->RHS->accept(this);
    Value *RHS=this->v;
    Value *tempcond;

    if(v->LHS->getType() == T_INT) {
        if(v->op == EQ) tempcond=builder->CreateICmpEQ(LHS,RHS,"cmptmp");
        else tempcond=builder->CreateICmpUGT(LHS, RHS, "cmptmp");
    }
    else {
        if(v->op == EQ) tempcond=builder->CreateFCmpUEQ(LHS,RHS,"cmptmp");
        else tempcond=builder->CreateFCmpUGT(LHS, RHS, "cmptmp");
    }

    builder->CreateCondBr(tempcond,ThenBlock,ElseBlock);

    builder->SetInsertPoint(ThenBlock);

    v->body->accept(this);
    builder->CreateBr(MergeBlock); // Return to rest of function body

    // Branch out of unused else label
    builder->SetInsertPoint(ElseBlock);
    if(v->elsedo) {
        v->elsedo->accept(this);
    }

    builder->CreateBr(MergeBlock);

    builder->SetInsertPoint(MergeBlock);

    CurrentFunc->getBasicBlockList().push_back(ElseBlock);
    CurrentFunc->getBasicBlockList().push_back(MergeBlock);
    builder->SetInsertPoint(MergeBlock);
}


void CodegenVisitor::visit( const NumberEx* v) {
    switch(v->getType()){
		case T_FLOAT:
			this->v = ConstantFP::get(getGlobalContext(), APFloat(v->getValue()));
			break;
		case T_INT:
			this->v = ConstantInt::get(getGlobalContext(), APInt(32, v->getIntValue()));
			break;
		default:
			cerr << __FILE__ <<":"<<__LINE__ << ": huh type?" << endl;
			exit(0);
}
};

void CodegenVisitor::visit(const VariableEx* v) {

	// a variable?
    if(VarTable.find(v->name) != VarTable.end()) {
        this->v = VarTable[v->name].val;
    }

	//.. or a parameter?
	else if(curFunc()->arg_begin()->getName() == v->name) {
        this->v = curFunc()->arg_begin();
    }

    else if(v->getEx()) {
        v->getEx()->accept(this);
        VarTable[v->name]=varTVal(this->v,v->getType());
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
	else{
        if(v->getType() == T_FLOAT) {
            switch(v->OP) {
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
        }
        else if(v->getType() == T_INT) {
            switch(v->OP) {
            case '+':
                this->v=builder->CreateAdd(lval,rval,"addtmp");
                break;
            case '-':
                this->v=builder->CreateSub(lval,rval,"subtmp");
                break;
            case '*':
                this->v=builder->CreateMul(lval,rval,"multmp");
                break;
            case '/':
                this->v=builder->CreateSDiv(lval,rval,"divtmp");
                break;
            default:
                this->v=0;
            }
        }

	}
};

void CodegenVisitor::visit( const ParamEx* v) { };

void CodegenVisitor::visit( const BlockEx* v) {
    for(auto i : *v->body) {
        i->accept(this);
    }
}

void CodegenVisitor::visit( const FunctionDefEx* v) {
    FunctionType *ft;
    Type * t_ret;
    vector<Type *> params;

	VarTable.clear(); // New scope

	switch(v->param->getType()){
	case T_FLOAT:
		params = vector<Type *>((v->param ? 1 : 0),Type::getFloatTy(getGlobalContext()));
		break;
	case T_INT:
		params = vector<Type *>((v->param ? 1 : 0),Type::getInt32Ty(getGlobalContext()));
		break;
	case T_VOID:
		params = vector<Type *>((v->param ? 1 : 0),Type::getVoidTy(getGlobalContext()));
		break;
	default:
		cerr << __FILE__ <<":"<<__LINE__ << ": huh type?" << endl;
		exit(0);
	}

	switch(v->getType()){
	case T_FLOAT:
		t_ret=Type::getFloatTy(getGlobalContext());
		break;
	case T_INT:
		t_ret=Type::getInt32Ty(getGlobalContext());
		break;
	case T_VOID:
		t_ret=Type::getVoidTy(getGlobalContext());
		break;
	default:
		cerr << __FILE__ <<":"<<__LINE__ << ": huh type?" << endl;
		exit(0);
	}

    ft = FunctionType::get(t_ret,params,false);
    Function *F = Function::Create(ft, Function::ExternalLinkage, v->name, mod);

	//update current context
	curFunc(F);

    if(v->param) {
        string name=((ParamEx *) v->param)->name;
        F->arg_begin()->setName(name);
    }

    BasicBlock * bb=BasicBlock::Create(getGlobalContext(),"entry",F);
    builder->SetInsertPoint(bb);
    v->body->accept(this); // Generate code from Body
    this->v=F;

	dumpfn();
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
