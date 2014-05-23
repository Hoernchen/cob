#pragma once

class PackageEx;
class PackageEx;
class NumberEx;
class VariableEx;
class ReturnEx;
class BinaryExprEx;
class ParamEx;
class BlockEx;
class FunctionDefEx;
class FunctionCallEx;

class IVisitor{

public:
	virtual void visit( const PackageEx* v) =0;
	virtual void visit( const NumberEx* v) =0;
	virtual void visit( const VariableEx* v) =0;
	virtual void visit( const ReturnEx* v) =0;
	virtual void visit( const BinaryExprEx* v) =0;
	virtual void visit( const ParamEx* v) =0;
	virtual void visit( const BlockEx* v) =0;
	virtual void visit( const FunctionDefEx* v) =0;
	virtual void visit( const FunctionCallEx* v) =0;
};