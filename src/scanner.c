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

void scannerGetToken(FILE *file, pToken *output){
	scannerFSM(file, output);
	if((*output)->type == ID){
		// TODO: Kontrola, jestli ID je keyword
	}
}

void scannerFSM(FILE *file, pToken *output){
	sState state = STATE_START;
	sState nextState;

	pToken token = malloc(sizeof(struct Token));
	token->type = UNKNOWN;
	*output = token;

	bool isActive = true;
	static int currChar = -10;
	if(currChar == -10) currChar = getc(file);
	
	while(isActive){
		nextState = STATE_ERROR;
		switch(state){
			case STATE_START:
				if(currChar == '!') state = STATE_NEQ;
				else if(currChar == '>') nextState = STATE_GT;
				else if(currChar == '<') nextState = STATE_LT;
				else if(currChar == '=') nextState = STATE_ASSIGN;
				else if(currChar == '+') nextState = STATE_ADD;
				else if(currChar == '-') nextState = STATE_SUB;
				else if(currChar == '*') nextState = STATE_MUL;
				else if(currChar == '/') nextState = STATE_DIV;
				else if(currChar == '#') nextState = STATE_LCMNT;
				else if(currChar == '(') nextState = STATE_LBR;
				else if(currChar == ')') nextState = STATE_RBR;
				else if(currChar == '"') nextState = STATE_STR;
				else if(currChar == '0') nextState = STATE_INT0;
				else if(currChar == EOL) nextState = STATE_EOL;
				else if(currChar == EOF) nextState = STATE_EOF;
				else if(isdigit(currChar)) nextState = STATE_INT;
				else if(islower(currChar) || currChar == '_') nextState = STATE_ID;
				else if(isspace(currChar)) nextState = STATE_START;
				break;
			case STATE_ADD:
				token->type = OP_ADD;
				break;
			case STATE_SUB:
				token->type = OP_SUB;
				break;
			case STATE_MUL:
				token->type = OP_MUL;
				break;
			case STATE_DIV:
				token->type = OP_DIV;
				break;
			case STATE_GTE:
				token->type = OP_GTE;
				break;
			case STATE_LTE:
				token->type = OP_LTE;
				break;
			case STATE_EQL:
				token->type = OP_EQL;
				break;
			case STATE_NEQ2:
				token->type = OP_NEQ;
				break;
			case STATE_LBR:
				token->type = SEP_LB;
				break;
			case STATE_RBR:
				token->type = SEP_RB;
				break;
			case STATE_EOF:
				token->type = SEP_EOF;
				break;
			case STATE_NEQ:
				if(currChar == '=') nextState = STATE_NEQ2;
				break;
			case STATE_GT:
				if(currChar == '=') nextState = STATE_GTE;
				else token->type = OP_GT;
				break;
			case STATE_LT:
				if(currChar == '=') nextState = STATE_LTE;
				else token->type = OP_LT;
				break;
			case STATE_ASSIGN:
				if(currChar == '=') nextState = STATE_EQL;
				else token->type = OP_ASGMT;
				break;
			case STATE_EOL:
				token->type = SEP_EOL;
				break; // TODO: Block comment
			case STATE_LCMNT:
				if(currChar == EOL) nextState = STATE_EOL;
				else nextState = STATE_LCMNT;
				break;
			case STATE_ID:
				if(currChar == '?' || currChar == '!') nextState = STATE_ID_FN;
				else if(isalpha(currChar) || isdigit(currChar) || currChar == '_') 
					nextState = STATE_ID;
				else token->type = ID;
				break;
			case STATE_ID_FN:
				token->type = ID_FN;
				break;
			case STATE_STR:
				if(currChar == '\\') nextState = STATE_STR2;
				else if(currChar == '"') nextState = STATE_STR4;
				else if(currChar >= 32) nextState = STATE_STR;
				break;
			case STATE_STR2:
				if(currChar == 'x') nextState = STATE_STR3;
				else if(currChar == '"' || currChar == 'n' || currChar == 't' ||
						currChar == 's' || currChar == '\\')
					nextState = STATE_STR;
				break;
			case STATE_STR3:
				if(isdigit(currChar) || 
				  (tolower(currChar) >= 'a' && tolower(currChar) <= 'f'))
					nextState = STATE_STR;
				break;
			case STATE_STR4:
				token->type = STRING;
				break;
			case STATE_INT0:
				if(tolower(currChar) == 'e') nextState = STATE_EXP;
				else if(currChar == '.') nextState = STATE_DBLE;
				else token->type = INTEGER;
				break;
			case STATE_INT:
				if(isdigit(currChar)) nextState = STATE_INT;
				else if(currChar == '.') nextState = STATE_DBLE;
				else if(tolower(currChar) == 'e') nextState = STATE_EXP;
				else token->type = INTEGER;
				break;
			case STATE_DBLE:
				if(isdigit(currChar)) nextState = STATE_DBLE2;
				break;
			case STATE_DBLE2:
				if(isdigit(currChar)) nextState = STATE_DBLE2;
				else if(tolower(currChar) == 'e') nextState = STATE_EXP;
				else token->type = DOUBLE;
				break;
			case STATE_EXP:
				if(isdigit(currChar)) nextState = STATE_EXP3;
				else if(currChar == '+' || currChar == '-') nextState = STATE_EXP2;
				break;
			case STATE_EXP2:
				if(isdigit(currChar)) nextState = STATE_EXP3;
				break;
			case STATE_EXP3:
				if(isdigit(currChar)) nextState = STATE_EXP3;
				else token->type = DOUBLE_EXP;
				break;
		}

		if(nextState == STATE_ERROR){
			isActive = false;
		}else if(token->type != UNKNOWN){
			isActive = false;
		}else{
			if(currChar != EOF) currChar = getc(file);
			state = nextState;
		}
	}
}

