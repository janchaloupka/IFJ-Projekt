#include "scanner.h"

int scannerGetTokenList(pToken *firstToken, FILE *file){
	pToken prevToken = NULL;
	
	while(prevToken == NULL || prevToken->type != T_EOF){
		pToken newToken;
		int ret = scannerGetToken(&newToken, file);
		
		if(prevToken == NULL) *firstToken = newToken;
		else prevToken->nextToken = newToken;
		newToken->prevToken = prevToken;
		prevToken = newToken;

		if(ret != 0){
			scannerFreeTokenList(&prevToken);
			*firstToken = NULL;
			return ret; // Nastala chyba v získání tokenu
		}
	}

	return 0;
}

int scannerGetToken(pToken *token, FILE *file){
	if(file == NULL) file = stdin;
	if(token == NULL) return 99;
	
	*token = malloc(sizeof(struct Token));
	(*token)->data = NULL;
	(*token)->prevToken = NULL;
	(*token)->nextToken = NULL;
	
	bool isError = false;
	do{
		free((*token)->data);
		(*token)->data = NULL;
		if(_scannerFSM(file, *token)) isError = true;
	}while(	(isError && (*token)->type != T_EOF) || 
			(*token)->type == T_UNKNOWN ||
			(*token)->linePos == 0 );

	_scannerIsKeyword(*token);

	return isError ? 1 : 0;
}

