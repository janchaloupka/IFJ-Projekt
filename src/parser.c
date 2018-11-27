#include "parser.h"
#include "expressions.h"

int parser(pToken *List){

	pToken token = (*List);		// List pro průchod syntaxe
	pToken preRun = (*List);	// List pro pre-run
	
	int error = 0;				// Chyba vstupního kódu
	int internalError = 0;			// Interní chyba překladače

	psTree funcTable;			// Hlavní tabulka definovaných funkcí
	psTree varTable;			// Hlavní tabulka proměnných
	psTree localTable = NULL;	// Lokální proměnné
	pToken func = NULL;			// ... této funkce (func->data)

	symTabInit(&funcTable);
	symTabInit(&varTable);

	parserSemanticsInitBuiltIn(&funcTable);	// Naplnění tabulky built-in funkcema

	SyntaxStack S;							// Stack pro rekurzivní sestup
	pSemanticsStack semanticError;			// Stack sémantických chyb

	parserSyntaxStackInit(&S, &internalError);
	parserSemanticStackInit(&semanticError, &internalError);

	bool tokenChecked = false;
	bool inFunc = false;	// Je-li true, jsme ve funkci
	int inAux = 0;			// Semafor? Za každý if/while ++, za každý END --

	while(preRun != NULL){	// Sémantický pre-run, naplnění tabulky definicemi funkcí
		parserSemanticsPreRun(&preRun, &funcTable, semanticError, &internalError, &error);	// Naplnění tabulky definicí funkcí
		parserSyntaxError(error, internalError, NULL, &S);	// Pouze pro výpis erroru 42
		preRun = preRun->nextToken;
	}

	while(token != NULL){	// Syntaktická analýza + Sémantická analýza

		pToken prevToken = token;

		if(tokenChecked){
			// Neudělej nic, protože jsi to už udělal
		}

		else{
			parserSemanticsInFunc(&inFunc, &inAux, token);	// Jsme-li ve funkci - tj. mezi DEF a příslušným END
			parserSemanticsCheck(token, &func, &funcTable, &varTable, &localTable, semanticError, inFunc, &internalError); // Víceméně jen check IDček
			tokenChecked = true;
		}

		if(S.a[S.last] > T_STRING){
			if(!inFunc) localTable = varTable;
			parserSyntaxExpand(&S, &token, &error, &internalError, localTable);	// Je-li na stacku neterminál
		}

		else{
			parserSyntaxCompare(S, token, &error);	// Je-li na stacku s čím porovnávat
			parserSyntaxStackPop(&S, &internalError);
			parserSyntaxIDFNCheck(token, &funcTable, &error);	// Kontrola ? a ! na konci proměnných

			token = token->nextToken;
			tokenChecked = false;	// Nový token pro sémantickou analýzu
		}

		error = parserSyntaxError(error, internalError, &prevToken, &S);
		if(error){
			
			// Úklid

			symTabDispose(&varTable);
			symTabDispose(&funcTable);
			parserSemanticStackDelete(&semanticError);
			parserSyntaxStackDelete(&S);
			return error;
		} 
	}

	error = parserSemanticError(semanticError);	// Došli jsme až sem, syntax je tedy správně, pokud jsou sémantické chyby, měly by i dávat smysl

	// Úklid

	symTabDispose(&varTable);
	symTabDispose(&funcTable);
	parserSemanticStackDelete(&semanticError);
	parserSyntaxStackDelete(&S);
	return error;
}




/******************************************************SYNTAX******************************************************************************/

void parserSyntaxCompare(SyntaxStack S, pToken token, int *error){
	if (S.a[S.last] == token->type){

	}
	else if(!*error) *error = 2;
}

