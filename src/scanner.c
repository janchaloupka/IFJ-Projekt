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
	//do{
		scannerFSM(file, output);
	//}while((*output)->type == T_UNKNOWN);

	if((*output)->type == T_ID){
		// TODO: Kontrola, jestli ID je keyword
	}
}

void scannerFSM(FILE *file, pToken *output){
	sState state = STATE_START;
	sState nextState;

	pToken token = malloc(sizeof(struct Token));
	token->type = T_UNKNOWN;
	*output = token;

	bool isActive = true;
	static unsigned int lineNumber = 1;
	static unsigned int linePos = 1;
	static int currChar = -2;
	if(currChar == -2) currChar = getc(file);
	
	printf("\n#%u-%u\t", lineNumber, linePos);

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
				else if(currChar == ',') nextState = STATE_COMMA;
				else if(currChar == EOL) nextState = STATE_EOL;
				else if(currChar == EOF) nextState = STATE_EOF;
				else if(isdigit(currChar)) nextState = STATE_INT;
				else if(islower(currChar) || currChar == '_') nextState = STATE_ID;
				else if(isspace(currChar)) nextState = STATE_START;
				break;
			case STATE_LBR:
				token->type = T_LBRCKT;
				break;
			case STATE_RBR:
				token->type = T_RBRCKT;
				break;
			case STATE_COMMA:
				token->type = T_COMMA;
				break;
			case STATE_EOL:
				token->type = T_EOL;
				lineNumber++;
				linePos = 1;
				break; // TODO: Block comment
			case STATE_EOF:
				token->type = T_EOF;
				break;
			// Expr
			case STATE_ADD:
				token->type = T_ADD;
				break;
			case STATE_SUB:
				token->type = T_SUB;
				break;
			case STATE_MUL:
				token->type = T_MUL;
				break;
			case STATE_DIV:
				token->type = T_DIV;
				break;
			case STATE_GT:
				if(currChar == '=') nextState = STATE_GTE;
				else token->type = T_GT;
				break;
			case STATE_GTE:
				token->type = T_GTE;
				break;
			case STATE_LT:
				if(currChar == '=') nextState = STATE_LTE;
				else token->type = T_LT;
				break;
			case STATE_LTE:
				token->type = T_LTE;
				break;
			case STATE_EQL:
				token->type = T_EQL;
				break;
			case STATE_NEQ:
				if(currChar == '=') nextState = STATE_NEQ2;
				break;
			case STATE_NEQ2:
				token->type = T_NEQ;
				break;
			case STATE_ASSIGN:
				if(currChar == '=') nextState = STATE_EQL;
				else token->type = T_ASSIGN;
				break;
			// Comment
			case STATE_LCMNT:
				if(currChar == EOL) nextState = STATE_EOL;
				else nextState = STATE_LCMNT;
				break;
			// Id
			case STATE_ID:
				if(currChar == '?' || currChar == '!') nextState = STATE_ID_FN;
				else if(isalpha(currChar) || isdigit(currChar) || currChar == '_') 
					nextState = STATE_ID;
				else token->type = T_ID;
				break;
			case STATE_ID_FN:
				token->type = T_IDFN;
				break;
			// String
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
				token->type = T_STRING;
				break;
			// Number
			case STATE_INT0:
				if(tolower(currChar) == 'e') nextState = STATE_EXP;
				else if(currChar == '.') nextState = STATE_DBLE;
				else if(!isdigit(currChar)) token->type = T_INTEGER;
				break;
			case STATE_INT:
				if(isdigit(currChar)) nextState = STATE_INT;
				else if(currChar == '.') nextState = STATE_DBLE;
				else if(tolower(currChar) == 'e') nextState = STATE_EXP;
				else token->type = T_INTEGER;
				break;
			case STATE_DBLE:
				if(isdigit(currChar)) nextState = STATE_DBLE2;
				break;
			case STATE_DBLE2:
				if(isdigit(currChar)) nextState = STATE_DBLE2;
				else if(tolower(currChar) == 'e') nextState = STATE_EXP;
				else token->type = T_DOUBLE;
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
				else token->type = T_DOUBLE_E;
				break;
		}

		if(token->type != T_UNKNOWN){
			isActive = false;
			return;
		}else if(nextState == STATE_ERROR){
			isActive = false;
			scannerHandleError(state, currChar, lineNumber, linePos);
		}
		
		if(currChar != EOF){
			currChar = getc(file);
			linePos++;
		}
		state = nextState;
	}
}

