#include "parser.h"

void parser(pToken *List){

	pToken token = (*List);

	bool correct = true;

	const char *wrong = NULL;

	tStack S;
	parserStackInit(&S);

	while(token != NULL){

		if(S.a[S.last] > T_STRING){
			parserExpand(&S, &token, &correct);
		}

		else{
			correct = (parserCompare(S, token));
			parserStackPop(&S);
			token = (token)->nextToken;
		}

		if(!correct){
			wrong = scannerTypeToString(S.a[S.top]);
			fprintf(stderr, "Chyba: %u. řádek, očekávaný symbol: %s\n", token->linePos, wrong);
			return;
		}
	}
}

bool parserCompare(tStack S, pToken token){
	return (S.a[S.last] == token->type);
}

void parserExpand(tStack *S, pToken *token, bool *correct){
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
			parserStackPop(&(*S));
			parserStackPush(&(*S), N_PROG);
			parserStackPush(&(*S), N_BODY);
		}

		else if((*token)->type == T_DEF){
			parserStackPop(&(*S));
			parserStackPush(&(*S), N_PROG);
			parserStackPush(&(*S), T_EOL);
			parserStackPush(&(*S), N_DEFUNC);
		}

		else if((*token)->type == T_EOF){
			parserStackPop(&(*S));
			parserStackPush(&(*S), T_EOF);
		}
		
		else *correct = false;
	}

	else if(S->a[S->last] == N_BODY){
		parserStackPop(&(*S));

		if((*token)->type == T_ID){
			parserStackPush(&(*S), N_BODY);
			parserStackPush(&(*S), T_EOL);
			parserStackPush(&(*S), N_BODY_ID);
			parserStackPush(&(*S), T_ID);
		}

		else if((*token)->type == T_NIL ||
		(*token)->type == T_INTEGER ||
		(*token)->type == T_STRING ||
		(*token)->type == T_FLOAT ||
		(*token)->type == T_NOT ||
		(*token)->type == T_LBRCKT){
			parserStackPush(&(*S), N_BODY);
			parserStackPush(&(*S), T_EOL);
			parserStackPush(&(*S), N_EXPR);
		}

		else if((*token)->type == T_IF){
			parserStackPush(&(*S), N_BODY);
			parserStackPush(&(*S), T_EOL);
			parserStackPush(&(*S), N_IF);
		}

		else if((*token)->type == T_WHILE){
			parserStackPush(&(*S), N_BODY);
			parserStackPush(&(*S), T_EOL);
			parserStackPush(&(*S), N_WHILE);
		}

		else if((*token)->type == T_EOL){
			parserStackPush(&(*S), N_BODY);
			parserStackPush(&(*S), T_EOL);
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
			parserStackPop(&(*S));
			parserStackPush(&(*S), N_FUNC);
		}

		else if((*token)->type == T_ASSIGN){
			parserStackPop(&(*S));
			parserStackPush(&(*S), N_DEFVAR);
			parserStackPush(&(*S), T_ASSIGN);
		}

		else if((*token)->type == T_EOL){
			parserStackPop(&(*S));
		}

		else *correct = false;
	}

	else if(S->a[S->last] == N_TYPE){
		if((*token)->type == T_NIL){
			parserStackPop(&(*S));
			parserStackPush(&(*S), T_NIL);	
		}

		else if((*token)->type == T_INTEGER){
			parserStackPop(&(*S));
			parserStackPush(&(*S), T_INTEGER);	
		}

		else if((*token)->type == T_STRING){
			parserStackPop(&(*S));
			parserStackPush(&(*S), T_STRING);	
		}

		else if((*token)->type == T_FLOAT){
			parserStackPop(&(*S));
			parserStackPush(&(*S), T_FLOAT);	
		}

		else *correct = false;
	}

	else if(S->a[S->last] == N_DEFUNC){
		if((*token)->type == T_DEF){
				parserStackPop(&(*S));
				parserStackPush(&(*S), T_END);
				parserStackPush(&(*S), N_BODY);
				parserStackPush(&(*S), T_EOL);
				parserStackPush(&(*S), T_RBRCKT);
				parserStackPush(&(*S), N_PARS);
				parserStackPush(&(*S), T_LBRCKT);
				parserStackPush(&(*S), T_ID);
				parserStackPush(&(*S), T_DEF);
		}

		else *correct = false;
	}

	else if(S->a[S->last] == N_FUNC){
		parserStackPop(&(*S));

		if((*token)->type == T_ID ||
		(*token)->type == T_NIL ||
		(*token)->type == T_INTEGER ||
		(*token)->type == T_STRING ||
		(*token)->type == T_FLOAT){
			parserStackPush(&(*S), N_PARS);
		}
		
		else if ((*token)->type == T_LBRCKT){
			parserStackPush(&(*S), T_RBRCKT);
			parserStackPush(&(*S), N_PARS);
			parserStackPush(&(*S), T_LBRCKT);
		}
	}

	else if(S->a[S->last] == N_PARS){
		parserStackPop(&(*S));

		if((*token)->type == T_ID){
			parserStackPush(&(*S), N_PARSN);
			parserStackPush(&(*S), T_ID);
		}

		else if((*token)->type == T_NIL ||
		(*token)->type == T_INTEGER ||
		(*token)->type == T_STRING ||
		(*token)->type == T_FLOAT){
			parserStackPush(&(*S), N_PARSN);
			parserStackPush(&(*S), N_TYPE);
		}
	}

	else if(S->a[S->last] == N_PARSN){
		parserStackPop(&(*S));

		if((*token)->type == T_COMMA){
			parserStackPush(&(*S), N_PARS);
			parserStackPush(&(*S), T_COMMA);
		}
	}

	else if(S->a[S->last] == N_IF){
		if((*token)->type == T_IF){
			parserStackPop(&(*S));
			parserStackPush(&(*S), T_END);
			parserStackPush(&(*S), N_BODY);
			parserStackPush(&(*S), T_EOL);
			parserStackPush(&(*S), T_ELSE);
			parserStackPush(&(*S), N_BODY);
			parserStackPush(&(*S), T_EOL);
			parserStackPush(&(*S), T_THEN);
			parserStackPush(&(*S), N_EXPR);
			parserStackPush(&(*S), T_IF);
		}

		else *correct = false;
	}

	else if(S->a[S->last] == N_WHILE){
		if((*token)->type == T_WHILE){
			parserStackPop(&(*S));
			parserStackPush(&(*S), T_END);
			parserStackPush(&(*S), N_BODY);
			parserStackPush(&(*S), T_EOL);
			parserStackPush(&(*S), T_DO);
			parserStackPush(&(*S), N_EXPR);
			parserStackPush(&(*S), T_WHILE);
		}

		else *correct = false;
	}

	else if(S->a[S->last] == N_DEFVAR){
		if((*token)->type == T_ID){
			parserStackPop(&(*S));
			parserStackPush(&(*S), N_DEFVARID);
			parserStackPush(&(*S), T_ID);
		}

		else if((*token)->type == T_NIL ||
		(*token)->type == T_INTEGER ||
		(*token)->type == T_FLOAT ||
		(*token)->type == T_STRING ||
		(*token)->type == T_NOT ||
		(*token)->type == T_LBRCKT){
			parserStackPop(&(*S));
			parserStackPush(&(*S), N_EXPR);
		}

		else *correct = false;
	}

	else if(S->a[S->last] == N_DEFVARID){
		parserStackPop(&(*S));

		if((*token)->type == T_LBRCKT ||
		(*token)->type == T_ID ||
		(*token)->type == T_NIL ||
		(*token)->type == T_INTEGER ||
		(*token)->type == T_FLOAT ||
		(*token)->type == T_STRING){
			parserStackPush(&(*S), N_FUNC);
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
			parserStackPush(&(*S), N_EXPR_O);
		}
	}

	else if(S->a[S->last] == N_EXPR_ID){
		if((*token)->type == T_ID){
			parserStackPop(&(*S));
			parserStackPush(&(*S), N_EXPR_O);
			parserStackPush(&(*S), T_ID);
		}

		else if((*token)->type == T_NIL ||
		(*token)->type == T_INTEGER ||
		(*token)->type == T_STRING ||
		(*token)->type == T_FLOAT ||
		(*token)->type == T_NOT){
			parserStackPop(&(*S));
			parserStackPush(&(*S), N_EXPR);
		}

		else if((*token)->type == T_LBRCKT){
			parserStackPop(&(*S));
			parserStackPush(&(*S), N_EXPR_O);
		}

		else *correct = false;
	}

	else if(S->a[S->last] == N_EXPR_O){
		parserStackPop(&(*S));

		if((*token)->type == T_ADD){
			parserStackPush(&(*S), N_EXPR_ID);
			parserStackPush(&(*S), T_ADD);
		}

		else if((*token)->type == T_SUB){
			parserStackPush(&(*S), N_EXPR_ID);
			parserStackPush(&(*S), T_SUB);
		}

		else if((*token)->type == T_DIV){
			parserStackPush(&(*S), N_EXPR_ID);
			parserStackPush(&(*S), T_DIV);
		}

		else if((*token)->type == T_MUL){
			parserStackPush(&(*S), N_EXPR_ID);
			parserStackPush(&(*S), T_MUL);
		}

		else if((*token)->type == T_EQL){
			parserStackPush(&(*S), N_EXPR_ID);
			parserStackPush(&(*S), T_EQL);
		}

		else if((*token)->type == T_NEQ){
			parserStackPush(&(*S), N_EXPR_ID);
			parserStackPush(&(*S), T_NEQ);
		}

		else if((*token)->type == T_GT){
			parserStackPush(&(*S), N_EXPR_ID);
			parserStackPush(&(*S), T_GT);
		}

		else if((*token)->type == T_LT){
			parserStackPush(&(*S), N_EXPR_ID);
			parserStackPush(&(*S), T_LT);
		}

		else if((*token)->type == T_GTE){
			parserStackPush(&(*S), N_EXPR_ID);
			parserStackPush(&(*S), T_GTE);
		}

		else if((*token)->type == T_LTE){
			parserStackPush(&(*S), N_EXPR_ID);
			parserStackPush(&(*S), T_LTE);
		}
	}

	else if(S->a[S->last] == N_EXPR){
		if((*token)->type == T_EOL ||
		(*token)->type == T_THEN ||
		(*token)->type == T_DO ||
		(*token)->type == T_EOF){
			parserStackPop(&(*S));
		}

		else *token = (*token)->nextToken;
	}

	else fprintf(stderr, "Interní chyba: Neočekávaný token na stacku!\n");

	return;
}

void parserStackInit(tStack *S){
	S->top = 1;
	S->last = 0;
	S->a[0] = N_PROG; 		// Výchozí neterminál je <prog>
}	

void parserStackPush(tStack *S, tType type){
	if (S->top==MAXSTACK){ 
		fprintf(stderr, "Interní chyba: Došlo k přetečení zásobníku!\n");
		return; //TODO
	}
	else {
		S->a[S->top]=type;
		S->top++;
		S->last++;  
	}	
}

tType parserStackPop(tStack *S){
	if (S->top==0) {
		fprintf(stderr, "Interní chyba: Došlo k podtečení zásobníku!\n");
		return(T_UNKNOWN); //TODO
	}	
	else {
		S->last--;
		return(S->a[S->top--]); 
	}	
}

bool parserStackEmpty(tStack *S){
	return(S->top==0);
}