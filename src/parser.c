#include "parser.h"

int parser(pToken *List){

	pToken token = (*List);
	bool correct = true;
	int error = 0;
	tStack S;
	error = parserStackInit(&S);

	while(token != NULL){

		pToken prevToken = token;

		if(S.a[S.last] > T_STRING){
			error = parserExpand(&S, &token, &correct, &error);
		}

		else{
			correct = (parserCompare(S, token));
			parserStackPop(&S, &error);
			token = token->nextToken;
		}

		if(!correct){
			fprintf(stderr, "[SYNTAX] Error on line %u:%u - expected %s, got %s\n", 
				prevToken->linePos, prevToken->colPos, scannerTypeToString(S.a[S.top]), scannerTypeToString(prevToken->type));
			return 2;
		}

		if(error) return 99;
	}

	
	parserStackDelete(&S);
	return 0;
}

bool parserCompare(tStack S, pToken token){
	return (S.a[S.last] == token->type);
}

int parserExpand(tStack *S, pToken *token, bool *correct, int *error){
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
			parserStackPop(&(*S), error);
			parserStackPush(&(*S), N_PROG, error);
			parserStackPush(&(*S), N_BODY, error);
		}

		else if((*token)->type == T_DEF){
			parserStackPop(&(*S), error);
			parserStackPush(&(*S), N_PROG, error);
			parserStackPush(&(*S), T_EOL, error);
			parserStackPush(&(*S), N_DEFUNC, error);
		}

		else if((*token)->type == T_EOF){
			parserStackPop(&(*S), error);
			parserStackPush(&(*S), T_EOF, error);
		}
		
		else *correct = false;
	}

	else if(S->a[S->last] == N_BODY){
		parserStackPop(&(*S), error);

		if((*token)->type == T_ID){
			parserStackPush(&(*S), N_BODY, error);
			parserStackPush(&(*S), T_EOL, error);
			parserStackPush(&(*S), N_BODY_ID, error);
			parserStackPush(&(*S), T_ID, error);
		}

		else if((*token)->type == T_NIL ||
		(*token)->type == T_INTEGER ||
		(*token)->type == T_STRING ||
		(*token)->type == T_FLOAT ||
		(*token)->type == T_NOT ||
		(*token)->type == T_LBRCKT){
			parserStackPush(&(*S), N_BODY, error);
			parserStackPush(&(*S), T_EOL, error);
			parserStackPush(&(*S), N_EXPR, error);
		}

		else if((*token)->type == T_IF){
			parserStackPush(&(*S), N_BODY, error);
			parserStackPush(&(*S), T_EOL, error);
			parserStackPush(&(*S), N_IF, error);
		}

		else if((*token)->type == T_WHILE){
			parserStackPush(&(*S), N_BODY, error);
			parserStackPush(&(*S), T_EOL, error);
			parserStackPush(&(*S), N_WHILE, error);
		}

		else if((*token)->type == T_EOL){
			parserStackPush(&(*S), N_BODY, error);
			parserStackPush(&(*S), T_EOL, error);
		}
	}

	else if(S->a[S->last] == N_BODY_ID){
		if((*token)->type == T_ID ||
		(*token)->type == T_NIL ||
		(*token)->type == T_INTEGER ||
		(*token)->type == T_STRING ||
		(*token)->type == T_FLOAT ||
		(*token)->type == T_LBRCKT ||
		(*token)->type == T_ADD ||
		(*token)->type == T_SUB ||
		(*token)->type == T_MUL ||
		(*token)->type == T_DIV ||
		(*token)->type == T_EQL ||
		(*token)->type == T_NEQ ||
		(*token)->type == T_GT ||
		(*token)->type == T_LT ||
		(*token)->type == T_GTE ||
		(*token)->type == T_LTE){
			parserStackPop(&(*S), error);
			parserStackPush(&(*S), N_FUNC, error);
		}

		else if((*token)->type == T_ASSIGN){
			parserStackPop(&(*S), error);
			parserStackPush(&(*S), N_DEFVAR, error);
			parserStackPush(&(*S), T_ASSIGN, error);
		}

		else if((*token)->type == T_EOL){
			parserStackPop(&(*S), error);
		}

		else *correct = false;
	}

	else if(S->a[S->last] == N_TYPE){
		if((*token)->type == T_NIL){
			parserStackPop(&(*S), error);
			parserStackPush(&(*S), T_NIL, error);	
		}

		else if((*token)->type == T_INTEGER){
			parserStackPop(&(*S), error);
			parserStackPush(&(*S), T_INTEGER, error);	
		}

		else if((*token)->type == T_STRING){
			parserStackPop(&(*S), error);
			parserStackPush(&(*S), T_STRING, error);	
		}

		else if((*token)->type == T_FLOAT){
			parserStackPop(&(*S), error);
			parserStackPush(&(*S), T_FLOAT, error);	
		}

		else *correct = false;
	}

	else if(S->a[S->last] == N_DEFUNC){
		if((*token)->type == T_DEF){
				parserStackPop(&(*S), error);
				parserStackPush(&(*S), T_END, error);
				parserStackPush(&(*S), N_BODY, error);
				parserStackPush(&(*S), T_EOL, error);
				parserStackPush(&(*S), T_RBRCKT, error);
				parserStackPush(&(*S), N_PARS, error);
				parserStackPush(&(*S), T_LBRCKT, error);
				parserStackPush(&(*S), T_ID, error);
				parserStackPush(&(*S), T_DEF, error);
		}

		else *correct = false;
	}

	else if(S->a[S->last] == N_FUNC){
		parserStackPop(&(*S), error);

		if((*token)->type == T_ID ||
		(*token)->type == T_NIL ||
		(*token)->type == T_INTEGER ||
		(*token)->type == T_STRING ||
		(*token)->type == T_FLOAT){
			parserStackPush(&(*S), N_PARS, error);
		}
		
		else if ((*token)->type == T_LBRCKT){
			parserStackPush(&(*S), T_RBRCKT, error);
			parserStackPush(&(*S), N_PARS, error);
			parserStackPush(&(*S), T_LBRCKT, error);
		}
	}

	else if(S->a[S->last] == N_PARS){
		parserStackPop(&(*S), error);

		if((*token)->type == T_ID){
			parserStackPush(&(*S), N_PARSN, error);
			parserStackPush(&(*S), T_ID, error);
		}

		else if((*token)->type == T_NIL ||
		(*token)->type == T_INTEGER ||
		(*token)->type == T_STRING ||
		(*token)->type == T_FLOAT){
			parserStackPush(&(*S), N_PARSN, error);
			parserStackPush(&(*S), N_TYPE, error);
		}
	}

	else if(S->a[S->last] == N_PARSN){
		parserStackPop(&(*S), error);

		if((*token)->type == T_COMMA){
			parserStackPush(&(*S), N_PARS, error);
			parserStackPush(&(*S), T_COMMA, error);
		}
	}

	else if(S->a[S->last] == N_IF){
		if((*token)->type == T_IF){
			parserStackPop(&(*S), error);
			parserStackPush(&(*S), T_END, error);
			parserStackPush(&(*S), N_BODY, error);
			parserStackPush(&(*S), T_EOL, error);
			parserStackPush(&(*S), T_ELSE, error);
			parserStackPush(&(*S), N_BODY, error);
			parserStackPush(&(*S), T_EOL, error);
			parserStackPush(&(*S), T_THEN, error);
			parserStackPush(&(*S), N_EXPR, error);
			parserStackPush(&(*S), T_IF, error);
		}

		else *correct = false;
	}

	else if(S->a[S->last] == N_WHILE){
		if((*token)->type == T_WHILE){
			parserStackPop(&(*S), error);
			parserStackPush(&(*S), T_END, error);
			parserStackPush(&(*S), N_BODY, error);
			parserStackPush(&(*S), T_EOL, error);
			parserStackPush(&(*S), T_DO, error);
			parserStackPush(&(*S), N_EXPR, error);
			parserStackPush(&(*S), T_WHILE, error);
		}

		else *correct = false;
	}

	else if(S->a[S->last] == N_DEFVAR){
		if((*token)->type == T_ID){
			parserStackPop(&(*S), error);
			parserStackPush(&(*S), N_DEFVARID, error);
			parserStackPush(&(*S), T_ID, error);
		}

		else if((*token)->type == T_NIL ||
		(*token)->type == T_INTEGER ||
		(*token)->type == T_FLOAT ||
		(*token)->type == T_STRING ||
		(*token)->type == T_NOT ||
		(*token)->type == T_LBRCKT){
			parserStackPop(&(*S), error);
			parserStackPush(&(*S), N_EXPR, error);
		}

		else *correct = false;
	}

	else if(S->a[S->last] == N_DEFVARID){
		parserStackPop(&(*S), error);

		if((*token)->type == T_LBRCKT ||
		(*token)->type == T_ID ||
		(*token)->type == T_NIL ||
		(*token)->type == T_INTEGER ||
		(*token)->type == T_FLOAT ||
		(*token)->type == T_STRING){
			parserStackPush(&(*S), N_FUNC, error);
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
			parserStackPush(&(*S), N_EXPR_O, error);
		}
	}

	else if(S->a[S->last] == N_EXPR_ID){
		if((*token)->type == T_ID){
			parserStackPop(&(*S), error);
			parserStackPush(&(*S), N_EXPR_O, error);
			parserStackPush(&(*S), T_ID, error);
		}

		else if((*token)->type == T_NIL ||
		(*token)->type == T_INTEGER ||
		(*token)->type == T_STRING ||
		(*token)->type == T_FLOAT ||
		(*token)->type == T_NOT ||
		(*token)->type == T_LBRCKT){
			parserStackPop(&(*S), error);
			parserStackPush(&(*S), N_EXPR, error);
		}

		else *correct = false;
	}

	else if(S->a[S->last] == N_EXPR_O){
		parserStackPop(&(*S), error);

		if((*token)->type == T_ADD){
			parserStackPush(&(*S), N_EXPR_ID, error);
			parserStackPush(&(*S), T_ADD, error);
		}

		else if((*token)->type == T_SUB){
			parserStackPush(&(*S), N_EXPR_ID, error);
			parserStackPush(&(*S), T_SUB, error);
		}

		else if((*token)->type == T_DIV){
			parserStackPush(&(*S), N_EXPR_ID, error);
			parserStackPush(&(*S), T_DIV, error);
		}

		else if((*token)->type == T_MUL){
			parserStackPush(&(*S), N_EXPR_ID, error);
			parserStackPush(&(*S), T_MUL, error);
		}

		else if((*token)->type == T_EQL){
			parserStackPush(&(*S), N_EXPR_ID, error);
			parserStackPush(&(*S), T_EQL, error);
		}

		else if((*token)->type == T_NEQ){
			parserStackPush(&(*S), N_EXPR_ID, error);
			parserStackPush(&(*S), T_NEQ, error);
		}

		else if((*token)->type == T_GT){
			parserStackPush(&(*S), N_EXPR_ID, error);
			parserStackPush(&(*S), T_GT, error);
		}

		else if((*token)->type == T_LT){
			parserStackPush(&(*S), N_EXPR_ID, error);
			parserStackPush(&(*S), T_LT, error);
		}

		else if((*token)->type == T_GTE){
			parserStackPush(&(*S), N_EXPR_ID, error);
			parserStackPush(&(*S), T_GTE, error);
		}

		else if((*token)->type == T_LTE){
			parserStackPush(&(*S), N_EXPR_ID, error);
			parserStackPush(&(*S), T_LTE, error);
		}
	}

	else if(S->a[S->last] == N_EXPR){
		if((*token)->type == T_EOL ||
		(*token)->type == T_THEN ||
		(*token)->type == T_DO ||
		(*token)->type == T_EOF){
			parserStackPop(&(*S), error);
		}

		else *token = (*token)->nextToken;
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

void parserStackPush(tStack *S, tType type, int *error){
	if (S->top == S->size){
		S->size += STACK_CHUNK_SIZE;
		S->a = realloc(S->a, S->size * sizeof(tType));
		if(S->a == NULL){
			fprintf(stderr, "[INTERNAL ERROR]: Failed malloc during stack expansion!\n");
			*error = 1;
		}
	}
	
	S->a[S->top] = type;
	S->top++;
	S->last++;
}

tType parserStackPop(tStack *S, int *error){
	if (S->top==0) {
		fprintf(stderr, "[INTERNAL ERROR]: Stack underflow!\n");
		*error = 1;
		return(T_UNKNOWN);
	}	
	else {
		S->last--;
		return(S->a[S->top--]); 
	}	
}