void parserSyntaxExpand(SyntaxStack *S, pToken *token, int *error, int *internalError, psTree localTable){
	if(S->a[S->last] == N_PROG){
		if((*token)->type == T_ID ||
		(*token)->type == T_NIL ||
		(*token)->type == T_INTEGER ||
		(*token)->type == T_STRING ||
		(*token)->type == T_FLOAT ||
		(*token)->type == T_NOT ||
		(*token)->type == T_LBRCKT ||
		(*token)->type == T_IF ||
		(*token)->type == T_WHILE ||
		(*token)->type == T_EOL){
			parserSyntaxStackPop(&(*S), internalError);
			parserSyntaxStackPush(&(*S), N_PROG, internalError);
			parserSyntaxStackPush(&(*S), N_BODY, internalError);
		}

		else if((*token)->type == T_DEF){
			parserSyntaxStackPop(&(*S), internalError);
			parserSyntaxStackPush(&(*S), N_PROG, internalError);
			parserSyntaxStackPush(&(*S), T_EOL, internalError);
			parserSyntaxStackPush(&(*S), N_DEFUNC, internalError);
		}

		else if((*token)->type == T_EOF){
			parserSyntaxStackPop(&(*S), internalError);
			parserSyntaxStackPush(&(*S), T_EOF, internalError);
		}
		
		else if(!*error) *error = 2;
	}

	else if(S->a[S->last] == N_BODY){
		parserSyntaxStackPop(&(*S), internalError);

		if((*token)->type == T_ID){
			parserSyntaxStackPush(&(*S), N_BODY, internalError);
			parserSyntaxStackPush(&(*S), T_EOL, internalError);
			parserSyntaxStackPush(&(*S), N_BODY_ID, internalError);
			parserSyntaxStackPush(&(*S), T_ID, internalError);
		}

		else if((*token)->type == T_NIL ||
		(*token)->type == T_INTEGER ||
		(*token)->type == T_STRING ||
		(*token)->type == T_FLOAT ||
		(*token)->type == T_NOT ||
		(*token)->type == T_LBRCKT){
			parserSyntaxStackPush(&(*S), N_BODY, internalError);
			parserSyntaxStackPush(&(*S), T_EOL, internalError);
			parserSyntaxStackPush(&(*S), N_EXPR, internalError);
		}

		else if((*token)->type == T_IF){
			parserSyntaxStackPush(&(*S), N_BODY, internalError);
			parserSyntaxStackPush(&(*S), T_EOL, internalError);
			parserSyntaxStackPush(&(*S), N_IF, internalError);
		}

		else if((*token)->type == T_WHILE){
			parserSyntaxStackPush(&(*S), N_BODY, internalError);
			parserSyntaxStackPush(&(*S), T_EOL, internalError);
			parserSyntaxStackPush(&(*S), N_WHILE, internalError);
		}

		else if((*token)->type == T_EOL){
			parserSyntaxStackPush(&(*S), N_BODY, internalError);
			parserSyntaxStackPush(&(*S), T_EOL, internalError);
		}
	}

	else if(S->a[S->last] == N_BODY_ID){
		if((*token)->type == T_ID ||
		(*token)->type == T_NIL ||
		(*token)->type == T_INTEGER ||
		(*token)->type == T_STRING ||
		(*token)->type == T_FLOAT ||
		(*token)->type == T_LBRCKT){
			parserSyntaxStackPop(&(*S), internalError);
			parserSyntaxStackPush(&(*S), N_FUNC, internalError);
		}

		else if((*token)->type == T_ADD ||
		(*token)->type == T_SUB ||
		(*token)->type == T_MUL ||
		(*token)->type == T_DIV ||
		(*token)->type == T_EQL ||
		(*token)->type == T_NEQ ||
		(*token)->type == T_GT ||
		(*token)->type == T_LT ||
		(*token)->type == T_GTE ||
		(*token)->type == T_LTE){
			parserSyntaxStackPop(&(*S), internalError);
			parserSyntaxStackPush(&(*S), N_EXPR_O, internalError);
		}

		else if((*token)->type == T_ASSIGN){
			parserSyntaxStackPop(&(*S), internalError);
			parserSyntaxStackPush(&(*S), N_DEFVAR, internalError);
			parserSyntaxStackPush(&(*S), T_ASSIGN, internalError);
		}

		else if((*token)->type == T_EOL){
			parserSyntaxStackPop(&(*S), internalError);
		}

		else if(!*error) *error = 2;
	}

	else if(S->a[S->last] == N_TYPE){
		if((*token)->type == T_NIL){
			parserSyntaxStackPop(&(*S), internalError);
			parserSyntaxStackPush(&(*S), T_NIL, internalError);	
		}

		else if((*token)->type == T_INTEGER){
			parserSyntaxStackPop(&(*S), internalError);
			parserSyntaxStackPush(&(*S), T_INTEGER, internalError);	
		}

		else if((*token)->type == T_STRING){
			parserSyntaxStackPop(&(*S), internalError);
			parserSyntaxStackPush(&(*S), T_STRING, internalError);	
		}

		else if((*token)->type == T_FLOAT){
			parserSyntaxStackPop(&(*S), internalError);
			parserSyntaxStackPush(&(*S), T_FLOAT, internalError);	
		}

		else if(!*error) *error = 2;
	}

	else if(S->a[S->last] == N_DEFUNC){
		if((*token)->type == T_DEF){
				parserSyntaxStackPop(&(*S), internalError);
				parserSyntaxStackPush(&(*S), T_END, internalError);
				parserSyntaxStackPush(&(*S), N_BODY, internalError);
				parserSyntaxStackPush(&(*S), T_EOL, internalError);
				parserSyntaxStackPush(&(*S), T_RBRCKT, internalError);
				parserSyntaxStackPush(&(*S), N_PARS, internalError);
				parserSyntaxStackPush(&(*S), T_LBRCKT, internalError);
				parserSyntaxStackPush(&(*S), T_ID, internalError);
				parserSyntaxStackPush(&(*S), T_DEF, internalError);
		}

		else if(!*error) *error = 2;
	}

	else if(S->a[S->last] == N_FUNC){
		parserSyntaxStackPop(&(*S), internalError);

		if((*token)->type == T_ID ||
		(*token)->type == T_NIL ||
		(*token)->type == T_INTEGER ||
		(*token)->type == T_STRING ||
		(*token)->type == T_FLOAT){
			parserSyntaxStackPush(&(*S), N_PARS, internalError);
		}
		
		else if ((*token)->type == T_LBRCKT){
			parserSyntaxStackPush(&(*S), T_RBRCKT, internalError);
			parserSyntaxStackPush(&(*S), N_PARS, internalError);
			parserSyntaxStackPush(&(*S), T_LBRCKT, internalError);
		}
	}

	else if(S->a[S->last] == N_PARS){
		parserSyntaxStackPop(&(*S), internalError);

		if((*token)->type == T_ID){
			parserSyntaxStackPush(&(*S), N_PARSN, internalError);
			parserSyntaxStackPush(&(*S), T_ID, internalError);
		}

		else if((*token)->type == T_NIL ||
		(*token)->type == T_INTEGER ||
		(*token)->type == T_STRING ||
		(*token)->type == T_FLOAT){
			parserSyntaxStackPush(&(*S), N_PARSN, internalError);
			parserSyntaxStackPush(&(*S), N_TYPE, internalError);
		}
	}

	else if(S->a[S->last] == N_PARSN){
		parserSyntaxStackPop(&(*S), internalError);

		if((*token)->type == T_COMMA){
			parserSyntaxStackPush(&(*S), N_PARS, internalError);
			parserSyntaxStackPush(&(*S), T_COMMA, internalError);
		}
	}

	else if(S->a[S->last] == N_IF){
		if((*token)->type == T_IF){
			parserSyntaxStackPop(&(*S), internalError);
			parserSyntaxStackPush(&(*S), T_END, internalError);
			parserSyntaxStackPush(&(*S), N_BODY, internalError);
			parserSyntaxStackPush(&(*S), T_EOL, internalError);
			parserSyntaxStackPush(&(*S), T_ELSE, internalError);
			parserSyntaxStackPush(&(*S), N_BODY, internalError);
			parserSyntaxStackPush(&(*S), T_EOL, internalError);
			parserSyntaxStackPush(&(*S), T_THEN, internalError);
			parserSyntaxStackPush(&(*S), N_EXPR, internalError);
			parserSyntaxStackPush(&(*S), T_IF, internalError);
		}

		else if(!*error) *error = 2;
	}

	else if(S->a[S->last] == N_WHILE){
		if((*token)->type == T_WHILE){
			parserSyntaxStackPop(&(*S), internalError);
			parserSyntaxStackPush(&(*S), T_END, internalError);
			parserSyntaxStackPush(&(*S), N_BODY, internalError);
			parserSyntaxStackPush(&(*S), T_EOL, internalError);
			parserSyntaxStackPush(&(*S), T_DO, internalError);
			parserSyntaxStackPush(&(*S), N_EXPR, internalError);
			parserSyntaxStackPush(&(*S), T_WHILE, internalError);
		}

		else if(!*error) *error = 2;
	}

	else if(S->a[S->last] == N_DEFVAR){
		if((*token)->type == T_ID){
			parserSyntaxStackPop(&(*S), internalError);
			parserSyntaxStackPush(&(*S), N_DEFVARID, internalError);
			parserSyntaxStackPush(&(*S), T_ID, internalError);
		}

		else if((*token)->type == T_NIL ||
		(*token)->type == T_INTEGER ||
		(*token)->type == T_FLOAT ||
		(*token)->type == T_STRING ||
		(*token)->type == T_NOT ||
		(*token)->type == T_LBRCKT){
			parserSyntaxStackPop(&(*S), internalError);
			parserSyntaxStackPush(&(*S), N_EXPR, internalError);
		}

		else if(!*error) *error = 2;
	}

	else if(S->a[S->last] == N_DEFVARID){
		parserSyntaxStackPop(&(*S), internalError);

		if((*token)->type == T_LBRCKT ||
		(*token)->type == T_ID ||
		(*token)->type == T_NIL ||
		(*token)->type == T_INTEGER ||
		(*token)->type == T_FLOAT ||
		(*token)->type == T_STRING){
			parserSyntaxStackPush(&(*S), N_FUNC, internalError);
		}

		else if((*token)->type == T_ADD ||
		(*token)->type == T_SUB ||
		(*token)->type == T_DIV ||
		(*token)->type == T_MUL ||
		(*token)->type == T_EQL ||
		(*token)->type == T_NEQ ||
		(*token)->type == T_LT ||
		(*token)->type == T_GT ||
		(*token)->type == T_LTE ||
		(*token)->type == T_GTE){
			parserSyntaxStackPush(&(*S), N_EXPR_O, internalError);
		}
	} 

	else if(S->a[S->last] == N_EXPR_O){
		*token = (*token)->prevToken;
		if(exprParse(&(*token), localTable)) 
			*error = 69;

		if((*token)->type == T_EOL ||
		(*token)->type == T_EOF){
			parserSyntaxStackPop(&(*S), internalError);
		}

		else if(!*error) *error = 2;
	}

	else if(S->a[S->last] == N_EXPR){
		if (exprParse(&(*token), localTable)) 
			*error = 69;

		if((*token)->type == T_EOL ||
		(*token)->type == T_THEN ||
		(*token)->type == T_DO ||
		(*token)->type == T_EOF){
			parserSyntaxStackPop(&(*S), internalError);
		}

		else if(!*error) *error = 2;
	}

	else{
		if (!*internalError) *internalError = 2;	// Neočekávaný token na stacku
	}
}

