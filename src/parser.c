/**
 * @file parser.c
 * 
 * Syntaktický analyzátor
 * 
 * IFJ Projekt 2018, Tým 13
 * 
 * @author <xforma04> Klára Formánková
 * @author <xlanco00> Jan Láncoš
 * @author <xsebel04> Vít Šebela
 * @author <xchalo16> Jan Chaloupka
 */

#include "parser.h"
#include "expressions.h"

int parser(pToken *List){

	pToken token = (*List);		// List pro průchod syntaxe
	pToken preRun = (*List);	// List pro pre-run
	
	int error = 0;				// Chyba vstupního kódu
	int internalError = 0;		// Interní chyba překladače

	psTree funcTable;			// Hlavní tabulka definovaných funkcí
	psTree varTable;			// Hlavní tabulka proměnných
	psTree localTable = NULL;	// Lokální proměnné
	pToken func = NULL;			// ... této funkce (func->data)

	symTabInit(&funcTable);
	symTabInit(&varTable);

	parserSemanticsInitBuiltIn(&funcTable);	// Naplnění tabulky built-in funkcema

	pSyntaxStack S;							// Stack pro rekurzivní sestup

	parserSyntaxStackInit(&S, &internalError);

	bool inFunc = false;	// Je-li true, jsme ve funkci
	int inAux = 0;			// Semafor - za každý if/while ++, za každý END --

	while(preRun != NULL){	// Sémantický pre-run, naplnění tabulky definicemi funkcí
		parserSemanticsPreRun(&preRun, &funcTable, &error);	// Naplnění tabulky definicí funkcí
		error = parserError(error, internalError, &preRun);

		if(error){

			// Úklid

			symTabDispose(&varTable);
			symTabDispose(&funcTable);
			parserSyntaxStackDelete(&S);
			return error;
		}

		preRun = preRun->nextToken;
	}

	while(token != NULL){	// Syntaktická analýza + Sémantická analýza

		pToken prevToken = token;

		if(S->a[S->last] > T_STRING){
			if(!inFunc) localTable = varTable;
			parserSyntaxExpand(S, &token, &error, &internalError, localTable);	// Je-li na stacku neterminál
		}

		else{
			parserSyntaxCompare(S, token, &error);	// Je-li na stacku s čím porovnávat
			parserSyntaxStackPop(S, &internalError);
			parserSyntaxIDFNCheck(token, &funcTable, &error);	// Kontrola ? a ! na konci proměnných (IDs)

			parserSemanticsInFunc(&inFunc, &inAux, token);	// Jsme-li ve funkci - tj. mezi DEF a příslušným END
			parserSemanticsCheck(token, &func, &funcTable, &varTable, &localTable, &error, inFunc); // Sémantická analýza (IDs)

			token = token->nextToken;
		}

		// Volání Klarušina generování kódu
		codeFromToken(S->a[S->last], token, localTable);

		error = parserError(error, internalError, &prevToken);

		if(error){
			
			// Úklid

			symTabDispose(&varTable);
			symTabDispose(&funcTable);
			parserSyntaxStackDelete(&S);
			return error;
		} 
	}

	// Úklid

	symTabDispose(&varTable);
	symTabDispose(&funcTable);
	parserSyntaxStackDelete(&S);
	return error;
}

