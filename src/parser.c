#include "parser.h"
#include "expressions.h"

int parser(pToken *List){

	pToken token = (*List);			// List pro průchod syntaxe
	pToken semanToken = (*List);	// List pro průchod sémantiky
	int error = 0;				// Chyba vstupního kódu
	int int_error = 0;			// Interní chyba překladače
	tStack S;					// Stack pro rekurzivní sestup
	psTree func_table;			// Hlavní tabulka definovaných funkcí
	psTree var_table;			// Hlavní tabulka proměnných
	symTabInit(&func_table);
	symTabInit(&var_table);
	parserSemanticsInit(&func_table);	// Naplnění tabulky built-in funkcema
	int_error = parserStackInit(&S);

	while(token != NULL){	// Syntaktická analýza + sémantické definice funkcí

		pToken prevToken = token;

		if(S.a[S.last] > T_STRING){
			int_error = parserExpand(&S, &token, &error, &int_error);	// Je-li na stacku neterminál
		}

		else{
			if(!error) error = (parserCompare(S, token));	// Je-li na stacku s čím porovnávat
			if(!error) error = (parserSemanticsPreRun(&token, &func_table));	// Naplnění tabulky definicí funkcí
			parserStackPop(&S, &int_error);
			token = token->nextToken;
		}

		error = parserError(error, int_error, &prevToken, &S);
		if(error) return error;
	}


	bool inFunc = false;	// Je-li true, jsme ve funkci
	int inAux = 0;			// Semafor? Za každý if/while ++, za každý END --

	while(semanToken != NULL){		// Sémantická analýza

		pToken prevToken = semanToken;
		pToken func;
		psTree local_frame;

		parserSemanticsInFunc(&inFunc, &inAux, semanToken);

		if(inFunc && semanToken->type == T_DEF){	
			local_frame = symTabSearch(&func_table, semanToken->nextToken->data)->localFrame; // Najdem local_frame v tabulce fukcí (definice již proběhly)
			func = semanToken->nextToken;	// Identifikátor funkce po DEF
		}

		if(semanToken->type == T_ID && semanToken->nextToken->type == T_ASSIGN){	// Je-li to definice proměnné
			if(!inFunc){					// A pokud nejsme nikde ve funkci
				if(symTabSearch(&func_table, semanToken->data)){
					printf("[SEMANTIC] Error: Variables and functions must have different IDs!\n");
					error = 3;
				}
				symTabInsert(&var_table, semanToken->data, parserSemanticsInitData(VAR, NULL, 0));
			}

			else{							// Pokud jsme ve funkci
				if(symTabSearch(&func_table, semanToken->data)){
					printf("[SEMANTIC] Error: Can't name a variable same as a previously defined function!\n");
					error = 3;
				}
				symTabInsert(&local_frame, semanToken->data, parserSemanticsInitData(VAR, NULL, 0));
			}
		}

		// Nechutně složitá podmínka, ve zkratce znamená "Pokud voláme funkci", zrefaktoruju
		else if((semanToken->type == T_ID && symTabSearch(&func_table, semanToken->data)) || (semanToken->type == T_ID && semanToken->prevToken->type != T_DEF && (semanToken->nextToken->type == T_LBRCKT || semanToken->nextToken->type == T_ID || semanToken->nextToken->type == T_FLOAT || semanToken->nextToken->type == T_STRING || semanToken->nextToken->type == T_INTEGER || semanToken->nextToken->type == T_NIL))){
			if(symTabSearch(&func_table, semanToken->data)){	// Pokud je definovaná
				
				psData func_data = symTabSearch(&func_table, semanToken->data);
				pToken param = NULL;
				if(semanToken->nextToken->type == T_LBRCKT) param = semanToken->nextToken->nextToken;
				else param = semanToken->nextToken;
				int params = 0;

				while(param->type != T_EOL && param->type != T_RBRCKT){	// Spočítáme parametry

					if(param->data != NULL && (symTabSearch(&func_table, param->data))){
						printf("[SEMANTIC] Error: Function %s can't have another function as an argument!\n", semanToken->data);
						error = 3;
						break;
					}

					if(param->type == T_COMMA){
						param = param->nextToken;
					}

					else if(param->type == T_ID || param->type == T_INTEGER || param->type == T_STRING || param->type == T_FLOAT || param->type == T_NIL){
						param = param->nextToken;
						params++;
					}
				}
				if(func_data->params == -1 && params >= 1){}

				else if(params != func_data->params && error != 3){
					printf("[SEMANTIC] Error: Wrong number of arguments in function %s!\n", semanToken->data);
					error = 3;
				}
			}

			else{
				printf("[SEMANTIC] Error: Calling an undefined function %s on line %u:%u!\n", semanToken->data, semanToken->linePos, semanToken->linePos);
				error = 3;
			}
		}

		else if(semanToken->type == T_ID){	// Je-li to osamocené ID, někde v expressionu
			if(!inFunc){					// A pokud nejsme nikde ve funkci
				psData data = symTabSearch(&var_table, semanToken->data);

				if(data == NULL){
					printf("[SEMANTIC] Error: Undefined variable %s on line %u:%u!\n", semanToken->data, semanToken->linePos, semanToken->linePos);
					error = 3;
				}
			}

			else{	// Pokud jsme ve funkci

				if(func->linePos == semanToken->linePos){	// Pokud jsou to definice proměnných v hlavičce funkce
					symTabInsert(&local_frame, semanToken->data, parserSemanticsInitData(VAR, NULL, 0));
				}

				else{
					psData data = symTabSearch(&local_frame, semanToken->data);
					
					if((data == NULL) && (symTabSearch(&func_table, semanToken->data) == NULL || symTabSearch(&func_table, semanToken->data)->type != FUNC)){
						printf("[SEMANTIC] Error: Undefined variable %s on line %u:%u!\n", semanToken->data, semanToken->linePos, semanToken->linePos);
						error = 3;
					}
				}
			}
		}

		error = parserError(error, int_error, &prevToken, &S);
		if(error) return error;
		semanToken = semanToken->nextToken;
	}

	symTabDispose(&var_table);
	symTabDispose(&func_table);
	parserStackDelete(&S);
	return 0;
}

