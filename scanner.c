#include "scanner.h"

char const *keywords[] = {
	"def",
	"do",
	"else",
	"end",
	"if",
	"not",
	"nil",
	"then",
	"while"
};

int scannerParse(char* input, pToken output){
	// Základní kostra FSM automatu, zdaleka není hotový
	for(int pos = 0; input[pos] != '\0'; pos++){
		char currChar = input[pos];
		switch(currChar){
			case '(':
			case ')':
			case '\n':
				// SEPARATOR
				//token->type = SEPARATOR;
				break;

			case '+':
			case '-':
			case '*':
			case '/':
				// OPERATOR
				//token->type = OPERATOR	;
				break;

			case '=':
				// OPERATOR '=', '==', nebo BLOCK COMMENT
				break;

			case '<':
			case '>':
				// OPERATOR může následovat znak '='
				//token->type = OPERATOR;
				break;

			case '!':
				//token->type = OPERATOR;
				// OPERATOR musí následovat znak '='
				break;

			case '"':
				// STRING
				//token->type = STRING;
				break;

			case '#':
				// LINE COMMENT
				//token->type = COMMENT;
				break;

			case '0':
				// NUMBER nebo DECIMAL 
				// jako další znak je povolený jen 'e' (str. 3, pozn. 5 a 6 zadání)
				break;

			default:
			if(currChar >= 49 && currChar <= 57){ // 1-9
				// NUMBER nebo DECIMAL

			}else if((currChar >= 97 && currChar <= 122) || currChar == '_'){ // a-z _
				// KEYWORD nebo IDENTIFIER

			}else{
				// Něco úplně jinýho, asi whitespace?
			}
		}
	}

	return 0;
}