int parserError(int error, int internalError, pToken *prevToken){
	if(internalError == 1){
		fprintf(stderr, "[INTERNAL] Fatal error - Unexpected token on stack\n");
		return 99;
	}

	else if(internalError == 2){
		fprintf(stderr, "[INTERNAL] Fatal error - Failed malloc\n");
		return 99;
	}

	else if(internalError == 3){
		fprintf(stderr, "[INTERNAL] Fatal error - Stack underflow\n");
		return 99;
	} 

	else if(internalError != 0 && internalError != 1 && internalError != 2 && internalError != 3){
		fprintf(stderr, "[INTERNAL] Fatal error - Something went terribly wrong\n");
		return 99;
	}

	if(error){
			if(error == 2){	// Syntax
				fprintf(stderr, "[SYNTAX] Error on line %u:%u\n", (*prevToken)->linePos, (*prevToken)->colPos);
				return 2;
			}

			else if(error == 11){	// Sémantika
				fprintf(stderr, "[SEMANTIC] Error on line %u:%u - Attempted to redefine function\n", (*prevToken)->linePos, (*prevToken)->colPos);
				return 3;
			}

			else if(error == 12){	// Sémantika
				fprintf(stderr, "[SEMANTIC] Error on line %u:%u - Variables and functions must have different IDs\n", (*prevToken)->linePos, (*prevToken)->colPos);
				return 3;
			}

			else if(error == 13){	// Sémantika
				fprintf(stderr, "[SEMANTIC] Error on line %u:%u - Can't name a variable same as a previously defined function\n", (*prevToken)->linePos, (*prevToken)->colPos);
				return 3;
			}

			else if(error == 14){	// Sémantika
				fprintf(stderr, "[SEMANTIC] Error on line %u:%u - Calling an undefined function\n", (*prevToken)->linePos, (*prevToken)->colPos);
				return 3;
			}

			else if(error == 15){	// Sémantika
				fprintf(stderr, "[SEMANTIC] Error on line %u:%u - Function can't have another function as an argument\n", (*prevToken)->linePos, (*prevToken)->colPos);
				return 6;
			}

			else if(error == 16){	// Sémantika
				fprintf(stderr, "[SEMANTIC] Error on line %u:%u - Wrong number of arguments in a function\n", (*prevToken)->linePos, (*prevToken)->colPos);
				return 5;
			}

			else if(error == 17){	// Sémantika
				fprintf(stderr, "[SEMANTIC] Error on line %u:%u - Undefined variable\n", (*prevToken)->linePos, (*prevToken)->colPos);
				return 3;
			}

			else if(error == 18){	// Sémantika
				fprintf(stderr, "[SEMANTIC] Error on line %u:%u - Can't have arguments of the same name in a function\n", (*prevToken)->linePos, (*prevToken)->colPos);
				return 6;
			}

			else if(error == 200){	//Error expressionu
				return 2;
			}

			else if(error == 300){	//Error expressionu
				return 3;
			}

			else if(error == 400){	//Error expressionu
				return 4;
			}

			else if(error == 69){	// Syntax
				fprintf(stderr, "[SYNTAX] Error - Only function names can end with \"!\" and \"?\" symbols!\n");
				return 2;
			}

			else if(error == 42){	// Syntax
				fprintf(stderr, "[SYNTAX] Error - Only variables can be used when defining parameters of a function!\n");
				return 2;
			}

			else{	// Nedefinovanej stav, nemělo by nikdy nastat
				fprintf(stderr, "[INTERNAL] Fatal error - Unexplainable unbelievable problem!\n");
				return 99;
			}
		}

	return 0;
}




/******************************************************SYNTAX******************************************************************************/

void parserSyntaxCompare(pSyntaxStack S, pToken token, int *error){
	if (S->a[S->last] == token->type);	// Jsou-li stejné, všecko ok
	else if(!*error) *error = 2;		// Jinak error
}