int parserCompare(tStack S, pToken token){
	if (S.a[S.last] == token->type) return 0;
	else return 2;
}

int parserExpand(tStack *S, pToken *token, int *error, int *int_error){
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
			parserStackPop(&(*S), int_error);
			parserStackPush(&(*S), N_PROG, int_error);
			parserStackPush(&(*S), N_BODY, int_error);
		}

		else if((*token)->type == T_DEF){
			parserStackPop(&(*S), int_error);
			parserStackPush(&(*S), N_PROG, int_error);
			parserStackPush(&(*S), T_EOL, int_error);
			parserStackPush(&(*S), N_DEFUNC, int_error);
		}

		else if((*token)->type == T_EOF){
			parserStackPop(&(*S), int_error);
			parserStackPush(&(*S), T_EOF, int_error);
		}
		
		else *error = 2;
	}

	else if(S->a[S->last] == N_BODY){
		parserStackPop(&(*S), int_error);

		if((*token)->type == T_ID){
			parserStackPush(&(*S), N_BODY, int_error);
			parserStackPush(&(*S), T_EOL, int_error);
			parserStackPush(&(*S), N_BODY_ID, int_error);
			parserStackPush(&(*S), T_ID, int_error);
		}

		else if((*token)->type == T_NIL ||
		(*token)->type == T_INTEGER ||
		(*token)->type == T_STRING ||
		(*token)->type == T_FLOAT ||
		(*token)->type == T_NOT ||
		(*token)->type == T_LBRCKT){
			parserStackPush(&(*S), N_BODY, int_error);
			parserStackPush(&(*S), T_EOL, int_error);
			parserStackPush(&(*S), N_EXPR, int_error);
		}

		else if((*token)->type == T_IF){
			parserStackPush(&(*S), N_BODY, int_error);
			parserStackPush(&(*S), T_EOL, int_error);
			parserStackPush(&(*S), N_IF, int_error);
		}

		else if((*token)->type == T_WHILE){
			parserStackPush(&(*S), N_BODY, int_error);
			parserStackPush(&(*S), T_EOL, int_error);
			parserStackPush(&(*S), N_WHILE, int_error);
		}

		else if((*token)->type == T_EOL){
			parserStackPush(&(*S), N_BODY, int_error);
			parserStackPush(&(*S), T_EOL, int_error);
		}
	}

	else if(S->a[S->last] == N_BODY_ID){
		if((*token)->type == T_ID ||
		(*token)->type == T_NIL ||
		(*token)->type == T_INTEGER ||
		(*token)->type == T_STRING ||
		(*token)->type == T_FLOAT ||
		(*token)->type == T_LBRCKT){
			parserStackPop(&(*S), int_error);
			parserStackPush(&(*S), N_FUNC, int_error);
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
			parserStackPop(&(*S), int_error);
			parserStackPush(&(*S), N_EXPR_O, int_error);
		}

		else if((*token)->type == T_ASSIGN){
			parserStackPop(&(*S), int_error);
			parserStackPush(&(*S), N_DEFVAR, int_error);
			parserStackPush(&(*S), T_ASSIGN, int_error);
		}

		else if((*token)->type == T_EOL){
			parserStackPop(&(*S), int_error);
		}

		else *error = 2;
	}

	else if(S->a[S->last] == N_TYPE){
		if((*token)->type == T_NIL){
			parserStackPop(&(*S), int_error);
			parserStackPush(&(*S), T_NIL, int_error);	
		}

		else if((*token)->type == T_INTEGER){
			parserStackPop(&(*S), int_error);
			parserStackPush(&(*S), T_INTEGER, int_error);	
		}

		else if((*token)->type == T_STRING){
			parserStackPop(&(*S), int_error);
			parserStackPush(&(*S), T_STRING, int_error);	
		}

		else if((*token)->type == T_FLOAT){
			parserStackPop(&(*S), int_error);
			parserStackPush(&(*S), T_FLOAT, int_error);	
		}

		else *error = 2;
	}

	else if(S->a[S->last] == N_DEFUNC){
		if((*token)->type == T_DEF){
				parserStackPop(&(*S), int_error);
				parserStackPush(&(*S), T_END, int_error);
				parserStackPush(&(*S), N_BODY, int_error);
				parserStackPush(&(*S), T_EOL, int_error);
				parserStackPush(&(*S), T_RBRCKT, int_error);
				parserStackPush(&(*S), N_PARS, int_error);
				parserStackPush(&(*S), T_LBRCKT, int_error);
				parserStackPush(&(*S), T_ID, int_error);
				parserStackPush(&(*S), T_DEF, int_error);
		}

		else *error = 2;
	}

	else if(S->a[S->last] == N_FUNC){
		parserStackPop(&(*S), int_error);

		if((*token)->type == T_ID ||
		(*token)->type == T_NIL ||
		(*token)->type == T_INTEGER ||
		(*token)->type == T_STRING ||
		(*token)->type == T_FLOAT){
			parserStackPush(&(*S), N_PARS, int_error);
		}
		
		else if ((*token)->type == T_LBRCKT){
			parserStackPush(&(*S), T_RBRCKT, int_error);
			parserStackPush(&(*S), N_PARS, int_error);
			parserStackPush(&(*S), T_LBRCKT, int_error);
		}
	}

	else if(S->a[S->last] == N_PARS){
		parserStackPop(&(*S), int_error);

		if((*token)->type == T_ID){
			parserStackPush(&(*S), N_PARSN, int_error);
			parserStackPush(&(*S), T_ID, int_error);
		}

		else if((*token)->type == T_NIL ||
		(*token)->type == T_INTEGER ||
		(*token)->type == T_STRING ||
		(*token)->type == T_FLOAT){
			parserStackPush(&(*S), N_PARSN, int_error);
			parserStackPush(&(*S), N_TYPE, int_error);
		}
	}

	else if(S->a[S->last] == N_PARSN){
		parserStackPop(&(*S), int_error);

		if((*token)->type == T_COMMA){
			parserStackPush(&(*S), N_PARS, int_error);
			parserStackPush(&(*S), T_COMMA, int_error);
		}
	}

	else if(S->a[S->last] == N_IF){
		if((*token)->type == T_IF){
			parserStackPop(&(*S), int_error);
			parserStackPush(&(*S), T_END, int_error);
			parserStackPush(&(*S), N_BODY, int_error);
			parserStackPush(&(*S), T_EOL, int_error);
			parserStackPush(&(*S), T_ELSE, int_error);
			parserStackPush(&(*S), N_BODY, int_error);
			parserStackPush(&(*S), T_EOL, int_error);
			parserStackPush(&(*S), T_THEN, int_error);
			parserStackPush(&(*S), N_EXPR, int_error);
			parserStackPush(&(*S), T_IF, int_error);
		}

		else *error = 2;
	}

	else if(S->a[S->last] == N_WHILE){
		if((*token)->type == T_WHILE){
			parserStackPop(&(*S), int_error);
			parserStackPush(&(*S), T_END, int_error);
			parserStackPush(&(*S), N_BODY, int_error);
			parserStackPush(&(*S), T_EOL, int_error);
			parserStackPush(&(*S), T_DO, int_error);
			parserStackPush(&(*S), N_EXPR, int_error);
			parserStackPush(&(*S), T_WHILE, int_error);
		}

		else *error = 2;
	}

	else if(S->a[S->last] == N_DEFVAR){
		if((*token)->type == T_ID){
			parserStackPop(&(*S), int_error);
			parserStackPush(&(*S), N_DEFVARID, int_error);
			parserStackPush(&(*S), T_ID, int_error);
		}

		else if((*token)->type == T_NIL ||
		(*token)->type == T_INTEGER ||
		(*token)->type == T_FLOAT ||
		(*token)->type == T_STRING ||
		(*token)->type == T_NOT ||
		(*token)->type == T_LBRCKT){
			parserStackPop(&(*S), int_error);
			parserStackPush(&(*S), N_EXPR, int_error);
		}

		else *error = 2;
	}

	else if(S->a[S->last] == N_DEFVARID){
		parserStackPop(&(*S), int_error);

		if((*token)->type == T_LBRCKT ||
		(*token)->type == T_ID ||
		(*token)->type == T_NIL ||
		(*token)->type == T_INTEGER ||
		(*token)->type == T_FLOAT ||
		(*token)->type == T_STRING){
			parserStackPush(&(*S), N_FUNC, int_error);
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
			parserStackPush(&(*S), N_EXPR_O, int_error);
		}
	} 

	else if(S->a[S->last] == N_EXPR_O){
		*token = (*token)->prevToken;
		if(exprParse(&(*token), NULL)) 
			*error = 69;

		if((*token)->type == T_EOL ||
		(*token)->type == T_EOF){
			parserStackPop(&(*S), int_error);
		}

		else *error = 2;
	}

	else if(S->a[S->last] == N_EXPR){
		if (exprParse(&(*token), NULL)) 
			*error = 69;

		if((*token)->type == T_EOL ||
		(*token)->type == T_THEN ||
		(*token)->type == T_DO ||
		(*token)->type == T_EOF){
			parserStackPop(&(*S), int_error);
		}

		else *error = 2;	// TODO Fixni error, když je přiřazení místo expressionu např. ve while cyklu
	}

	else{
		fprintf(stderr, "[INTERNAL ERROR]: Unexpected token on stack!\n");
		return 1;
	}

	return 0;
}