int _scannerFSM(FILE *file, pToken token){
	sState state = STATE_START;
	sState nextState;
	

	bool isActive = true;
	static unsigned int linePos = 0;
	static unsigned int colPos = 1;
	static int currChar = -2;
	if(currChar == -2) currChar = EOL;

	if(file == NULL){
		linePos = 0;
		colPos = 1;
		currChar = -2;
		return 0;
	}

	token->type = T_UNKNOWN;
	token->colPos = colPos;
	token->linePos = linePos;
	
	unsigned int rawStrPos = 0;
	unsigned int rawStrLength = STRING_CHUNK_LEN;
	char *rawStr = malloc(sizeof(char)*rawStrLength);

	while(isActive){
		if(rawStrPos >= rawStrLength){
			rawStrLength += STRING_CHUNK_LEN;
			rawStr = realloc(rawStr, sizeof(char)*rawStrLength);
		}
		rawStr[rawStrPos] = currChar;
		rawStrPos++;
		nextState = STATE_NULL;
		
		switch(state){
			case STATE_START:
				if(currChar == '!') nextState = STATE_NEQ;
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
				else if(isspace(currChar)) nextState = STATE_SPACE;
				else nextState = STATE_ERROR;
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
				if(currChar == '=') nextState = STATE_BCMT;
				else token->type = T_EOL;
				linePos++;
				colPos = 1;
				break;
			case STATE_EOF:
				token->type = T_EOF;
				break;
			case STATE_SPACE:
				if(isspace(currChar) && currChar != EOL) nextState = STATE_SPACE;
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
				else nextState = STATE_ERROR;
				break;
			case STATE_NEQ2:
				token->type = T_NEQ;
				break;
			case STATE_ASSIGN:
				if(currChar == '=') nextState = STATE_EQL;
				else token->type = T_ASSIGN;
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
				else nextState = STATE_ERROR;
				break;
			case STATE_STR2:
				if(currChar == 'x') nextState = STATE_STR3;
				else if(currChar == '"' || currChar == 'n' || currChar == 't' ||
						currChar == 's' || currChar == '\\')
					nextState = STATE_STR;
				else nextState = STATE_ERROR;
				break;
			case STATE_STR3:
				if(isdigit(currChar) || 
				  (tolower(currChar) >= 'a' && tolower(currChar) <= 'f'))
					nextState = STATE_STR;
				else nextState = STATE_ERROR;
				break;
			case STATE_STR4:
				token->type = T_STRING;
				break;
			// Number
			case STATE_INT0:
				if(tolower(currChar) == 'e') nextState = STATE_EXP;
				else if(currChar == '.') nextState = STATE_DBLE;
				else if(!isdigit(currChar)) token->type = T_INTEGER;
				else nextState = STATE_ERROR;
				break;
			case STATE_INT:
				if(isdigit(currChar)) nextState = STATE_INT;
				else if(currChar == '.') nextState = STATE_DBLE;
				else if(tolower(currChar) == 'e') nextState = STATE_EXP;
				else token->type = T_INTEGER;
				break;
			case STATE_DBLE:
				if(isdigit(currChar)) nextState = STATE_DBLE2;
				else nextState = STATE_ERROR;
				break;
			case STATE_DBLE2:
				if(isdigit(currChar)) nextState = STATE_DBLE2;
				else if(tolower(currChar) == 'e') nextState = STATE_EXP;
				else token->type = T_FLOAT;
				break;
			case STATE_EXP:
				if(isdigit(currChar)) nextState = STATE_EXP3;
				else if(currChar == '+' || currChar == '-') nextState = STATE_EXP2;
				else nextState = STATE_ERROR;
				break;
			case STATE_EXP2:
				if(isdigit(currChar)) nextState = STATE_EXP3;
				else nextState = STATE_ERROR;
				break;
			case STATE_EXP3:
				if(isdigit(currChar)) nextState = STATE_EXP3;
				else token->type = T_FLOAT;
				break;
			// Comment
			case STATE_LCMNT:
				if(currChar != EOL && currChar != EOF) nextState = STATE_LCMNT;
				break;
			case STATE_BCMT:
				if(currChar == 'b') nextState = STATE_BCMT2;
				else nextState = STATE_ERROR;
				break;
			case STATE_BCMT2:
				if(currChar == 'e') nextState = STATE_BCMT3;
				else nextState = STATE_ERROR;
				break;
			case STATE_BCMT3:
				if(currChar == 'g') nextState = STATE_BCMT4;
				else nextState = STATE_ERROR;
				break;
			case STATE_BCMT4:
				if(currChar == 'i') nextState = STATE_BCMT5;
				else nextState = STATE_ERROR;
				break;
			case STATE_BCMT5:
				if(currChar == 'n') nextState = STATE_BCMT6;
				else nextState = STATE_ERROR;
				break;
			case STATE_BCMT6:
				if(currChar == EOL) nextState = STATE_BCMT8;
				else if(isspace(currChar)) nextState = STATE_BCMT7;
				else nextState = STATE_ERROR;
				break;
			case STATE_BCMT7:
				if(currChar == EOL) nextState = STATE_BCMT8;
				else if(currChar == EOF) nextState = STATE_ERROR;
				else nextState = STATE_BCMT7;
				break;
			case STATE_BCMT8:
				if(currChar == '=') nextState = STATE_BCMT9;
				else if(currChar == EOL) nextState = STATE_BCMT8;
				else if(currChar == EOF) nextState = STATE_ERROR;
				else nextState = STATE_BCMT7;
				linePos++;
				colPos = 1;
				break;
			case STATE_BCMT9:
				if(currChar == 'e') nextState = STATE_BCMT10;
				else if(currChar == EOL) nextState = STATE_BCMT8;
				else if(currChar == EOF) nextState = STATE_ERROR;
				else nextState = STATE_BCMT7;
				break;
			case STATE_BCMT10:
				if(currChar == 'n') nextState = STATE_BCMT11;
				else if(currChar == EOL) nextState = STATE_BCMT8;
				else if(currChar == EOF) nextState = STATE_ERROR;
				else nextState = STATE_BCMT7;
				break;
			case STATE_BCMT11:
				if(currChar == 'd') nextState = STATE_BCMT12;
				else if(currChar == EOL) nextState = STATE_BCMT8;
				else if(currChar == EOF) nextState = STATE_ERROR;
				else nextState = STATE_BCMT7;
				break;
			case STATE_BCMT12:
				if(isspace(currChar) && currChar != EOL) nextState = STATE_BCMT13;
				else if(currChar != EOF && currChar != EOL) nextState = STATE_BCMT7;
				break;
			case STATE_BCMT13:
				if(currChar != EOF && currChar != EOL) nextState = STATE_BCMT13;
				break;
			default:
				// TODO: Něco udělat když se dostaneme do divnýho stavu
				break;
		}

		if(token->type != T_UNKNOWN || nextState == STATE_NULL){
			break; 
		}else if(nextState == STATE_ERROR){
			_scannerHandleError(state, currChar, linePos, colPos);
			if(state != STATE_START) break;
			isActive = false;
		}
		
		if(currChar != EOF){
			currChar = getc(file);
			colPos++;
		}
		state = nextState;
	}

	// Rozhodování, jestli se má string zachovat nebo ne
	switch(state){
		case STATE_STR4:
		case STATE_INT:
		case STATE_INT0:
		case STATE_DBLE2:
		case STATE_EXP3:
		case STATE_ID:
		case STATE_ID_FN:
			rawStr[rawStrPos-1] = '\0';
			rawStr = realloc(rawStr, rawStrPos);
			token->data = rawStr;
			break;
		default:
			free(rawStr);
			break;
	}

	return nextState == STATE_ERROR ? 1 : 0;
}

bool _scannerIsKeyword(pToken token){
	if(token->type != T_ID) return false;

	tType newType = token->type;
	if(strcmp(token->data, "def") == 0) newType = T_DEF;
	else if(strcmp(token->data, "do") == 0) newType = T_DO;
	else if(strcmp(token->data, "else") == 0) newType = T_ELSE;
	else if(strcmp(token->data, "end") == 0) newType = T_END;
	else if(strcmp(token->data, "if") == 0) newType = T_IF;
	else if(strcmp(token->data, "not") == 0) newType = T_NOT;
	else if(strcmp(token->data, "nil") == 0) newType = T_NIL;
	else if(strcmp(token->data, "then") == 0) newType = T_THEN;
	else if(strcmp(token->data, "while") == 0) newType = T_WHILE;

	if(token->type != newType){
		free(token->data);
		token->data = NULL;
		token->type = newType;
		return true;
	}

	return false;
}

