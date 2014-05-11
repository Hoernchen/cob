#include <map>
#include <vector>
#include "lex.h"


class Expression {
    public:
    virtual ~Expression() {}
    virtual float getValue()=0;
    virtual void graph(int parent, int & index)=0;
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

class PackageEx : public Expression {
    std::string name;
    vector<Expression *> * body;
    public:
    PackageEx(std::string name) : name(name), body(new vector<Expression *>()) {}
	~PackageEx()  { delete body; }
    void addLine(Expression * p) { if(p != 0) body->push_back(p); }
    float getValue() {
        return 0;
    }
    void graph(int parent, int & index) {
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
	float getValue() {return value;}
    void graph(int parent, int & index) {
        int id=++index;
        cout<<id<<"[label=\""<<value<<"\"]"<<endl;
         cout<<parent<<"->"<<id<<endl;
    }
};

class VariableEx : public Expression {
	std::string varname;
	Variables * vars;
	public:
	VariableEx(std::string varname, Variables * var) : varname(varname), vars(var) {}
	float getValue() {
        return vars->getValue(varname)->getValue(); // Resolve to a number
	}
    void graph(int parent, int & index) {
        int id=++index;
        cout<<id<<"[label=\"variable | " << varname << "\" shape=\"Mrecord\" color=\"blue\"]"<<endl;
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
    float getValue() {
        return val->getValue(); // Resolve
    }
    void graph(int parent, int & index) {
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
    void graph(int parent, int & index) {
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
    float getValue() {
        return 0;
    }
    void graph(int parent, int & index) {
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
    float getValue() {return 0;}
    void graph(int parent, int & index) {
        int id=++index;
        cout<<id<<"[label=\"block\" shape=\"box\"]"<<endl;
        cout<<parent<<"->"<<id<<endl;
        for(auto i : *body) {
            i->graph(id,index);
        }
    }
};


class FunctionDefEx : public Expression {
    string name;
    float value;
    Expression *param;
    Expression * body;
    public:
    FunctionDefEx(string p_name,Expression *p_param,Expression * body) : name(p_name),value(0),param(p_param),body(body) {};
    float getValue() {return value;}
    void graph(int parent, int & index) {
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
    public:
    FunctionCallEx(string p_name,Expression *p_param) : name(p_name),value(0),param(p_param) {};
    float getValue() {return value;}
    void graph(int parent, int & index) {
        int id=++index;
        cout<<id<<"[label=\"call to "<<name<<"\"]"<<endl;
         cout<<parent<<"->"<<id<<endl;
        cout<<id+1<<"[label=\"param\"]"<<endl;
        cout<<id<<"->"<<++index<<endl;
        if(param) param->graph(index,index);
    }
};





class parser {
	Variables * vars;
	lexer * mylex;
	token currentToken;
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

	public:
	parser(char* path);
	~parser();
	token getNext(bool);
    bool parseFile(int &);
};