int parserStackInit(tStack *S){
	S->top = 1;
	S->last = 0;

	if(!(S->a = malloc(STACK_CHUNK_SIZE * sizeof(tType)))){
		fprintf(stderr, "[INTERNAL ERROR]: Failed malloc during stack initialization!\n");
		return 1;
	}

	S->size = STACK_CHUNK_SIZE;
	S->a[0] = N_PROG; 				// Výchozí neterminál je <prog>
	return 0;
}

void parserStackDelete(tStack *S){
	free(S->a);
}

void parserStackPush(tStack *S, tType type, int *int_error){
	if (S->top == S->size){
		S->size += STACK_CHUNK_SIZE;
		S->a = realloc(S->a, S->size * sizeof(tType));
		if(S->a == NULL){
			fprintf(stderr, "[INTERNAL ERROR]: Failed malloc during stack expansion!\n");
			*int_error = 1;
		}
	}
	
	S->a[S->top] = type;
	S->top++;
	S->last++;
}

tType parserStackPop(tStack *S, int *int_error){
	if (S->top==0) {
		fprintf(stderr, "[INTERNAL ERROR]: Stack underflow!\n");
		*int_error = 1;
		return(T_UNKNOWN);
	}	
	else {
		S->last--;
		return(S->a[S->top--]); 
	}	
}

