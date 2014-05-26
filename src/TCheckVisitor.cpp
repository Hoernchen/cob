#include "parser.h"
#include "TCheckVisitor.h"

void TCheckVisitor::visit( const PackageEx* v) {
    cerr<<"Visiting Package Expression"<<endl;
    for(auto i : *(v->body)) {
        i->accept(this);
    }
};

void TCheckVisitor::visit( const NumberEx* v) { };

void TCheckVisitor::visit( const VariableEx* v) {
    cerr<<"Visiting Variable Expression"<<endl;
    if(v->getType() != v->type) {
        cerr<<"Operand type does not match type of variable "<<v->name<<" in assignment"<<endl;
        exit(0);
    }
}

void TCheckVisitor::visit( const ReturnEx* v) {
    cerr<<"Visiting Return Expression"<<endl;
    v->val->accept(this);
};

void TCheckVisitor::visit( const BinaryExprEx* v) {
    cerr<<"Visiting Binary Expression"<<endl;
    if(v->LHS && v->RHS && v->LHS->getType() == v->RHS->getType())
        return;
    else {
        cerr<<"Type mismatch in binary expression"<<endl;
        exit(0);
    }
}

void TCheckVisitor::visit( const ParamEx* v) {};

void TCheckVisitor::visit( const BlockEx* v) {
    cerr<<"Visiting Block Expression"<<endl;
    for(auto i : *(v->body)) {
        i->accept(this);
    }
}

void TCheckVisitor::visit( const FunctionDefEx* v) {
    cerr<<"Visiting Function Def Expression"<<endl;
    vector<ReturnEx *> ret=((BlockEx *) v->body)->getReturn();
    if(!ret.empty()) {
        if(ret[0]->getType() != v->getType()) {
            cerr<<"Return type does not match function's return type in "<<v->name<<endl;
            exit(0);
        }
    }
    else if(v->getType() != T_VOID) {
        cerr<<"No return value for non-void function "<<v->name<<endl;
        exit(0);
    }
    v->body->accept(this);
}

void TCheckVisitor::visit( const FunctionCallEx* v) {
    cerr<<"Visiting Function Call Expression"<<endl;
    FunctionDefEx * func=(FunctionDefEx *) (v->p->getFunction(v->name));
    if(func) {
        if(func->param) {
            if(v->param) {
                if(v->param->getType() != func->param->getType()) {
                        cerr<<"Parameter's type does not match passed value's type in "<<v->name<<endl;
                        exit(0);
                }
            }
            else {
                cerr<<"Function "<<v->name<<" demands one argument, none given"<<endl;
                exit(0);
            }
        }
        else if(v->param) {
            cerr<<"Function "<<v->name<<" does not allow argument"<<endl;
            exit(0);
        }
    }
    else {
        cerr<<"Function "<<v->name<<" does not exist"<<endl;
        exit(0);
    }
}
