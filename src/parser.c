#include "parser.h"
#include "expressions.h"

int parser(pToken *List){

	pToken token = (*List);
	int error = 0;				// Chyba vstupního kódu
	int int_error = 0;			// Interní chyba překladače
	tStack S;
	int_error = parserStackInit(&S);

	while(token != NULL){

		pToken prevToken = token;

		if(S.a[S.last] > T_STRING){
			int_error = parserExpand(&S, &token, &error, &int_error);
		}

		else{
			error = (parserCompare(S, token));
			parserStackPop(&S, &int_error);
			token = token->nextToken;
		}

		if(error){
			if(error == 2){
				fprintf(stderr, "[SYNTAX] Error on line %u:%u - expected %s, got %s\n",
					prevToken->linePos, prevToken->colPos, scannerTypeToString(S.a[S.top]), scannerTypeToString(prevToken->type));
				return 2;
			}

			else if(error == 3){
				return 2;
			}

			else{
				fprintf(stderr, "[INTERNAL ERROR]: Unexplainable unbelievable problem!\n");
				return 99;
			}
		}

		if(int_error) return 99;
	}

	
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
		if((*token)->type == T_EOL ||
		(*token)->type == T_EOF){
			parserStackPop(&(*S), int_error);
		}

		else{
			*token = (*token)->prevToken;
			if(exprParse(&(*token), NULL)) 
			*error = 3;
		}
	}

	else if(S->a[S->last] == N_EXPR){
		if((*token)->type == T_EOL ||
		(*token)->type == T_THEN ||
		(*token)->type == T_DO ||
		(*token)->type == T_EOF){
			parserStackPop(&(*S), int_error);
		}

		else{
			if (exprParse(&(*token), NULL)) 
			*error = 3; 
		}
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

//TODO not není not