bool scannerFoundError = false;

void scannerHandleError(tType state, char currChar, unsigned int line, unsigned int pos){
	fprintf(stderr, "[SCANNER] Error on line %d:%d - ", line, pos);
	
	switch(state){
		case STATE_STR:
			fprintf(stderr, "Expected ASCII char >= 0x20 in string, got");
			break;
		case STATE_STR2:
			fprintf(stderr, "Expected '\"','n','t','s','x' or '\\', got ");
			break;
		case STATE_STR3:
			fprintf(stderr, "Expected a-f or digit after 'x', got ");
			break;
		case STATE_INT0:
			fprintf(stderr, "Integer cannot start with '0', found ");
			break;
		case STATE_DBLE:
			fprintf(stderr, "Expected digit after decimal point, got ");
			break;
		case STATE_EXP:
			fprintf(stderr, "Expected digit or sign in exponent, got ");
			break;
		case STATE_EXP2:
			fprintf(stderr, "Expected digit after sign in exponent, got ");
			break;
		case STATE_NEQ:
			fprintf(stderr, "; Expected '=' after '!', got ");
			break;
		default:
			fprintf(stderr, "Unexpected ");
			break;
	}

	fprintf(stderr, "'%c' (0x%x)", currChar, currChar);
	if((int)state == (int)STATE_INT0) fprintf(stderr, " after '0'");
	fprintf(stderr, "\n");
	
	scannerFoundError = true;
}

void scannerPrintToken(pToken token){
	char *type[] = {
		"UNKNOWN", 	//!< Výchozí typ
		"ID", 		//!< Identifikátor funkce/proměnné
		"IDFN", 	//!< Identifikátor funkce
		"DEF",		//!< Keyword - DEF
		"DO",		//!< Keyword - DO
		"ELSE",		//!< Keyword - ELSE
		"END",		//!< Keyword - END
		"IF",		//!< Keyword - IF
		"NOT",		//!< Keyword - NOT
		"NIL",		//!< Keyword - NIL
		"THEN",		//!< Keyword - THEN
		"WHILE",	//!< Keyword - WHILE
		"RBRCKT",	//!< Levá závorka
		"LBRCKT",	//!< Pravá závorka
		"COMMA",	//!< Oddělovací čárka
		"EOL",		//!< Konec řádku
		"EOF",		//!< Konec souboru
		"ADD",		//!< Matematika - Sčítání
		"SUB",		//!< Matematika - Krácení
		"MUL",		//!< Matematika - Násobení
		"DIV",		//!< Matematika - Dělení
		"EQL",		//!< Porovnání - Rovná se
		"NEQ",		//!< Porovnání - Nerovná se
		"LT",		//!< Porovnání - Menší
		"GT",		//!< Porovnání - Větší
		"LTE",		//!< Porovnání - Menší nebo se rovná
		"GTE",		//!< Porovnání - Větší nebo se rovná
		"ASSIGN",	//!< Přiřazení
		"INTEGER",	//!< Celé kladné číslo
		"DOUBLE",	//!< Desetinné kladné číslo
		"DOUBLE_E",	//!< Desetinné kladné číslo zapsané exponentem
		"STRING",	//!< Řetězec znaků
		"ERROR"		//!< Speciální typ, při parsování tohoto úseku nastala chyba
	};

	printf(type[token->type]);
}