void parserSyntaxExpand(pSyntaxStack S, pToken *token, int *error, int *internalError, psTree localTable){
	if(S->a[S->last] == N_PROG){	// Konečný automat podle LL(1) gramatiky
		if((*token)->type == T_DEF){
			parserSyntaxStackPop(S, internalError);
			parserSyntaxStackPush(S, N_PROG, internalError);
			parserSyntaxStackPush(S, T_EOL, internalError);
			parserSyntaxStackPush(S, N_DEFUNC, internalError);
		}

		else if((*token)->type == T_ID ||
		(*token)->type == T_NIL ||
		(*token)->type == T_INTEGER ||
		(*token)->type == T_FLOAT ||
		(*token)->type == T_STRING ||
		(*token)->type == T_NOT ||
		(*token)->type == T_TRUE ||
		(*token)->type == T_FALSE ||
		(*token)->type == T_ADD ||
		(*token)->type == T_SUB ||
		(*token)->type == T_LBRCKT ||
		(*token)->type == T_IF ||
		(*token)->type == T_WHILE ||
		(*token)->type == T_EOL){
			parserSyntaxStackPop(S, internalError);
			parserSyntaxStackPush(S, N_PROG, internalError);
			parserSyntaxStackPush(S, N_BODY, internalError);
		}


		else if((*token)->type == T_EOF){
			parserSyntaxStackPop(S, internalError);
			parserSyntaxStackPush(S, T_EOF, internalError);
		}
		
		else if(!*error) *error = 2;
	}

	else if(S->a[S->last] == N_BODY){
		parserSyntaxStackPop(S, internalError);

		if((*token)->type == T_ID){
			parserSyntaxStackPush(S, N_BODY, internalError);
			parserSyntaxStackPush(S, T_EOL, internalError);
			parserSyntaxStackPush(S, N_BODY_ID, internalError);
			parserSyntaxStackPush(S, T_ID, internalError);
		}

		else if((*token)->type == T_NIL ||
		(*token)->type == T_INTEGER ||
		(*token)->type == T_FLOAT ||
		(*token)->type == T_STRING ||
		(*token)->type == T_TRUE ||
		(*token)->type == T_FALSE ||
		(*token)->type == T_ADD ||
		(*token)->type == T_SUB ||
		(*token)->type == T_NOT ||
		(*token)->type == T_LBRCKT){
			parserSyntaxStackPush(S, N_BODY, internalError);
			parserSyntaxStackPush(S, T_EOL, internalError);
			parserSyntaxStackPush(S, N_EXPR, internalError);
		}

		else if((*token)->type == T_IF){
			parserSyntaxStackPush(S, N_BODY, internalError);
			parserSyntaxStackPush(S, T_EOL, internalError);
			parserSyntaxStackPush(S, N_IF, internalError);
		}

		else if((*token)->type == T_WHILE){
			parserSyntaxStackPush(S, N_BODY, internalError);
			parserSyntaxStackPush(S, T_EOL, internalError);
			parserSyntaxStackPush(S, N_WHILE, internalError);
		}

		else if((*token)->type == T_EOL){
			parserSyntaxStackPush(S, N_BODY, internalError);
			parserSyntaxStackPush(S, T_EOL, internalError);
		}
	}

	else if(S->a[S->last] == N_BODY_ID){
		if((*token)->type == T_ADD ||
		(*token)->type == T_SUB ||
		(*token)->type == T_MUL ||
		(*token)->type == T_DIV ||
		(*token)->type == T_EQL ||
		(*token)->type == T_NEQ ||
		(*token)->type == T_GT ||
		(*token)->type == T_LT ||
		(*token)->type == T_GTE ||
		(*token)->type == T_LTE){
			parserSyntaxStackPop(S, internalError);
			parserSyntaxStackPush(S, N_EXPR_O, internalError);
		}

		else if((*token)->type == T_ASSIGN){
			parserSyntaxStackPop(S, internalError);
			parserSyntaxStackPush(S, N_DEFVAR, internalError);
			parserSyntaxStackPush(S, T_ASSIGN, internalError);
		}

		else if((*token)->type == T_ID ||
		(*token)->type == T_NIL ||
		(*token)->type == T_INTEGER ||
		(*token)->type == T_FLOAT ||
		(*token)->type == T_STRING ||
		(*token)->type == T_TRUE ||
		(*token)->type == T_FALSE ||
		(*token)->type == T_LBRCKT){
			parserSyntaxStackPop(S, internalError);
			parserSyntaxStackPush(S, N_FUNC, internalError);
		}

		else if((*token)->type == T_EOL){
			parserSyntaxStackPop(S, internalError);
		}

		else if(!*error) *error = 2;
	}

	else if(S->a[S->last] == N_TYPE){
		if((*token)->type == T_NIL){
			parserSyntaxStackPop(S, internalError);
			parserSyntaxStackPush(S, T_NIL, internalError);	
		}

		else if((*token)->type == T_INTEGER){
			parserSyntaxStackPop(S, internalError);
			parserSyntaxStackPush(S, T_INTEGER, internalError);	
		}

		else if((*token)->type == T_FLOAT){
			parserSyntaxStackPop(S, internalError);
			parserSyntaxStackPush(S, T_FLOAT, internalError);	
		}

		else if((*token)->type == T_STRING){
			parserSyntaxStackPop(S, internalError);
			parserSyntaxStackPush(S, T_STRING, internalError);	
		}

		else if((*token)->type == T_TRUE){
			parserSyntaxStackPop(S, internalError);
			parserSyntaxStackPush(S, T_TRUE, internalError);	
		}

		else if((*token)->type == T_FALSE){
			parserSyntaxStackPop(S, internalError);
			parserSyntaxStackPush(S, T_FALSE, internalError);	
		}

		else if(!*error) *error = 2;
	}

	else if(S->a[S->last] == N_TYPE_ID){

		if((*token)->type == T_NIL ||
		(*token)->type == T_INTEGER ||
		(*token)->type == T_FLOAT ||
		(*token)->type == T_STRING ||
		(*token)->type == T_TRUE ||
		(*token)->type == T_FALSE){
			parserSyntaxStackPop(S, internalError);
			parserSyntaxStackPush(S, N_TYPE, internalError);
		}

		else if((*token)->type == T_ID){
			parserSyntaxStackPop(S, internalError);
			parserSyntaxStackPush(S, T_ID, internalError);
		}

		else if(!*error) *error = 2;
	}

	else if(S->a[S->last] == N_DEFUNC){
		if((*token)->type == T_DEF){
				parserSyntaxStackPop(S, internalError);
				parserSyntaxStackPush(S, T_END, internalError);
				parserSyntaxStackPush(S, N_BODY, internalError);
				parserSyntaxStackPush(S, T_EOL, internalError);
				parserSyntaxStackPush(S, T_RBRCKT, internalError);
				parserSyntaxStackPush(S, N_PARS, internalError);
				parserSyntaxStackPush(S, T_LBRCKT, internalError);
				parserSyntaxStackPush(S, T_ID, internalError);
				parserSyntaxStackPush(S, T_DEF, internalError);
		}

		else if(!*error) *error = 2;
	}

	else if(S->a[S->last] == N_FUNC){
		parserSyntaxStackPop(S, internalError);
		
		if ((*token)->type == T_LBRCKT){
			parserSyntaxStackPush(S, T_RBRCKT, internalError);
			parserSyntaxStackPush(S, N_PARS, internalError);
			parserSyntaxStackPush(S, T_LBRCKT, internalError);
		}

		else if((*token)->type == T_ID ||
		(*token)->type == T_NIL ||
		(*token)->type == T_INTEGER ||
		(*token)->type == T_FLOAT ||
		(*token)->type == T_STRING ||
		(*token)->type == T_TRUE ||
		(*token)->type == T_FALSE){
			parserSyntaxStackPush(S, N_PARS, internalError);
		}
	}

	else if(S->a[S->last] == N_PARS){
		parserSyntaxStackPop(S, internalError);

		if((*token)->type == T_NIL ||
		(*token)->type == T_INTEGER ||
		(*token)->type == T_FLOAT ||
		(*token)->type == T_STRING ||
		(*token)->type == T_TRUE ||
		(*token)->type == T_FALSE){
			parserSyntaxStackPush(S, N_PARSN, internalError);
			parserSyntaxStackPush(S, N_TYPE, internalError);
		}

		else if((*token)->type == T_ID){
			parserSyntaxStackPush(S, N_PARSN, internalError);
			parserSyntaxStackPush(S, T_ID, internalError);
		}
	}

	else if(S->a[S->last] == N_PARSN){
		parserSyntaxStackPop(S, internalError);

		if((*token)->type == T_COMMA){
			parserSyntaxStackPush(S, N_PARSN, internalError);
			parserSyntaxStackPush(S, N_TYPE_ID, internalError);
			parserSyntaxStackPush(S, T_COMMA, internalError);
		}
	}

	else if(S->a[S->last] == N_IF){
		if((*token)->type == T_IF){
			parserSyntaxStackPop(S, internalError);
			parserSyntaxStackPush(S, T_END, internalError);
			parserSyntaxStackPush(S, N_BODY, internalError);
			parserSyntaxStackPush(S, T_EOL, internalError);
			parserSyntaxStackPush(S, T_ELSE, internalError);
			parserSyntaxStackPush(S, N_BODY, internalError);
			parserSyntaxStackPush(S, T_EOL, internalError);
			parserSyntaxStackPush(S, T_THEN, internalError);
			parserSyntaxStackPush(S, N_EXPR, internalError);
			parserSyntaxStackPush(S, T_IF, internalError);
		}

		else if(!*error) *error = 2;
	}

	else if(S->a[S->last] == N_WHILE){
		if((*token)->type == T_WHILE){
			parserSyntaxStackPop(S, internalError);
			parserSyntaxStackPush(S, T_END, internalError);
			parserSyntaxStackPush(S, N_BODY, internalError);
			parserSyntaxStackPush(S, T_EOL, internalError);
			parserSyntaxStackPush(S, T_DO, internalError);
			parserSyntaxStackPush(S, N_EXPR, internalError);
			parserSyntaxStackPush(S, T_WHILE, internalError);
		}

		else if(!*error) *error = 2;
	}

	else if(S->a[S->last] == N_DEFVAR){
		if((*token)->type == T_NIL ||
		(*token)->type == T_INTEGER ||
		(*token)->type == T_FLOAT ||
		(*token)->type == T_STRING ||
		(*token)->type == T_TRUE ||
		(*token)->type == T_FALSE ||
		(*token)->type == T_ADD ||
		(*token)->type == T_SUB ||
		(*token)->type == T_NOT ||
		(*token)->type == T_LBRCKT){
			parserSyntaxStackPop(S, internalError);
			parserSyntaxStackPush(S, N_EXPR, internalError);
		}

		else if((*token)->type == T_ID){
			parserSyntaxStackPop(S, internalError);
			parserSyntaxStackPush(S, N_DEFVARID, internalError);
			parserSyntaxStackPush(S, T_ID, internalError);
		}


		else if(!*error) *error = 2;
	}

	else if(S->a[S->last] == N_DEFVARID){
		parserSyntaxStackPop(S, internalError);
		if((*token)->type == T_ADD ||
		(*token)->type == T_SUB ||
		(*token)->type == T_DIV ||
		(*token)->type == T_MUL ||
		(*token)->type == T_EQL ||
		(*token)->type == T_NEQ ||
		(*token)->type == T_LT ||
		(*token)->type == T_GT ||
		(*token)->type == T_LTE ||
		(*token)->type == T_GTE){
			parserSyntaxStackPush(S, N_EXPR_O, internalError);
		}

		else if((*token)->type == T_LBRCKT ||
		(*token)->type == T_ID ||
		(*token)->type == T_NIL ||
		(*token)->type == T_INTEGER ||
		(*token)->type == T_FLOAT ||
		(*token)->type == T_STRING ||
		(*token)->type == T_TRUE ||
		(*token)->type == T_FALSE){
			parserSyntaxStackPush(S, N_FUNC, internalError);
		}

	} 

	else if(S->a[S->last] == N_EXPR_O){
		*token = (*token)->prevToken;
		*error = exprParse(&(*token), localTable);	// Volání externí funkce ke zpracování výrazů
		if(error) 
			*error = *error * 100;	// Pokud nula, stále nula, jinak 200, 300, 400

		if((*token)->type == T_EOL ||
		(*token)->type == T_EOF){
			parserSyntaxStackPop(S, internalError);
		}

		else if(!*error) *error = 2;
	}

	else if(S->a[S->last] == N_EXPR){
		*error = exprParse(&(*token), localTable);	// Volání externí funkce ke zpracování výrazů
		if(error) 
			*error = *error * 100;	// Pokud nula, stále nula, jinak 200, 300, 400

		if((*token)->type == T_EOL ||
		(*token)->type == T_THEN ||
		(*token)->type == T_DO ||
		(*token)->type == T_EOF){
			parserSyntaxStackPop(S, internalError);
		}

		else if(!*error) *error = 2;
	}

	else{
		if (!*internalError) *internalError = 2;	// Neočekávaný token na stacku
	}
}

