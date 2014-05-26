#include "parser.h"
#include "ASTGraphVisitor.h"

void ASTGraphVisitor::visit( const PackageEx* v) {

	cout<<"digraph parse_out"<<v->index<<"{"<<endl;
	int id=++v->index; // Always first node
	cout<<id<<"[label=\"package\" shape=\"hexagon\"]"<<endl;
	cout<<id+1<<"[label=\""<<v->name<<"\"]"<<endl;
	cout<<v->index++<<"->"<<id+1<<endl;
	for(auto i : *(v->body)) {
		cout << id+1 <<"->";
		i->accept(this);
	}
	cout<<"}"<<endl;

};
void ASTGraphVisitor::visit( const NumberEx* v){

	int id=++v->index;
	cout<<id << endl;
	cout<<id<<"[label=\""<<v->value<<"\"]"<<endl;

};
void ASTGraphVisitor::visit( const VariableEx* v){

	int id=++v->index;
	cout<<id << endl;
	cout<<id<<"[label=\"&lt;"<< (v->def ? "d" : "u")<<"&gt; variable | " << v->name << " | " << TypeNames[v->type] << "\" shape=\"Mrecord\" color=\"blue\"]"<<endl;

    if(curVars()->getValue(v->name)){
		cout<<id<<"->";
        curVars()->getValue(v->name)->accept(this);
	}

};
void ASTGraphVisitor::visit( const ReturnEx* v){

	int id=++v->index;
	cout<<id << endl;
	cout<<id<<"[label=\"return\"]"<<endl;

	if(v->val){
		cout<<id<<"->";
		v->val->accept(this);
	}

};
void ASTGraphVisitor::visit( const BinaryExprEx* v){

	int id=++v->index;
	cout<<id << endl;
	cout<<id<<"[label=\"binary expr\"]"<<endl;

	cout<<id+1<<"[label=\""<<v->OP<<"\"]"<<endl;
	cout<<id++<<"->"<<id<<endl;
	v->index++;
	cout<<id <<"->"; v->LHS->accept(this);
	cout<<id <<"->"; v->RHS->accept(this);

};
void ASTGraphVisitor::visit( const ParamEx* v){

	int id=++v->index;
	cout<<id << endl;
	cout<<id<<"[label=\"{param | { " << v->type << " | " << v->name << "}}\" shape=\"Mrecord\" color=\"green\"]"<<endl;
};
void ASTGraphVisitor::visit( const BlockEx* v){

	int id=++v->index;
	cout<<id << endl;
	cout<<id<<"[label=\"block\" shape=\"box\"]"<<endl;

	for(auto i : *v->body) {
		cout<<id <<"->";
		i->accept(this);
	}

};
void ASTGraphVisitor::visit( const FunctionDefEx* v){
	//update variable scope
    curVars(v->vars);

	int id=++v->index;
	cout<<id << endl;
	cout<<id <<"[label=\"def "<<v->name<<"\" color=\"red\"]"<<endl;

	// FIXME attach to subtree for closures
	if(v->param){
		cout<<id<< "->";
		v->param->accept(this);
	}
	if(v->body){
		cout<<id<< "->";
		v->body->accept(this);
	}

};
void ASTGraphVisitor::visit( const FunctionCallEx* v){

	int id=++v->index;
	cout<<id << endl;
	cout<<id<<"[label=\"call to "<<v->name<<"\"]"<<endl;

	cout<<id+1<<"[label=\"param\"]"<<endl;
	cout<<id<<"->"<<++v->index<<endl;
	if(v->param){
		cout<<id+1 <<"->";
		v->param->accept(this);
	}

};