int parserSyntaxError(int error, int internalError, pToken *prevToken, SyntaxStack *S){
	if(internalError == 1){
		fprintf(stderr, "[INTERNAL ERROR]: Unexpected token on stack!\n");
		return 99;
	}

	if(internalError == 2){
		fprintf(stderr, "[INTERNAL ERROR]: Failed malloc!\n");
		return 99;
	} 

	if(error){	//Syntax error
			if(error == 2){
				fprintf(stderr, "[SYNTAX] Error on line %u:%u - expected %s, got %s\n",
					(*prevToken)->linePos, (*prevToken)->colPos, scannerTypeToString((*S).a[(*S).top]), scannerTypeToString((*prevToken)->type));
				return 2;
			}

			else if(error == 32){	//Syntaktický error expressionu
				return 2;
			}

			else if(error == 69){
				fprintf(stderr, "[SYNTAX] Error: Only function names can end with \"!\" and \"?\" symbols!\n");
				return 2;
			}

			else if(error == 42){
				fprintf(stderr, "[SYNTAX] Error: Only variables can be used when defining parameters of a function!\n");
				return 2;
			}

			else{
				fprintf(stderr, "[INTERNAL ERROR]: Unexplainable unbelievable problem!\n");
				return 99;
			}
		}

	return 0;
}

