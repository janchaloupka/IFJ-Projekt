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

void scannerDebugPrint(pToken firstToken){
	char *typeToStr[] = {
		"UNKNOWN",
		"IDENTIFIER",
		"BLOCK_KW_START",
		"KW_DEF",
		"KW_DO",
		"KW_ELSE",
		"KW_END",
		"KW_IF",
		"KW_NOT",
		"KW_NIL",
		"KW_THEN",
		"KW_WHILE",
		"BLOCK_KW_END",
		"BLOCK_SEP_START",
		"SEP_RB",
		"SEP_LB",
		"SEP_EOL",
		"BLOCK_SEP_END",
		"BLOCK_OP_START",
		"OP_ADD",
		"OP_SUB",
		"OP_MUL",
		"OP_DIV",
		"OP_EQL",
		"OP_NEQ",
		"OP_LT",
		"OP_GT",
		"OP_LTE",
		"OP_GTE",
		"OP_ASGMT",
		"BLOCK_OP_END",
		"BLOCK_NUM_START",
		"INTEGER",
		"DOUBLE",
		"DOUBLE_EXP",
		"BLOCK_NUM_END",
		"COMMENT",
		"STRING",
		"ERROR"
	};

	while(firstToken != NULL){
		printf("TYPE: %s\n", typeToStr[firstToken->type]);
		printf("LINE: %d\n", firstToken->line);
		printf("------------\n");

		firstToken = firstToken->tNext;
	}
}


int scannerParse(char* input, pToken *output){
	// Základní kostra FSM automatu, není hotový
	
	pToken currToken = malloc(sizeof(struct Token));
	currToken->line = 1;
	currToken->tNext = NULL;
	currToken->type = UNKNOWN;

	*output = currToken;

	sState state = STATE_START;

	// TODO: Použít pointery funkcí, zmenší to kód
	//void (*stateFunc)(char, pToken, void*, bool*) = *scannerFSM_START;
	
	int pos = -1;
	bool requestNextChar = true;
	do{
		if(requestNextChar) pos++;
		
		if(state == STATE_START && currToken->type != UNKNOWN){
			pToken newToken = malloc(sizeof(struct Token));
			newToken->line = currToken->line;
			newToken->tNext = NULL;
			newToken->type = UNKNOWN;
			
			currToken->tNext = newToken;
			currToken = newToken;
		}

		requestNextChar = true;
		//(*stateFunc)(input[pos], token, stateFunc, &requestNextChar);
		if(state == STATE_START) scannerFSM_START(input[pos], currToken, &state, &requestNextChar);
		else if(state == STATE_EQL) scannerFSM_EQL(input[pos], currToken, &state, &requestNextChar);
		else if(state == STATE_LT) scannerFSM_LT(input[pos], currToken, &state, &requestNextChar);
		else if(state == STATE_GT) scannerFSM_GT(input[pos], currToken, &state, &requestNextChar);
		else if(state == STATE_NEQ) scannerFSM_NEQ(input[pos], currToken, &state, &requestNextChar);
		else if(state == STATE_LCOMNT) scannerFSM_LCOMNT(input[pos], currToken, &state, &requestNextChar);
		else if(state == STATE_INT0) scannerFSM_INT0(input[pos], currToken, &state, &requestNextChar);
		else if(state == STATE_INT) scannerFSM_INT(input[pos], currToken, &state, &requestNextChar);
		else if(state == STATE_DBLE) scannerFSM_DBLE(input[pos], currToken, &state, &requestNextChar);
		else if(state == STATE_DBLE2) scannerFSM_DBLE2(input[pos], currToken, &state, &requestNextChar);
		else if(state == STATE_EXP) scannerFSM_EXP(input[pos], currToken, &state, &requestNextChar);
		else if(state == STATE_EXP2) scannerFSM_EXP2(input[pos], currToken, &state, &requestNextChar);
		else if(state == STATE_EXP3) scannerFSM_EXP3(input[pos], currToken, &state, &requestNextChar);
		else{
			// TODO: Ošetřit chybu, neznámý stav FSM
			printf("Chyba lexikalniho analyzatoru, FSM se dostal do neznameho stavu");
		}

	}while(input[pos] != '\0' && input[pos] != EOF);
	
	
	return 0;
}

// OPERATOR '=', '==', nebo BLOCK COMMENT
void scannerFSM_EQL(char input, pToken token, sState *nextState, bool *requestNextChar){
	if(input == '='){
		token->type = OP_EQL;
	}else if(input == 'b'){
		// TODO: Možná zaačátek blokovýho commentu
	}else{
		token->type = OP_ASGMT;
		*requestNextChar = false;
	}

	*nextState = STATE_START;
}

// OPERATOR < nebo <=
void scannerFSM_LT(char input, pToken token, sState *nextState, bool *requestNextChar){
	if(input == '='){
		token->type = OP_LTE;
	}else{
		token->type = OP_LT;
		*requestNextChar = false;
	}

	*nextState = STATE_START;
}

// OPERATOR > nebo >=
void scannerFSM_GT(char input, pToken token, sState *nextState, bool *requestNextChar){
	if(input == '='){
		token->type = OP_GTE;
	}else{
		token->type = OP_GT;
		*requestNextChar = false;
	}
	
	*nextState = STATE_START;
}