int parserSemanticsPreRun(pToken *token, psTree *func_table){
	if((*token)->type == T_ID){

		if((*token)->prevToken != NULL && (*token)->prevToken->type == T_DEF){
			
			if(!(symTabSearch(func_table, (*token)->data))){
				psTree localFrame;
				symTabInit(&localFrame);
				psData data = parserSemanticsInitData(FUNC, localFrame, 0);
				pToken param = (*token)->nextToken->nextToken;

				while(param != NULL){
					if(param->type == T_COMMA){
						param = param->nextToken;
					}

					else if(param->type == T_ID){
						symTabInsert(&localFrame, param->data, parserSemanticsInitData(VAR, NULL, 0));
						param = param->nextToken;
						data->params++;
					}

					else if(param->type == T_STRING || param->type == T_FLOAT || param->type == T_INTEGER || param->type == T_NIL){
						return 42;
					}

					else break;
				}

				symTabInsert(func_table, (*token)->data, data);
			}

			else{
				 fprintf(stderr, "[SEMANTICS] Error: Attempted to redefine a function!\n");
				 return 3;
			}
		}
	}

	return 0;
}

psData parserSemanticsInitData(sType type, struct sTree *localFrame, int params){
	psData data = malloc(sizeof(struct sData));
	data->type = type;
	data->localFrame = &(*localFrame);
	data->params = params;
	return data;
}

