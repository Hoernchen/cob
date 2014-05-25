#include "parser.h"
#include "TCheckVisitor.h"

void TCheckVisitor::visit( const PackageEx* v) { };
void TCheckVisitor::visit( const NumberEx* v) { };
void TCheckVisitor::visit( const VariableEx* v) { };
void TCheckVisitor::visit( const ReturnEx* v) { };
void TCheckVisitor::visit( const BinaryExprEx* v) { };
void TCheckVisitor::visit( const ParamEx* v) { };
void TCheckVisitor::visit( const BlockEx* v) { };
void TCheckVisitor::visit( const FunctionDefEx* v) { };
void TCheckVisitor::visit( const FunctionCallEx* v) { };