void scannerFreeToken(pToken *token){
	if(token == NULL || *token == NULL) return;

	switch((*token)->type){
		case T_STRING:
		case T_INTEGER:
		case T_FLOAT:
		case T_ID:
		case T_IDFN:
			free((*token)->data);
			break;
		default:
			break;
	}

	free(*token);
	*token = NULL;
}

void scannerFreeTokenList(pToken *token){
	if(token == NULL || *token == NULL) return;

	while((*token)->nextToken != NULL){
		*token = (*token)->nextToken;
	}

	while(*token != NULL){
		pToken prev = (*token)->prevToken;
		scannerFreeToken(token);
		*token = prev;
	}
}

void _scannerHandleError(sState state, char currChar, unsigned int line, unsigned int col){
	fprintf(stderr, "[SCANNER] Error on line %d:%d - ", line, col);
	
	switch(state){
		case STATE_STR:
			fprintf(stderr, "Expected printable ASCII char (>= 0x20) in string, found ");
			break;
		case STATE_STR2:
			fprintf(stderr, "Expected '\"','n','t','s','x' or '\\' after '\\', found ");
			break;
		case STATE_STR3:
			fprintf(stderr, "Expected a-f or digit after 'x', found ");
			break;
		case STATE_INT0:
			fprintf(stderr, "Integer cannot start with '0', found digit ");
			break;
		case STATE_DBLE:
			fprintf(stderr, "Expected digit after decimal point, found ");
			break;
		case STATE_EXP:
			fprintf(stderr, "Expected digit or +,- sign in exponent, found ");
			break;
		case STATE_EXP2:
			fprintf(stderr, "Expected digit after +,- sign in exponent, found ");
			break;
		case STATE_NEQ:
			fprintf(stderr, "Expected '=' after '!', found ");
			break;
		case STATE_BCMT7:
		case STATE_BCMT8:
		case STATE_BCMT9:
		case STATE_BCMT10:
		case STATE_BCMT11:
			fprintf(stderr, "Expected \"=end\", found ");
			break;
		default:
			fprintf(stderr, "Unexpected ");
			break;
	}
	
	if(currChar >= 0x20){
		fprintf(stderr, "'%c'", currChar);
	}else if(currChar == EOL || currChar == '\r'){
		fprintf(stderr, "EOL");
	}else if(currChar == EOF){
		fprintf(stderr, "EOF");
	}else{
		fprintf(stderr, "0x%x", currChar);
	}
	
	if(state == STATE_INT0) 
		fprintf(stderr, " after '0'");
	else if(state >= STATE_BCMT7 && state <= STATE_BCMT11) 
		fprintf(stderr, "; Reached end of file but block comment is still opened");
	fprintf(stderr, "\n");
}

const char *scannerTypeToString(tType type){
	switch(type){
		case T_ID: 		return "ID"; break;
		case T_IDFN: 	return "IDFN"; break;
		case T_DEF: 	return "DEF"; break;
		case T_DO: 		return "DO"; break;
		case T_ELSE: 	return "ELSE"; break;
		case T_END: 	return "END"; break;
		case T_IF: 		return "IF"; break;
		case T_NOT:	 	return "NOT"; break;
		case T_NIL: 	return "NIL"; break;
		case T_THEN:	return "THEN"; break;
		case T_WHILE: 	return "WHILE"; break;
		case T_RBRCKT: 	return "RBRCKT"; break;
		case T_LBRCKT: 	return "LBRCKT"; break;
		case T_COMMA: 	return "COMMA"; break;
		case T_EOL: 	return "EOL"; break;
		case T_EOF: 	return "EOF"; break;
		case T_ADD: 	return "ADD"; break;
		case T_SUB: 	return "SUB"; break;
		case T_MUL: 	return "MUL"; break;
		case T_DIV: 	return "DIV"; break;
		case T_EQL: 	return "EQL"; break;
		case T_NEQ: 	return "NEQ"; break;
		case T_LT: 		return "LT"; break;
		case T_GT: 		return "GT"; break;
		case T_LTE: 	return "LTE"; break;
		case T_GTE: 	return "GTE"; break;
		case T_ASSIGN: 	return "ASSIGN"; break;
		case T_INTEGER: return "INTEGER"; break;
		case T_FLOAT: 	return "DOUBLE"; break;
		case T_STRING: 	return "STRING"; break;
		default: return "UNKNOWN"; break;
	}
}

void scannerPrintToken(pToken token){
	if(token == NULL){
		printf("pToken is NULL \n");
		return;
	}

	printf("#%d:%d\t", token->linePos, token->colPos);
	
	printf(scannerTypeToString(token->type));
	
	switch(token->type){
		case T_STRING:
		case T_INTEGER:
		case T_FLOAT:
		case T_ID:
		case T_IDFN:
			printf("(%s)", token->data);
			break;
		default:
			break;
	}
	printf("\n");
}

void scannerPrintTokenList(pToken token){
	if(token == NULL){
		printf("pToken is NULL \n");
		return;
	}

	while(token->prevToken != NULL){
		token = token->prevToken;
	}

	while(token->nextToken != NULL){
		scannerPrintToken(token);
		token = token->nextToken;
	}
}