void parserSyntaxIDFNCheck(pToken token, psTree *funcTable, int *error){
	if(token->type == T_ID && (token->data[strlen(token->data) - 1] == '!' || token->data[strlen(token->data) - 1] == '?'))
		if(!symTabSearch(funcTable, token->data))
			if(!*error) *error = 69;
}




/*******************************************************STACK*****************************************************************************/

void parserSyntaxStackInit(pSyntaxStack *S, int *internalError){
	if(!(*S = malloc(sizeof(struct SyntaxStack)))){
		if (!*internalError) *internalError = 2;
		return;
	}

	(*S)->top = 1;
	(*S)->last = 0;

	if(!((*S)->a = malloc(STACK_CHUNK_SIZE * sizeof(tType)))){
		if (!*internalError) *internalError = 2;
		return;
	}

	(*S)->size = STACK_CHUNK_SIZE;
	(*S)->a[0] = N_PROG; 				// Výchozí neterminál je <prog>
}

void parserSyntaxStackDelete(pSyntaxStack *S){
	free((*S)->a);
	free(*S);
	*S = NULL;
}

void parserSyntaxStackPush(pSyntaxStack S, tType type, int *internalError){
	if (S->top == S->size){
		S->size += STACK_CHUNK_SIZE;
		S->a = realloc(S->a, S->size * sizeof(tType));	// Dynamická realokace zásobníku v případě přetečení
		if(S->a == NULL){
			if (!*internalError) *internalError = 2;
			return;
		}
	}
	
	S->a[S->top] = type;
	S->top++;
	S->last++;
}

