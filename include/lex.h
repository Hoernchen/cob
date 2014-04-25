enum tokenType {VAR, NUMBER, ASSIGNMENT, OPERATOR, OPEN, CLOSE, EOL=10, END};

struct token {
	std::string text;
	tokenType type;
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
	token makeToken(tokenType,string &);
		
	public:
	lexer();
	~lexer();
	void openFile(char *);
	token getNext(bool);
	token readLast();
	string getCurrentLine();
	bool newLine();
};