void parserSemanticsInit(psTree *func_table){
	symTabInsert(func_table, "print", parserSemanticsInitData(FUNC, NULL, -1));
	symTabInsert(func_table, "inputs", parserSemanticsInitData(FUNC, NULL, 0));
	symTabInsert(func_table, "inputi", parserSemanticsInitData(FUNC, NULL, 0));
	symTabInsert(func_table, "inputf", parserSemanticsInitData(FUNC, NULL, 0));
	symTabInsert(func_table, "length", parserSemanticsInitData(FUNC, NULL, 1));
	symTabInsert(func_table, "substr", parserSemanticsInitData(FUNC, NULL, 3));
	symTabInsert(func_table, "ord", parserSemanticsInitData(FUNC, NULL, 2));
	symTabInsert(func_table, "chr", parserSemanticsInitData(FUNC, NULL, 1));
	return;
}

void parserSemanticsInFunc(bool *inFunc, int *inAux, pToken semanToken){
	if(semanToken->type == T_DEF){
		*inFunc = true;
	}

	else if(semanToken->type == T_IF || semanToken->type == T_WHILE){
		*inAux = *inAux + 1;
	}

	else if(semanToken->type == T_END && !(*inAux)){
		*inFunc = false;
	}

	else if(semanToken->type == T_END && *inAux){
		*inAux = *inAux - 1;
	}
}

int parserError(int error, int int_error, pToken *prevToken, tStack *S){
	if(int_error) return 99;

	if(error){
			if(error == 2){
				fprintf(stderr, "[SYNTAX] Error on line %u:%u - expected %s, got %s\n",
					(*prevToken)->linePos, (*prevToken)->colPos, scannerTypeToString((*S).a[(*S).top]), scannerTypeToString((*prevToken)->type));
				return 2;
			}

			else if(error == 69){	//Syntaktický error expressionu
				return 2;
			}

			else if(error == 3){
				return 3;
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

//TODO NOT není NOT