void parserSyntaxStackPop(pSyntaxStack S, int *internalError){
	if (S->top==0) {
		if (!*internalError) *internalError = 3;	// Ošetření podtečení
		return;
	}	
	else {
		S->last--;
		S->top--; 
	}	
}




/*****************************************************SÉMANTIKA***************************************************************************/

void parserSemanticsInitBuiltIn(psTree *funcTable){
	symTabInsert(funcTable, "print", parserSemanticsInitData(FUNC, NULL, -1, true));
	symTabInsert(funcTable, "inputs", parserSemanticsInitData(FUNC, NULL, 0, true));
	symTabInsert(funcTable, "inputi", parserSemanticsInitData(FUNC, NULL, 0, true));
	symTabInsert(funcTable, "inputf", parserSemanticsInitData(FUNC, NULL, 0, true));
	symTabInsert(funcTable, "length", parserSemanticsInitData(FUNC, NULL, 1, true));
	symTabInsert(funcTable, "substr", parserSemanticsInitData(FUNC, NULL, 3, true));
	symTabInsert(funcTable, "ord", parserSemanticsInitData(FUNC, NULL, 2, true));
	symTabInsert(funcTable, "chr", parserSemanticsInitData(FUNC, NULL, 1, true));
	return;
}

void parserSemanticsPreRun(pToken *token, psTree *funcTable, int *error){
	if((*token)->type == T_ID){

		if((*token)->prevToken != NULL && (*token)->prevToken->type == T_DEF){	// Jde-li o definici funkce
			
			if(!(symTabSearch(funcTable, (*token)->data))){	// A nejde-li o redefinici (jinak error)
				psTree localTable;
				symTabInit(&localTable);	// Zadefinujeme si lokální rámec
				psData data = parserSemanticsInitData(FUNC, localTable, 0, true);
				pToken param;
				if((*token)->nextToken != NULL) param = (*token)->nextToken->nextToken;

				while(param != NULL){	// Spočítání parametrů k pozdějšímu porovnání při volání
					if(param->type == T_COMMA){
						param = param->nextToken;
					}

					else if(param->type == T_ID){
						symTabInsert(&localTable, param->data, parserSemanticsInitData(VAR, NULL, 0, false));
						param = param->nextToken;
						data->params++;
					}

					else if(param->type == T_STRING || param->type == T_FLOAT || param->type == T_INTEGER || 
					param->type == T_NIL || param->type == T_TRUE || param->type == T_FALSE){ 	// Syntaktická kontrola použití
						if (!*error) *error = 42;												// typu místo ID v parametrech při definici
						break;
					}

					else break;
				}

				symTabInsert(funcTable, (*token)->data, data);
				symTabSearch(funcTable, (*token)->data)->localFrame = localTable;	// Napojíme lokální rámec k funkci v tabulce funkcí
			}

			else{
				if (!*error) *error = 11;
			}
		}
	}
}

