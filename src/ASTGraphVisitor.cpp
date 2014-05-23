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
	//cout<<parent<<"->"<<id<<endl;

};
void ASTGraphVisitor::visit( const VariableEx* v){

	int id=++v->index;
	cout<<id << endl;
	cout<<id<<"[label=\"&lt;"/*<< (def ? "d" : "u")*/ <<"&gt; variable | " << v->name << "\" shape=\"Mrecord\" color=\"blue\"]"<<endl;
	//cout<<parent<<"->"<<id<<endl;
	//cout<<++index<<"[label=\""<<varname<<"\"]"<<endl;
	//cout<<id<<"->"<<index<<endl;
	if(v->vars->getValue(v->name)){
		cout<<id<<"->";
		v->vars->getValue(v->name)->accept(this);
	}

};
void ASTGraphVisitor::visit( const ReturnEx* v){

	int id=++v->index;
	cout<<id << endl;
	cout<<id<<"[label=\"return\"]"<<endl;
	//cout<<parent<<"->"<<id<<endl;
	if(v->val){
		cout<<id<<"->";
		v->val->accept(this);
	}

};
void ASTGraphVisitor::visit( const BinaryExprEx* v){

	int id=++v->index;
	cout<<id << endl;
	cout<<id<<"[label=\"binary expr\"]"<<endl;
	//cout<<parent<<"->"<<id<<endl;
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
	//cout<<parent<<"->"<<id<<endl;

	//cout<<id+1<<"[label=\""<<type<<"\"]"<<endl;
	//cout<<id<<"->"<<++index<<endl;
	//cout<<id+2<<"[label=\""<<name<<"\"]"<<endl;
	//cout<<id<<"->"<<++index<<endl;
	//index+=2;

};
void ASTGraphVisitor::visit( const BlockEx* v){

	int id=++v->index;
	cout<<id << endl;
	cout<<id<<"[label=\"block\" shape=\"box\"]"<<endl;
	//cout<<parent<<"->"<<id<<endl;
	for(auto i : *v->body) {
		cout<<id <<"->";
		i->accept(this);
	}

};
void ASTGraphVisitor::visit( const FunctionDefEx* v){

	int id=++v->index;
	cout<<id << endl;
	cout<<id <<"[label=\"def "<<v->name<<"\" color=\"red\"]"<<endl;
	//cout<<parent<<"->"<<id<<endl;
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
	//cout<<parent<<"->"<<id<<endl;
	cout<<id+1<<"[label=\"param\"]"<<endl;
	cout<<id<<"->"<<++v->index<<endl;
	if(v->param){
		cout<<id+1 <<"->";
		v->param->accept(this);
	}

};
