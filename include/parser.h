#include <map>
#include <vector>
#include "lex.h"


class parser;

enum myTypes {T_INT,T_FLOAT,T_VOID};


class Expression {
    public:
    virtual ~Expression() {}
    virtual float getValue()=0;
    virtual myTypes getType()=0;
    virtual void graph(int parent, int & index,bool def=false)=0;
};

class Variables {
    std::map<string,Expression *> * vars;
    public:
        Variables() : vars(new map<string,Expression *>()) {};
        ~Variables()  { delete vars;};
        void insertVar(string & name, Expression * val) {
            vars->insert(pair<string,Expression *>(name,val));
        }
        Expression * getValue(string & name) {
            map<string,Expression *>::iterator it = vars->find(name);
            if(it != vars->end())
                return it->second;
            else return 0;
        }
};

class parser {
    Variables * vars;
    lexer * mylex;
    token currentToken;
    map<string,Expression *> * FuncTable;
    // float result;
    float operand;

    //bool isVar(token &);
    //bool isNumber(token &);
    //bool isOperator(token &);
    //bool isAssignment(token &);
    //bool isEOL(token &);
    //bool isEnd(token &);
    token nextToken();
    float doMath(float);
    float resolveVar(string);
    float parentheses();
    float handleVar();

    Expression *ParseExpression();
    Expression *ParsePrimary();
    Expression *ParseNumberExpr();
    Expression *ParseBinRHS(Expression *);
    Expression *ParseParenthesesExpr();
    Expression *ParseIdentifExpr();
    Expression *ParseFunctionCallExpr(string);
    Expression * ParseDefFunctionExpr();
    Expression * ParseParamExpr();
    Expression * ParsePackage();
    Expression * ParseDef();
    Expression * ParseBlockEx();
    Expression * ParseVarDec();

    void addFunction(string & name,Expression * fun);


    public:
    Expression * getFunction(string name);
    parser(char* path);
    ~parser();
    token getNext(bool);
    bool parseFile(int &);
};


class PackageEx : public Expression {
    std::string name;
    vector<Expression *> * body;
    public:
    PackageEx(std::string name) : name(name), body(new vector<Expression *>()) {}
	~PackageEx()  { delete body; }
    void addLine(Expression * p) { if(p != 0) body->push_back(p); }
    myTypes getType() { return T_VOID; }
    float getValue() {
        return 0;
    }
    void graph(int parent, int & index,bool def=false) {
        cout<<"digraph parse_out"<<index<<"{"<<endl;
        int id=++index; // Always first node
        cout<<id<<"[label=\"package\" shape=\"hexagon\"]"<<endl;
        cout<<id+1<<"[label=\""<<name<<"\"]"<<endl;
        cout<<index++<<"->"<<id+1<<endl;
        for(auto i : *body) {
            i->graph(id+1,index);
        }
        cout<<"}"<<endl;
    }
};




class NumberEx : public Expression {
	float value;
	public:
	NumberEx(float val) : value(val) {};
    myTypes getType() { return T_FLOAT; }
	float getValue() {return value;}
    void graph(int parent, int & index,bool def=false) {
        int id=++index;
        cout<<id<<"[label=\""<<value<<"\"]"<<endl;
         cout<<parent<<"->"<<id<<endl;
    }
};

class VariableEx : public Expression {
	std::string varname;
	Variables * vars;
    myTypes type;
	public:
    VariableEx(std::string varname, Variables * var,myTypes type) : varname(varname), vars(var),type(type) {}
    myTypes getType() { return type; }
	float getValue() {
        return vars->getValue(varname)->getValue(); // Resolve to a number
	}
    void graph(int parent, int & index,bool def=false) {
        int id=++index;
        cout<<id<<"[label=\"\&lt;"<< (def ? "d" : "u") <<"&gt; variable | " << varname << "\" shape=\"Mrecord\" color=\"blue\"]"<<endl;
         cout<<parent<<"->"<<id<<endl;
        //cout<<++index<<"[label=\""<<varname<<"\"]"<<endl;
        //cout<<id<<"->"<<index<<endl;
        if(vars->getValue(varname)) vars->getValue(varname)->graph(id,index);
    }
};

class ReturnEx : public Expression {
    Expression *val;
    public:
    ReturnEx(Expression * val) : val(val) {}
    myTypes getType() { return val->getType(); }
    float getValue() {
        return val->getValue(); // Resolve
    }
    void graph(int parent, int & index,bool def=false) {
        int id=++index;
        cout<<id<<"[label=\"return\"]"<<endl;
         cout<<parent<<"->"<<id<<endl;
        if(val) val->graph(id,index);
    }
};