void parserSyntaxIDFNCheck(pToken token, psTree *funcTable, int *error){
	if(token->type == T_ID && (token->data[strlen(token->data) - 1] == '!' || token->data[strlen(token->data) - 1] == '?'))
		if(!symTabSearch(funcTable, token->data))
			if(!*error) *error = 69;
}




/*******************************************************STACK*****************************************************************************/

void parserSyntaxStackInit(SyntaxStack *S, int *internalError){
	S->top = 1;
	S->last = 0;

	if(!(S->a = malloc(STACK_CHUNK_SIZE * sizeof(tType)))){
		fprintf(stderr, "[INTERNAL ERROR]: Failed malloc during stack initialization!\n");
		if (!*internalError) *internalError = 1;
	}

	S->size = STACK_CHUNK_SIZE;
	S->a[0] = N_PROG; 				// Výchozí neterminál je <prog>
}

void parserSyntaxStackDelete(SyntaxStack *S){
	free(S->a);
}

void parserSyntaxStackPush(SyntaxStack *S, tType type, int *internalError){
	if (S->top == S->size){
		S->size += STACK_CHUNK_SIZE;
		S->a = realloc(S->a, S->size * sizeof(tType));
		if(S->a == NULL){
			fprintf(stderr, "[INTERNAL ERROR]: Failed malloc during stack expansion!\n");
			if (!*internalError) *internalError = 1;
		}
	}
	
	S->a[S->top] = type;
	S->top++;
	S->last++;
}