// OPERATOR !=
void scannerFSM_NEQ(char input, pToken token, sState *nextState, bool *requestNextChar){
	if(input == '='){
		token->type = OP_NEQ;
	}else{
		// TODO: ERROR, za ! musí být =
		token->type = ERROR;
		*requestNextChar = false;
	}
	
	*nextState = STATE_START;
}

// Řádkový komentář
void scannerFSM_LCOMNT(char input, pToken token, sState *nextState, bool *requestNextChar){
	token->type = COMMENT;
	if(input == '\n'){
		*requestNextChar = false;
		*nextState = STATE_START;
	}
}

// Číslo začínající nulou
// jako další znak je povolený jen 'e' a '.' (str. 3, pozn. 5 a 6 zadání)
void scannerFSM_INT0(char input, pToken token, sState *nextState, bool *requestNextChar){
	token->type = INTEGER;
	if(isdigit(input)){
		// TODO: ERROR, celé číslo nemůže začínat nulou
		token->type = ERROR;
		*nextState = STATE_START;
	}else if(input == '.'){
		token->type = DOUBLE;
		*nextState = STATE_DBLE;
	}else if(input == 'e' || input == 'E'){
		token->type = DOUBLE_EXP;
		*nextState = STATE_EXP;
	}else{
		*requestNextChar = false;
		*nextState = STATE_START;
	}
}

// Začítek celého čísla, může následovat tečka nebo exponent
void scannerFSM_INT(char input, pToken token, sState *nextState, bool *requestNextChar){
	token->type = INTEGER;
	if(input == '.'){
		token->type = DOUBLE;
		*nextState = STATE_DBLE;
	}else if(input == 'e' || input == 'E'){
		token->type = DOUBLE_EXP;
		*nextState = STATE_EXP;
	}else if(!isdigit(input)){
		*requestNextChar = false;
		*nextState = STATE_START;
	}
}

// Začátek desetinného čísla, kontrola jestli je za tečkou číslo
void scannerFSM_DBLE(char input, pToken token, sState *nextState, bool *requestNextChar){
	if(isdigit(input)){
		*nextState = STATE_DBLE2;
	}else{
		// TODO: ERROR, po '.' musí být číslo
		token->type = ERROR;
		*nextState = STATE_START;
	}
}

// Desetinné číslo
void scannerFSM_DBLE2(char input, pToken token, sState *nextState, bool *requestNextChar){
	if(input == 'e' || input == 'E'){
		token->type = DOUBLE_EXP;
		*nextState = STATE_EXP;
	}else if(!isdigit(input)){
		*requestNextChar = false;
		*nextState = STATE_START;
	}
}

// Začítek exponentu, kontrola přítomnosti + nebo - nebo čísla
void scannerFSM_EXP(char input, pToken token, sState *nextState, bool *requestNextChar){
	if(input == '+' || input == '-'){
		*nextState = STATE_EXP2;
	}else if(isdigit(input)){
		*nextState = STATE_EXP3;
	}else{
		// TODO: ERROR, po 'e' musí být číslo
		token->type = ERROR;
		*nextState = STATE_START;
	}
}

// Pokud je za exponentem + nebo -, zkrontrolovat jestli následuje číslo
void scannerFSM_EXP2(char input, pToken token, sState *nextState, bool *requestNextChar){
	if(isdigit(input)){
		*nextState = STATE_EXP3;
	}else{
		// TODO: ERROR, po 'e' musí být číslo
		token->type = ERROR;
		*nextState = STATE_START;
	}
}

// Desetinné číslo s exponentem
void scannerFSM_EXP3(char input, pToken token, sState *nextState, bool *requestNextChar){
	if(!isdigit(input)){
		*requestNextChar = false;
		*nextState = STATE_START;
	}
}


void scannerFSM_START(char input, pToken token, sState *nextState, bool *requestNextChar){
	if(input == '(') token->type = SEP_LB;
	else if(input == ')') token->type = SEP_RB;
	else if(input == '+') token->type = OP_ADD;
	else if(input == '-') token->type = OP_SUB;
	else if(input == '*') token->type = OP_MUL;
	else if(input == '/') token->type = OP_DIV;
	else if(input == '=') *nextState = STATE_EQL;
	else if(input == '<') *nextState = STATE_LT;
	else if(input == '>') *nextState = STATE_GT;
	else if(input == '!') *nextState = STATE_NEQ;
	else if(input == '"') printf("String not implemented\n");
	else if(input == '#') *nextState = STATE_LCOMNT;
	else if(input == '0') *nextState = STATE_INT0;
	else if(isdigit(input)) *nextState = STATE_INT;
	else if(islower(input) || input == '_') 
		printf("Identifiers not implemented\n"); // KEYWORD nebo IDENTIFIER
	else if(input == '\n'){
		token->type = SEP_EOL;
		token->line++;
	}
	else if(!isspace(input)){
		// Neznámý znak, nejspíš chyba
		// TODO: Ošetřit chybu
		token->type = ERROR;
	}
}