void parserSemanticsCheck(pToken token, pToken *func, psTree *funcTable, psTree *varTable, psTree *localTable, int *error, bool inFunc){
	
	/*******************************Local frame***********************************************************/

	if(inFunc && token->type == T_DEF){	// Nacházíme se ve funkci, tedy zřídíme localTable a zapamatujem si token s názvem funkce
		*localTable = symTabSearch(funcTable, token->nextToken->data)->localFrame; // Najdem localTable v tabulce fukcí
		*func = token->nextToken;	// Identifikátor funkce po DEF
	}




	/*******************************Definice proměnné*****************************************************/

	if(token->type == T_ID && token->nextToken->type == T_ASSIGN){	// Je-li to definice proměnné
		if(!inFunc){					// A pokud nejsme nikde ve funkci
			if(symTabSearch(funcTable, token->data)){	// Pokud je identifikátor už zabrán jakožto název funkce
				if (!*error) *error = 12;
			}

			if(symTabSearch(varTable, token->data)){	// Pokud už existuje v příslušné tabulce, aktualizujeme bool defined
				symTabInsert(varTable, token->data, parserSemanticsInitData(VAR, NULL, 0, true));
			}

			else symTabInsert(varTable, token->data, parserSemanticsInitData(VAR, NULL, 0, false));	// Pokud ne, definujeme
		}

		else{							// Pokud jsme ve funkci
			if(symTabSearch(funcTable, token->data)){
				if (!*error) *error = 13;
			}

			if(symTabSearch(localTable, token->data)){	// Pokud už existuje v příslušné tabulce, aktualizujeme bool defined
				symTabInsert(localTable, token->data, parserSemanticsInitData(VAR, NULL, 0, true));
			}

			else symTabInsert(localTable, token->data, parserSemanticsInitData(VAR, NULL, 0, false));	// Pokud ne, definujeme
		}
	}




	/*******************************Volání funkce**********************************************************/

	else if((token->type == T_ID && symTabSearch(funcTable, token->data)) || 
	((token->prevToken != NULL && token->nextToken != NULL) &&
	(token->type == T_ID && token->prevToken->type != T_DEF && (token->nextToken->type == T_LBRCKT || 
	token->nextToken->type == T_ID || token->nextToken->type == T_FLOAT || token->nextToken->type == T_STRING || 
	token->nextToken->type == T_INTEGER || token->nextToken->type == T_NIL)))){

		if(symTabSearch(funcTable, token->data)){				// Pokud je definovaná
			
			psData func_data = symTabSearch(funcTable, token->data);	// Uložit si data o funkci z tabulky (kvůli počtu parametrů)
			pToken param = NULL;

			if(token->nextToken->type == T_LBRCKT) param = token->nextToken->nextToken; // Dostat se k prvnímu parametru
			else param = token->nextToken;

			int params = 0;

			while(param->type != T_EOL && param->type != T_RBRCKT){			// Spočítáme parametry

				if(param->data != NULL && (symTabSearch(funcTable, param->data))){	// Pokud je token ID (čárka třeba nemá data) a existuje v tabulce funkcí
					if (!*error) *error = 15;
					break;
				}

				if(param->type == T_COMMA){
					param = param->nextToken;
				}

				else if(param->type == T_ID || param->type == T_INTEGER || param->type == T_STRING || param->type == T_FLOAT || param->type == T_NIL){
					param = param->nextToken;
					params++;
				}

				else{
					break;
				}
			}

			if(func_data->params == -1 && params >= 1){}	// Print může mít argumentů, kolik chce, pokud je to alespoň jeden

			else if(params != func_data->params){
				if (!*error) *error = 16;
			}
		}

		else{
			if((*func != NULL) && token != NULL && (*func)->linePos != token->linePos){ // V hlavičce funkce má přednost Syntax error
				if (!*error) *error = 14;
			}
		}
	}


	

	/*******************************Proměnná v expressionu**************************************************/

	else if(token->type == T_ID){	// Je-li to osamocené ID, někde v expressionu
		if(!inFunc){					// A pokud nejsme nikde ve funkci

			psData data = symTabSearch(varTable, token->data);

			if(data == NULL){
				if (!*error) *error = 17;
			}
		}

		else{	// Pokud jsme ve funkci

			if((*func)->linePos == token->linePos){		// Pokud jsou to definice proměnných v hlavičce funkce
				symTabInsert(localTable, token->data, parserSemanticsInitData(VAR, NULL, 0, false));	// Zadefinujeme je do local rámce

				pToken aux = token->prevToken;
				while(aux->type != T_LBRCKT){	// Zkontroluju předchozí, jestli se náhodou nevyskytujou duplicity
					if(aux->type == T_COMMA) aux = aux->prevToken;
					else{
						if(!strcmp(token->data, aux->data)) 
							if (!*error) *error = 18;

						aux = aux->prevToken;
					}
				}
			}

			else{
				psData data = symTabSearch(localTable, token->data);
				
				if(data == NULL){
					if (!*error) *error = 17;
				}
			}
		}
	}
}

psData parserSemanticsInitData(sType type, struct sTree *localTable, int params, bool defined){
	psData data = malloc(sizeof(struct sData));
	data->type = type;
	data->localFrame = &(*localTable);
	data->params = params;
	data->defined = defined;
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