class BinaryExprEx : public Expression {
	char OP;
	Expression *LHS;
	Expression *RHS;
	public:
	BinaryExprEx(char op, Expression *lhs, Expression * rhs) : OP(op), LHS(lhs), RHS(rhs) {}
    myTypes getType() { return LHS->getType(); }
    float getValue() {
		if(RHS == 0 || LHS == 0) return 0;
		switch(OP) {
			case '+':
				return LHS->getValue()+RHS->getValue();
				break;
			case '-':
				return LHS->getValue()-RHS->getValue();
				break;
			case '*':
				return LHS->getValue()*RHS->getValue();
				break;
			case '/':
				return LHS->getValue()/RHS->getValue();
				break;
			}
		return 0;
	}
    void graph(int parent, int & index,bool def=false) {
        int id=++index;
        cout<<id<<"[label=\"binary expr\"]"<<endl;
         cout<<parent<<"->"<<id<<endl;
        cout<<id+1<<"[label=\""<<OP<<"\"]"<<endl;
        cout<<id++<<"->"<<id<<endl;
        index++;
        LHS->graph(id,index);
        RHS->graph(id,index);
    }
};


class ParamEx : public Expression {
    std::string type;
    std::string name;
    public:
    ParamEx(std::string name, std::string type) : type(type), name(name) {}
    myTypes getType() {
        if(type=="int") return T_INT;
        if(type=="float") return T_FLOAT;
    }
    float getValue() {
        return 0;
    }
    void graph(int parent, int & index,bool def=false) {
        int id=++index;
		cout<<id<<"[label=\"{param | { " << type << " | " << name << "}}\" shape=\"Mrecord\" color=\"green\"]"<<endl;
         cout<<parent<<"->"<<id<<endl;

        //cout<<id+1<<"[label=\""<<type<<"\"]"<<endl;
        //cout<<id<<"->"<<++index<<endl;
        //cout<<id+2<<"[label=\""<<name<<"\"]"<<endl;
        //cout<<id<<"->"<<++index<<endl;
        //index+=2;
    }
};

class BlockEx : public Expression {
    vector<Expression *> * body;
    public:
    BlockEx() : body(new vector<Expression *>()) {};
	~BlockEx() { delete body; };
    void addLine(Expression * p) { if(p != 0) body->push_back(p); }
    myTypes getType() { return T_VOID; }
    float getValue() {return 0;}
    void graph(int parent, int & index,bool def=false) {
        int id=++index;
        cout<<id<<"[label=\"block\" shape=\"box\"]"<<endl;
        cout<<parent<<"->"<<id<<endl;
        for(auto i : *body) {
            i->graph(id,index,true);
        }
    }
};


class FunctionDefEx : public Expression {
    myTypes type;
    string name;
    float value;
    Expression *param;
    Expression * body;
    public:
    FunctionDefEx(string p_name,Expression *p_param,Expression * body, myTypes type) : name(p_name),value(0),param(p_param),body(body),type(type) {};
    myTypes getType() { return type; }
    float getValue() {return value;}
    void graph(int parent, int & index,bool def=false) {
        int id=++index;
        cout<<id<<"[label=\"def "<<name<<"\" color=\"red\"]"<<endl;
        cout<<parent<<"->"<<id<<endl;
        // FIXME attach to subtree for closures
        if(param) param->graph(id,index);
        if(body) body->graph(id,index);
    }
};

class FunctionCallEx : public Expression {
    string name;
    float value;
    Expression *param;
    parser * p;
    public:
    FunctionCallEx(string p_name,Expression *p_param, parser * p) : name(p_name),value(0),param(p_param),p(p) {};
    myTypes getType() {
        Expression * temp = p->getFunction(name);
        if(temp) return temp->getType();
        else return T_VOID;
    }

    float getValue() {return value;}
    void graph(int parent, int & index,bool def=false) {
        int id=++index;
        cout<<id<<"[label=\"call to "<<name<<"\"]"<<endl;
         cout<<parent<<"->"<<id<<endl;
        cout<<id+1<<"[label=\"param\"]"<<endl;
        cout<<id<<"->"<<++index<<endl;
        if(param) param->graph(index,index);
    }
};