tType parserSyntaxStackPop(SyntaxStack *S, int *internalError){
	if (S->top==0) {
		fprintf(stderr, "[INTERNAL ERROR]: Stack underflow!\n");
		if (!*internalError) *internalError = 1;
		return(T_UNKNOWN);
	}	
	else {
		S->last--;
		return(S->a[S->top--]); 
	}	
}




/*****************************************************SÉMANTIKA***************************************************************************/

void parserSemanticsInitBuiltIn(psTree *funcTable){
	symTabInsert(funcTable, "print", parserSemanticsInitData(FUNC, NULL, -1));
	symTabInsert(funcTable, "inputs", parserSemanticsInitData(FUNC, NULL, 0));
	symTabInsert(funcTable, "inputi", parserSemanticsInitData(FUNC, NULL, 0));
	symTabInsert(funcTable, "inputf", parserSemanticsInitData(FUNC, NULL, 0));
	symTabInsert(funcTable, "length", parserSemanticsInitData(FUNC, NULL, 1));
	symTabInsert(funcTable, "substr", parserSemanticsInitData(FUNC, NULL, 3));
	symTabInsert(funcTable, "ord", parserSemanticsInitData(FUNC, NULL, 2));
	symTabInsert(funcTable, "chr", parserSemanticsInitData(FUNC, NULL, 1));
	return;
}

void parserSemanticsPreRun(pToken *token, psTree *funcTable, pSemanticsStack semanticError, int *internalError, int *error){
	if((*token)->type == T_ID){

		if((*token)->prevToken != NULL && (*token)->prevToken->type == T_DEF){
			
			if(!(symTabSearch(funcTable, (*token)->data))){
				psTree localTable;
				symTabInit(&localTable);
				psData data = parserSemanticsInitData(FUNC, localTable, 0);
				pToken param;
				if((*token)->nextToken != NULL) param = (*token)->nextToken->nextToken;

				while(param != NULL){
					if(param->type == T_COMMA){
						param = param->nextToken;
					}

					else if(param->type == T_ID){
						symTabInsert(&localTable, param->data, parserSemanticsInitData(VAR, NULL, 0));
						param = param->nextToken;
						data->params++;
					}

					else if(param->type == T_STRING || param->type == T_FLOAT || param->type == T_INTEGER || param->type == T_NIL){
						*error = 42;
					}

					else break;
				}

				symTabInsert(funcTable, (*token)->data, data);
			}

			else{
				 parserSemanticStackPush(semanticError, 1, (*token)->data, internalError, (*token)->linePos, (*token)->colPos);
			}
		}
	}
}