void scannerPrintType(pToken token){
	char *type[] = {"UNKNOWN", 	//!< Výchozí typ
	"ID", 		//!< Identifikátor funkce/proměnné
	"ID_FN", 		//!< Identifikátor funkce
	"KW_DEF",		//!< Keyword - DEF
	"KW_DO",		//!< Keyword - DO
	"KW_ELSE",	//!< Keyword - ELSE
	"KW_END",		//!< Keyword - END
	"KW_IF",		//!< Keyword - IF
	"KW_NOT",		//!< Keyword - NOT
	"KW_NIL",		//!< Keyword - NIL
	"KW_THEN",	//!< Keyword - THEN
	"KW_WHILE",	//!< Keyword - WHILE
	"SEP_RB",		//!< Levá závorka
	"SEP_LB",		//!< Pravá závorka
	"SEP_EOL",	//!< Konec řádku
	"SEP_EOF",	//!< Konec souboru
	"OP_ADD",		//!< Matematika - Sčítání
	"OP_SUB",		//!< Matematika - Krácení
	"OP_MUL",		//!< Matematika - Násobení
	"OP_DIV",		//!< Matematika - Dělení
	"OP_EQL",		//!< Porovnání - Rovná se
	"OP_NEQ",		//!< Porovnání - Nerovná se
	"OP_LT",		//!< Porovnání - Menší
	"OP_GT",		//!< Porovnání - Větší
	"OP_LTE",		//!< Porovnání - Menší nebo se rovná
	"OP_GTE",		//!< Porovnání - Větší nebo se rovná
	"OP_ASGMT",	//!< Přiřazení
	"INTEGER",	//!< Celé kladné číslo
	"DOUBLE",		//!< Desetinné kladné číslo
	"DOUBLE_EXP",	//!< Desetinné kladné číslo zapsané exponentem
	"STRING",		//!< Řetězec znaků
	"ERROR"};

	printf(type[token->type]);
}