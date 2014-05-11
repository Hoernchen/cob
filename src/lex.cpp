/* Lexer 0.99 Alpha 
 * Valid lexemes are:
 * Variables: [A-Za-z]+
 * Number: [0-9]+(\.[0-9]+)?
 * Operator: [\+\-\*\/\(\)]{1}
 * Assignment: (\:\=){1}
*/

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <cctype>

#include "lex.h"

lexer::lexer() : linecount(0),infile(0),linestream(0), first(true) {
	linestream=new stringstream();
}

lexer::~lexer() {
	if(infile==NULL) return;
	infile->close();
	delete infile;
	delete linestream;
}

void lexer::openFile(char * filepath) {
	first=true;
	infile = new ifstream(filepath);
}

string lexer::getCurrentLine() {
	return line;
}

void lexer::setType(tokenType type, int pos) {
	// cout<<"Type modified "<<pos<<endl;
	currentType=type;
}

bool lexer::newLine() {
	if(infile->good()) { // Get new line if there's more in the ifstream
		linecount++;
		getline(*infile,line);
		if(linestream != NULL) delete linestream;
		linestream=new stringstream(line);
		return true;
	}
	else return false;
}

bool lexer::isOperator(char p) {
	if(p == '*' || p == '+' || p == '-' || p == '/') return true;
	else return false;
}

bool lexer::acceptChar() {
	if(isalpha(linestream->peek())) {
        setType(WORD,1);
		currentLex+=linestream->get();
		return acceptChar();
	}
	if(!currentLex.empty() && isalpha(currentLex.back()))
		return true;
	else return false;
}

bool lexer::acceptDot() {
	if(linestream->peek() == '.') {
		currentLex+=linestream->get();
		if(!isdigit(linestream->peek())){
			//throw runtime_error("A dot must be followed by a number");
			linestream->unget();
			currentLex.pop_back();
			return false;
		}
		return true;
	}
	return false;
}

bool lexer::acceptDigit() {
	if(isdigit(linestream->peek())) {
		currentLex+=linestream->get();
		return acceptDigit();
	}
	if(!currentLex.empty() && isdigit(currentLex.back()))
		return true;
	else return false;
};


bool lexer::acceptNumber() {
	acceptDigit();
	acceptDot();
	acceptDigit();
	if(!currentLex.empty() && isdigit(currentLex.back())) {
		setType(NUMBER,2);
		return true;
	}
	return false;
}

bool lexer::acceptOperator() {
	if(isOperator(linestream->peek())) {
		currentLex+=linestream->get();
		setType(OPERATOR,3);
		return true;
	}
	return false;
}

bool lexer::acceptAssignment() {
	if(linestream->peek() == ':') {
		currentLex+=linestream->get();
		if(linestream->peek() == '=') {
			currentLex+=linestream->get();
			setType(ASSIGNMENT,4);
			return true;
		}
		linestream->unget();
		currentLex.pop_back();
		//else throw runtime_error(": must be followed by =");
	}
	return false;
}

bool lexer::acceptOpen() {
	if(linestream->peek() == '(') {
		currentLex+=linestream->get();
		setType(OPEN,5);
		return true;
	}
	else return false;
}

bool lexer::acceptClose() {
	if(linestream->peek() == ')') {
		currentLex+=linestream->get();
		setType(CLOSE,6);
		return true;
	}
	else return false;
}

bool lexer::acceptCurlOpen() {
    if(linestream->peek() == '{') {
        currentLex+=linestream->get();
        setType(CURLOPEN,6);
        return true;
    }
    else return false;
}

bool lexer::acceptCurlClose() {
    if(linestream->peek() == '}') {
        currentLex+=linestream->get();
        setType(CURLCLOSE,6);
        return true;
    }
    else return false;
}

// Must be called before acceptOperator!
bool lexer::acceptComment() {
    if(linestream->peek() == '/') {
        currentLex+=linestream->get();
        if(linestream->peek() == '/') {
            currentLex+=linestream->get();
            cerr<<"Filtered comment: "<<linestream->str().substr(line.length()-linestream->rdbuf()->in_avail()-2,linestream->rdbuf()->in_avail()+2)<<endl;
            newLine(); // Discard rest of current line
            setType(EOL,0);
            return true;
        }
        else linestream->unget();
    }
    return false;
}

void lexer::removeTrailing() {
		while(isspace(linestream->peek())) linestream->get();
}


token lexer::getNext(bool forceNew=false) {
	currentLex.clear();

	// get next line from file if necessary
	if(linestream->rdbuf()->in_avail() <= 0 || linestream->peek() == ';' || forceNew==true) {
		if(!newLine()) // Get new line if there's more in the ifstream
			return token(END,currentLex,linecount); // ifstream is empty
        setType(EOL,7);
        return token(EOL,currentLex,linecount);
    }
	
	// Test all possible types of lexeme
	removeTrailing();
    if(acceptComment());
    else if (acceptOperator());
	else if(acceptAssignment());
	else if(acceptNumber());
	else if(acceptChar());
	else if(acceptOpen());
	else if(acceptClose());
    else if(acceptCurlOpen());
    else if(acceptCurlClose());
	else return token(); // Someone tried to feed us crap
	
	return token(currentType,currentLex,linecount); // Return valid lexeme
}

token lexer::readLast() {
	return token(currentType,currentLex,linecount);
}


/*
int main() {
	lexer * myLex=new lexer();
	char in[] = "testinput.txt";
	myLex->openFile(in);
	token foo;
	do {
		try {
			foo=myLex->getNext();
			// cout<<myLex->getCurrentLine()<<endl;
			if(foo.type != EOL && foo.type != END) cout<<foo.text<<endl;
			if(foo.type == EOL) cout<<"END OF LINE"<<endl;
			if(foo.type == END) cout<<"END"<<endl;
		}
		catch(runtime_error ex) {
			cout<<ex.what()<<" at line: "<<myLex->getCurrentLine()<<endl;
			exit(-1);
		}
	}
	while(foo.type != END);
	delete myLex;
}
*/