void parserSemanticsCheck(pToken token, pToken *func, psTree *funcTable, psTree *varTable, psTree *localTable, pSemanticsStack semanticError, bool inFunc, int *internalError){
	
	/*******************************Local frame***********************************************************/

	if(inFunc && token->type == T_DEF){	// Nacházíme se ve funkci, tedy zřídíme localTable a zapamatujem si token s názvem funkce
		*localTable = symTabSearch(funcTable, token->nextToken->data)->localFrame; // Najdem localTable v tabulce fukcí
		*func = token->nextToken;	// Identifikátor funkce po DEF
	}

	/*******************************Definice proměnné*****************************************************/

	if(token->type == T_ID && token->nextToken->type == T_ASSIGN){	// Je-li to definice proměnné
		if(!inFunc){					// A pokud nejsme nikde ve funkci
			if(symTabSearch(funcTable, token->data)){
				parserSemanticStackPush(semanticError, 2, token->data, internalError, token->linePos, token->colPos);
			}
			symTabInsert(varTable, token->data, parserSemanticsInitData(VAR, NULL, 0));
		}

		else{							// Pokud jsme ve funkci
			if(symTabSearch(funcTable, token->data)){
				parserSemanticStackPush(semanticError, 3, token->data, internalError, token->linePos, token->colPos);
			}
			symTabInsert(localTable, token->data, parserSemanticsInitData(VAR, NULL, 0));
		}
	}

	/*******************************Volání funkce**********************************************************/

	else if((token->type == T_ID && symTabSearch(funcTable, token->data)) || (token->type == T_ID && token->prevToken->type != T_DEF && (token->nextToken->type == T_LBRCKT || token->nextToken->type == T_ID || token->nextToken->type == T_FLOAT || token->nextToken->type == T_STRING || token->nextToken->type == T_INTEGER || token->nextToken->type == T_NIL))){
		if(symTabSearch(funcTable, token->data)){				// Pokud je definovaná
			
			psData func_data = symTabSearch(funcTable, token->data);	// Uložit si data o funkci z tabulky (kvůli počtu parametrů)
			pToken param = NULL;

			if(token->nextToken->type == T_LBRCKT) param = token->nextToken->nextToken; // Dostat se k prvnímu parametru
			else param = token->nextToken;

			int params = 0;

			while(param->type != T_EOL && param->type != T_RBRCKT){			// Spočítáme parametry

				if(param->data != NULL && (symTabSearch(funcTable, param->data))){	// Pokud je token ID (čárka třeba nemá data) a existuje v tabulce funkcí
					parserSemanticStackPush(semanticError, 5, token->data, internalError, token->linePos, token->colPos);
				}

				if(param->type == T_COMMA){
					param = param->nextToken;
				}

				else if(param->type == T_ID || param->type == T_INTEGER || param->type == T_STRING || param->type == T_FLOAT || param->type == T_NIL){
					param = param->nextToken;
					params++;
				}
			}

			if(func_data->params == -1 && params >= 1){}	// Print může mít argumentů, kolik chce, pokud je to alespoň jeden

			else if(params != func_data->params){
				parserSemanticStackPush(semanticError, 6, token->data, internalError, token->linePos, token->colPos);
			}
		}

		else{
			parserSemanticStackPush(semanticError, 4, token->data, internalError, token->linePos, token->colPos);
		}
	}

	/*******************************Proměnná v expressionu**************************************************/

	else if(token->type == T_ID){	// Je-li to osamocené ID, někde v expressionu
		if(!inFunc){					// A pokud nejsme nikde ve funkci

			psData data = symTabSearch(varTable, token->data);

			if(data == NULL){
				parserSemanticStackPush(semanticError, 7, token->data, internalError, token->linePos, token->colPos);
			}
		}

		else{	// Pokud jsme ve funkci

			if((*func)->linePos == token->linePos){		// Pokud jsou to definice proměnných v hlavičce funkce
				symTabInsert(localTable, token->data, parserSemanticsInitData(VAR, NULL, 0));	// Zadefinujeme je do local rámce
			}

			else{
				psData data = symTabSearch(localTable, token->data);
				
				if((data == NULL) && (symTabSearch(funcTable, token->data) == NULL || symTabSearch(funcTable, token->data)->type != FUNC)){
					parserSemanticStackPush(semanticError, 7, token->data, internalError, token->linePos, token->colPos);
				}
			}
		}
	}


}

