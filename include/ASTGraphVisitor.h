#pragma once

#include "IVisitor.h"

class ASTGraphVisitor: public IVisitor{
    Variables * localVars;
public:
	virtual void visit( const PackageEx* v) override;
	virtual void visit( const NumberEx* v) override;
	virtual void visit( const VariableEx* v) override;
	virtual void visit( const ReturnEx* v) override;
	virtual void visit( const BinaryExprEx* v) override;
	virtual void visit( const ParamEx* v) override;
	virtual void visit( const BlockEx* v) override;
	virtual void visit( const FunctionDefEx* v) override;
	virtual void visit( const FunctionCallEx* v) override;
    virtual void visit( const ConditionalEx* v) override;
	Variables* curVars() {return localVars;}
	void curVars(Variables* newfunc) { localVars = newfunc;}
};
