enum tokenType {VAR, NUMBER, ASSIGNMENT, OPERATOR, OPEN, CLOSE, EOL=10, END, INVAL};

class token {
	public:
	std::string text;
	tokenType type;
	int lpos;

public:
	token() : type(INVAL) {};
	token(tokenType b, std::string a) : text(a), type(b), lpos(0) {};
	token(tokenType b, std::string a, int lc) : text(a), type(b), lpos(lc) {};

	token& ty(tokenType v) { type = v; return *this;};
	token& str(std::string v) { text = v; return *this;};
	
	const tokenType ty() const { return type;};
	const std::string str() const { return text;};
	int ln() const { return lpos;};

	bool operator()()  const { return type != INVAL;};
};

using namespace std;

class lexer {
	int linecount;
	ifstream * infile;
	string line;
	string currentLex;
	stringstream * linestream;
	tokenType currentType;
	bool first;
		
	bool isOperator(char);
	bool acceptChar();
	bool acceptDot();
	bool acceptDigit();
	bool acceptNumber();
	bool acceptOperator();
	bool acceptOpen();
	bool acceptClose();
	bool acceptAssignment();
	void removeTrailing();
	void setType(tokenType,int);
		
	public:
	lexer();
	~lexer();
	void openFile(char *);
	token getNext(bool);
	token readLast();
	string getCurrentLine();
	bool newLine();
};