psData parserSemanticsInitData(sType type, struct sTree *localTable, int params){
	psData data = malloc(sizeof(struct sData));
	data->type = type;
	data->localFrame = &(*localTable);
	data->params = params;
	return data;
}

void parserSemanticsInFunc(bool *inFunc, int *inAux, pToken token){
	if(token->type == T_DEF){
		*inFunc = true;
	}

	else if(token->type == T_IF || token->type == T_WHILE){
		*inAux = *inAux + 1;
	}

	else if(token->type == T_END && !(*inAux)){
		*inFunc = false;
	}

	else if(token->type == T_END && *inAux){
		*inAux = *inAux - 1;
	}
}

int parserSemanticError(pSemanticsStack semanticError){
	int error = 0;

	for(int i = 0; i < semanticError->top; i++){
		if(semanticError->ErrorArray[i].error == 1){
			fprintf(stderr, "[SEMANTICS] Error: Attempted to redefine function %s!\n", semanticError->ErrorArray[i].name);
			if(!error) error = 3;
		}

		else if(semanticError->ErrorArray[i].error == 2){
			fprintf(stderr, "[SEMANTIC] Error: Variables (%s %u:%u) and functions must have different IDs!\n", semanticError->ErrorArray[i].name, semanticError->ErrorArray[i].line, semanticError->ErrorArray[i].col);
			if(!error) error = 3;
		}

		else if(semanticError->ErrorArray[i].error == 3){
			fprintf(stderr, "[SEMANTIC] Error: Can't name a variable (%s %u:%u) same as a previously defined function!\n", semanticError->ErrorArray[i].name, semanticError->ErrorArray[i].line, semanticError->ErrorArray[i].col);
			if(!error) error = 3;
		}

		else if(semanticError->ErrorArray[i].error == 4){
			fprintf(stderr, "[SEMANTIC] Error: Calling an undefined function %s on line %u:%u!\n", semanticError->ErrorArray[i].name, semanticError->ErrorArray[i].line, semanticError->ErrorArray[i].col);
			if(!error) error = 3;
		}

		else if(semanticError->ErrorArray[i].error == 5){
			fprintf(stderr, "[SEMANTIC] Error: Function %s (%u:%u) can't have another function as an argument!\n", semanticError->ErrorArray[i].name, semanticError->ErrorArray[i].line, semanticError->ErrorArray[i].col);
			if(!error) error = 6;
		}

		else if(semanticError->ErrorArray[i].error == 6){
			fprintf(stderr, "[SEMANTIC] Error: Wrong number of arguments in function %s on line %u:%u!\n", semanticError->ErrorArray[i].name, semanticError->ErrorArray[i].line, semanticError->ErrorArray[i].col);
			if(!error) error = 5;
		}

		else if(semanticError->ErrorArray[i].error == 7){
			fprintf(stderr, "[SEMANTIC] Error: Undefined variable %s on line %u:%u!\n", semanticError->ErrorArray[i].name, semanticError->ErrorArray[i].line, semanticError->ErrorArray[i].col);
			if(!error) error = 3;
		}

		else{
			fprintf(stderr, "[INTERNAL] Error: A seriously weird thing somehow happened!\n");
			if(!error) error = 99;
		}
	}
	
	return error;
}




/*******************************************************STACK*****************************************************************************/

void parserSemanticStackInit(pSemanticsStack *S, int *internalError){
	if(!S) return;
	if(!(*S = malloc(sizeof(struct SemanticsStack)))){
		if (!*internalError) *internalError = 2;
	}

	(*S)->ErrorArray = NULL;
	(*S)->top = 0;
}

void parserSemanticStackDelete(pSemanticsStack *S){
	free((*S)->ErrorArray);
	free(*S);
}

void parserSemanticStackPush(pSemanticsStack S, int error, char *name, int *internalError, int line, int col){
	if(!(S->ErrorArray = realloc(S->ErrorArray, sizeof(struct Error) * (S->top + 1)))){
		if (!*internalError) *internalError = 2;
	}

	else{
		S->ErrorArray[S->top].error = error;
		S->ErrorArray[S->top].name = name;
		S->ErrorArray[S->top].line = line;
		S->ErrorArray[S->top].col = col;
		S->top++;
	}
}