#include "parser.h"

void parser(pToken *List){

	pToken token = (*List);

	bool correct = true;

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
			fprintf(stderr, "Chyba: Neočekávaný symbol!");
			return;
		}
	}
}

bool parserCompare(tStack S, pToken token){
	return (S.a[S.last] == token->type);
}

void parserExpand(tStack *S, pToken *token, bool *correct){
	if(S->a[S->last] == N_PROG){
		switch((*token)->type){
			case T_ID:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_PROG);
				parserStackPush(&(*S), N_BODY);
				break;
			case T_NIL:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_PROG);
				parserStackPush(&(*S), N_BODY);
				break;
			case T_INTEGER:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_PROG);
				parserStackPush(&(*S), N_BODY);
				break;
			case T_STRING:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_PROG);
				parserStackPush(&(*S), N_BODY);
				break;
			case T_FLOAT:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_PROG);
				parserStackPush(&(*S), N_BODY);
				break;
			case T_NOT:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_PROG);
				parserStackPush(&(*S), N_BODY);
				break;
			case T_LBRCKT:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_PROG);
				parserStackPush(&(*S), N_BODY);
				break;
			case T_IF:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_PROG);
				parserStackPush(&(*S), N_BODY);
				break;
			case T_WHILE:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_PROG);
				parserStackPush(&(*S), N_BODY);
				break;
			case T_DEF:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_PROG);
				parserStackPush(&(*S), N_NEWL);
				parserStackPush(&(*S), N_DEFUNC);
				break;
			case T_EOF:
				parserStackPop(&(*S));
				parserStackPush(&(*S), T_EOF);
				break;
			default:
				*correct = false;
				break;
		}
	}

	else if(S->a[S->last] == N_BODY){
		switch((*token)->type){
			case T_ID:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_BODY);
				parserStackPush(&(*S), N_NEWL);
				parserStackPush(&(*S), N_BODY_ID);
				parserStackPush(&(*S), T_ID);
				break;
			case T_NIL:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_BODY);
				parserStackPush(&(*S), N_NEWL);
				parserStackPush(&(*S), N_EXPR);
				break;
			case T_INTEGER:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_BODY);
				parserStackPush(&(*S), N_NEWL);
				parserStackPush(&(*S), N_EXPR);
				break;
			case T_STRING:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_BODY);
				parserStackPush(&(*S), N_NEWL);
				parserStackPush(&(*S), N_EXPR);
				break;
			case T_FLOAT:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_BODY);
				parserStackPush(&(*S), N_NEWL);
				parserStackPush(&(*S), N_EXPR);
				break;
			case T_NOT:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_BODY);
				parserStackPush(&(*S), N_NEWL);
				parserStackPush(&(*S), N_EXPR);
				break;
			case T_END:
				parserStackPop(&(*S));
				break;
			case T_ELSE:
				parserStackPop(&(*S));
				break;
			case T_IF:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_BODY);
				parserStackPush(&(*S), N_NEWL);
				parserStackPush(&(*S), N_IF);
				break;
			case T_WHILE:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_BODY);
				parserStackPush(&(*S), N_NEWL);
				parserStackPush(&(*S), N_WHILE);
				break;
			default:
				parserStackPop(&(*S));
				break;
		}
	}

	else if(S->a[S->last] == N_BODY_ID){
		switch((*token)->type){
			case T_ID:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_FUNC);
				break;
			case T_NIL:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_FUNC);
				break;
			case T_INTEGER:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_FUNC);
				break;
			case T_STRING:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_FUNC);
				break;
			case T_FLOAT:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_FUNC);
				break;
			case T_LBRCKT:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_FUNC);
				break;
			case T_ADD:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_EXPR_O);
				break;
			case T_SUB:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_EXPR_O);
				break;
			case T_MUL:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_EXPR_O);
				break;
			case T_DIV:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_EXPR_O);
				break;
			case T_EQL:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_EXPR_O);
				break;
			case T_NEQ:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_EXPR_O);
				break;
			case T_GT:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_EXPR_O);
				break;
			case T_LT:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_EXPR_O);
				break;
			case T_GTE:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_EXPR_O);
				break;
			case T_LTE:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_EXPR_O);
				break;
			case T_ASSIGN:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_DEFVAR);
				parserStackPush(&(*S), T_ASSIGN);
				break;
			case T_EOL:
				parserStackPop(&(*S));
				break;
			default:
				*correct = false;
				break;
		}
	}

	else if(S->a[S->last] == N_TYPE){
		switch((*token)->type){
			case T_NIL:
				parserStackPop(&(*S));
				parserStackPush(&(*S), T_NIL);			
				break;
			case T_INTEGER:
				parserStackPop(&(*S));
				parserStackPush(&(*S), T_INTEGER);
				break;
			case T_STRING:
				parserStackPop(&(*S));
				parserStackPush(&(*S), T_STRING);
				break;
			case T_FLOAT:
				parserStackPop(&(*S));
				parserStackPush(&(*S), T_FLOAT);
				break;	
			default:
				*correct = false;
				break;
		}
	}

	else if(S->a[S->last] == N_NEWL){
		switch((*token)->type){
			case T_EOL:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_NEWLN);
				parserStackPush(&(*S), T_EOL);
				break;
			default:
				*correct = false;
				return;
				break;
		}
	}

	else if(S->a[S->last] == N_NEWLN){
		switch((*token)->type){
			case T_ID:
				parserStackPop(&(*S));
				break;
			case T_NIL:
				parserStackPop(&(*S));
				break;
			case T_INTEGER:
				parserStackPop(&(*S));
				break;
			case T_STRING:
				parserStackPop(&(*S));
				break;
			case T_FLOAT:
				parserStackPop(&(*S));
				break;
			case T_NOT:
				parserStackPop(&(*S));
				break;
			case T_EOF:
				parserStackPop(&(*S));
				break;
			case T_LBRCKT:
				parserStackPop(&(*S));
				break;
			case T_DEF:
				parserStackPop(&(*S));
				break;
			case T_IF:
				parserStackPop(&(*S));
				break;
			case T_THEN:
				parserStackPop(&(*S));
				break;
			case T_EOL:
				parserStackPop(&(*S));
				parserStackPush(&(*S), T_EOL);
				break;
			default:
				parserStackPop(&(*S));
				break;
		}
	}

	else if(S->a[S->last] == N_DEFUNC){
		switch((*token)->type){
			case T_DEF:
				parserStackPop(&(*S));
				parserStackPush(&(*S), T_END);
				parserStackPush(&(*S), N_BODY);
				parserStackPush(&(*S), T_EOL);
				parserStackPush(&(*S), T_RBRCKT);
				parserStackPush(&(*S), N_PARS);
				parserStackPush(&(*S), T_LBRCKT);
				parserStackPush(&(*S), T_ID);
				parserStackPush(&(*S), T_DEF);
				break;
			default:
				*correct = false;
				break;
		}
	}

	else if(S->a[S->last] == N_FUNC){
		switch((*token)->type){
			case T_ID:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_PARS);
				break;
			case T_NIL:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_PARS);
				break;
			case T_INTEGER:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_PARS);
				break;
			case T_STRING:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_PARS);
				break;
			case T_FLOAT:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_PARS);
				break;
			case T_LBRCKT:
				parserStackPop(&(*S));
				parserStackPush(&(*S), T_RBRCKT);
				parserStackPush(&(*S), N_PARS);
				parserStackPush(&(*S), T_LBRCKT);
				break;
			case T_EOL:
				parserStackPop(&(*S)); //TODO
				parserStackPush(&(*S), N_PARS);
				break;
			default:
				*correct = false;
				break;
		}
	}

	else if(S->a[S->last] == N_PARS){
		switch((*token)->type){
			case T_ID:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_PARSN);
				parserStackPush(&(*S), T_ID);
				break;
			case T_NIL:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_PARSN);
				parserStackPush(&(*S), N_TYPE);
				break;
			case T_INTEGER:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_PARSN);
				parserStackPush(&(*S), N_TYPE);
				break;
			case T_STRING:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_PARSN);
				parserStackPush(&(*S), N_TYPE);
				break;
			case T_FLOAT:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_PARSN);
				parserStackPush(&(*S), N_TYPE);
				break;
			case T_EOL:
				parserStackPop(&(*S));
				break;
			case T_RBRCKT:
				parserStackPop(&(*S));
				break;
			default:
				parserStackPop(&(*S));
				break;
		}
	}

	else if(S->a[S->last] == N_PARSN){
		switch((*token)->type){
			case T_COMMA:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_PARS);
				parserStackPush(&(*S), T_COMMA);
				break;
			case T_RBRCKT:
				parserStackPop(&(*S));
				break;
			case T_EOL:
				parserStackPop(&(*S));
				break;
			default:
				parserStackPop(&(*S));
				break;
		}
	}

	else if(S->a[S->last] == N_IF){
		switch((*token)->type){
			case T_IF:
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
				break;
			default:
				*correct = false;
				break;
		}
	}

	else if(S->a[S->last] == N_WHILE){
		switch((*token)->type){
			case T_WHILE:
				parserStackPop(&(*S));
				parserStackPush(&(*S), T_END);
				parserStackPush(&(*S), N_BODY);
				parserStackPush(&(*S), T_EOL);
				parserStackPush(&(*S), T_DO);
				parserStackPush(&(*S), N_EXPR);
				parserStackPush(&(*S), T_WHILE);
				break;
			default:
				*correct = false;
				break;
		}
	}

	else if(S->a[S->last] == N_DEFVAR){
		switch((*token)->type){
			case T_ID:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_DEFVARID);
				parserStackPush(&(*S), T_ID);
				break;
			case T_NIL:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_EXPR);
				break;
			case T_INTEGER:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_EXPR);
				break;
			case T_FLOAT:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_EXPR);
				break;
			case T_STRING:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_EXPR);
				break;
			case T_NOT:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_EXPR);
				break;
			case T_LBRCKT:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_EXPR);
				break;
			default:
				*correct = false;
				break;
		}
	}

	else if(S->a[S->last] == N_DEFVARID){
		switch((*token)->type){
			case T_LBRCKT:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_FUNC);
				break;
			case T_ID:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_FUNC);
				break;
			case T_NIL:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_FUNC);
				break;
			case T_INTEGER:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_FUNC);
				break;
			case T_FLOAT:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_FUNC);
				break;
			case T_STRING:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_FUNC);
				break;
			case T_EOL: //TODO
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_FUNC);
				break;
			case T_ADD:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_EXPR_O);
				break;
			case T_SUB:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_EXPR_O);
				break;
			case T_DIV:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_EXPR_O);
				break;
			case T_MUL:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_EXPR_O);
				break;
			case T_EQL:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_EXPR_O);
				break;
			case T_NEQ:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_EXPR_O);
				break;
			case T_LT:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_EXPR_O);
				break;
			case T_GT:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_EXPR_O);
				break;
			case T_LTE:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_EXPR_O);
				break;
			case T_GTE:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_EXPR_O);
				break;
			default:
				*correct = false;
				break;
		}
	}

	else if(S->a[S->last] == N_EXPR_ID){
		switch((*token)->type){
			case T_ID:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_EXPR_O);
				parserStackPush(&(*S), T_ID);
				break;
			case T_NIL:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_EXPR);
				break;
			case T_INTEGER:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_EXPR);
				break;
			case T_STRING:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_EXPR);
				break;
			case T_FLOAT:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_EXPR);
				break;
			case T_NOT:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_EXPR);
				break;
			case T_LBRCKT:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_EXPR_O);
				break;
			default:
				*correct = false;
				break;
		}
	}

	else if(S->a[S->last] == N_EXPR_O){
		switch((*token)->type){
			case T_ADD:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_EXPR_ID);
				parserStackPush(&(*S), T_ADD);
				break;
			case T_SUB:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_EXPR_ID);
				parserStackPush(&(*S), T_SUB);
				break;
			case T_DIV:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_EXPR_ID);
				parserStackPush(&(*S), T_DIV);
				break;
			case T_MUL:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_EXPR_ID);
				parserStackPush(&(*S), T_MUL);
				break;
			case T_EQL:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_EXPR_ID);
				parserStackPush(&(*S), T_EQL);
				break;
			case T_NEQ:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_EXPR_ID);
				parserStackPush(&(*S), T_NEQ);
				break;
			case T_GT:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_EXPR_ID);
				parserStackPush(&(*S), T_GT);
				break;
			case T_LT:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_EXPR_ID);
				parserStackPush(&(*S), T_LT);
				break;
			case T_GTE:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_EXPR_ID);
				parserStackPush(&(*S), T_GTE);
				break;
			case T_LTE:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_EXPR_ID);
				parserStackPush(&(*S), T_LTE);
				break;
			case T_EOL:
				parserStackPop(&(*S));
				break;
			default:
				parserStackPop(&(*S));
				break;
		}
	}

	else if(S->a[S->last] == N_EXPR){
		switch((*token)->type){
			case T_EOL:
				parserStackPop(&(*S));
				break;
			case T_THEN:
				parserStackPop(&(*S));
				break;
			case T_DO:
				parserStackPop(&(*S));
				break;
			case T_EOF:
				parserStackPop(&(*S));
				break;
			default:
				*token = (*token)->nextToken;
				break;
		}
	}

	else fprintf(stderr, "Chyba: Neočekávaný token na stacku!\n");

	return;
}

void parserStackInit(tStack *S){
	S->top = 1;
	S->last = 0;
	S->a[0] = N_PROG; 		// Výchozí neterminál je <prog>
}	

void parserStackPush(tStack *S, tType type){
	if (S->top==MAXSTACK) 
		fprintf(stderr, "Interní chyba: Došlo k přetečení zásobníku!\n");
	else {
		S->a[S->top]=type;
		S->top++;
		S->last++;  
	}	
}

tType parserStackPop(tStack *S){
	if (S->top==0) {
		fprintf(stderr, "Interní chyba: Došlo k podtečení zásobníku!\n"); //TODO
		return(S->a[0]);	
	}	
	else {
		S->last--;
		return(S->a[S->top--]); 
	}	
}

bool parserStackEmpty(tStack *S){
	return(S->top==0);
}