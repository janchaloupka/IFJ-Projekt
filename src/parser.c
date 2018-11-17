#include "parser.h"

void parser(pToken *List){

	pToken token = (*List);

	bool correct = true;

	tStack S;
	parserStackInit(&S);

	while(token != NULL){
		
		if(token->type == T_ID) parserTestIDFN(token); // Zatím nedělá nic, TODO

		if(S.a[S.last] > T_STRING){
			parserExpand(&S, &token);
		}

		else{
			correct = (parserCompare(S, token));
			parserStackPop(&S);
			token = (token)->nextToken;
		}

		if(!correct){
			printf("Chyba: Neočekávaný symbol!\n");
			return;
		}
	}
}

bool parserCompare(tStack S, pToken token){
	return (S.a[S.last] == token->type);
}

void parserExpand(tStack *S, pToken *token){
	if(S->a[S->last] == N_PROG){
		switch((*token)->type){
			case T_ID:
				if((*token)->nextToken->type != T_ASSIGN){
					parserStackPop(&(*S));
					parserStackPush(&(*S), N_EXPR);
				}
				else{
					parserStackPop(&(*S));
					parserStackPush(&(*S), N_PROG);
					parserStackPush(&(*S), T_EOL);
					parserStackPush(&(*S), N_BODY);
				}
				break;
			case T_IDFN:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_PROG);
				parserStackPush(&(*S), T_EOL);
				parserStackPush(&(*S), N_BODY);
				break;
			case T_EOF:
				parserStackPop(&(*S));
				parserStackPush(&(*S), T_EOF);
				break;
			case T_DEF:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_PROG);
				parserStackPush(&(*S), T_EOL);
				parserStackPush(&(*S), N_DEFUNC);
				break;
			case T_IF:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_PROG);
				parserStackPush(&(*S), T_EOL);
				parserStackPush(&(*S), N_BODY);
				break;
			case T_WHILE:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_PROG);
				parserStackPush(&(*S), T_EOL);
				parserStackPush(&(*S), N_BODY);
				break;
			default:
				printf("Chyba: Neočekávaný symbol!\n");
				break;
		}
	}

	else if(S->a[S->last] == N_BODY){
		switch((*token)->type){
			case T_ID:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_BODY);
				parserStackPush(&(*S), N_NEWL);
				parserStackPush(&(*S), N_DEFVAR);
				break;
			case T_IDFN:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_BODY);
				parserStackPush(&(*S), N_NEWL);
				parserStackPush(&(*S), N_CALFUNC);
				break;
			case T_END:
				parserStackPop(&(*S));
				break;
			case T_IF:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_BODY);
				parserStackPush(&(*S), N_NEWL);
				parserStackPush(&(*S), N_IF);
				break;
			case T_ELSE:
				parserStackPop(&(*S));
				break;
			case T_WHILE:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_BODY);
				parserStackPush(&(*S), N_NEWL);
				parserStackPush(&(*S), N_WHILE);				
				break;
			case T_EOF:
				parserStackPop(&(*S));
				break;
			default:
				printf("Chyba: Neočekávaný symbol!\n");
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
			case T_EOF:
				parserStackPop(&(*S));
				break;
			default:
				printf("Chyba: Neočekávaný symbol!\n");
				break;
		}
	}

	else if(S->a[S->last] == N_NEWLN){
		switch((*token)->type){
			case T_ID:
				parserStackPop(&(*S));
				break;
			case T_IDFN:
				parserStackPop(&(*S));
				break;
			case T_EOL:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_NEWLN);
				break;
			case T_IF:
				parserStackPop(&(*S));
				break;
			case T_WHILE:
				parserStackPop(&(*S));
				break;
			case T_ELSE:
				parserStackPop(&(*S));
				break;
			case T_END:
				parserStackPop(&(*S));
				break;
			default:
				printf("Chyba: Neočekávaný symbol!\n");
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
			case T_STRING:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_PARSN);
				parserStackPush(&(*S), T_STRING);
				break;
			case T_RBRCKT:
				parserStackPop(&(*S));
				break;
			default:
				printf("Chyba: Neočekávaný symbol!\n");
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
				printf("Chyba: Neočekávaný symbol!\n");
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
				parserStackPush(&(*S), T_IDFN);
				parserStackPush(&(*S), T_DEF);
				break;
			default:
				printf("Chyba: Neočekávaný symbol!\n");
				break;
		}
	}

	else if(S->a[S->last] == N_CALFUNC){
		switch((*token)->type){
			case T_IDFN:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_FUNC);
				parserStackPush(&(*S), T_IDFN);
				break;
			default:
				printf("Chyba: Neočekávaný symbol!\n");
				break;
		}
	}

	else if(S->a[S->last] == N_FUNC){
		switch((*token)->type){
			case T_ID:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_PARS);
				break;
			case T_EOL:
				parserStackPop(&(*S));
				break;
			case T_STRING:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_PARS);
				break;
			case T_LBRCKT:
				parserStackPop(&(*S));
				parserStackPush(&(*S), T_RBRCKT);
				parserStackPush(&(*S), N_PARS);
				parserStackPush(&(*S), T_LBRCKT);
				break;
			default:
				printf("Chyba: Neočekávaný symbol!\n");
				break;
		}
	}

	else if(S->a[S->last] == N_DEFVAR){
		switch((*token)->type){
			case T_ID:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_VAR);
				parserStackPush(&(*S), T_ASSIGN);
				parserStackPush(&(*S), T_ID);
				break;
			default:
				printf("Chyba: Neočekávaný symbol!\n");
				break;
		}
	}

	else if(S->a[S->last] == N_VAR){
		switch((*token)->type){
			case T_INTEGER:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_EXPR);
				break;
			case T_DOUBLE:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_EXPR);
				break;
			case T_DOUBLE_E:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_EXPR);
				break;
			case T_ID:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_EXPR);
				break;
			case T_IDFN:
				parserStackPop(&(*S));
				parserStackPush(&(*S), N_CALFUNC);
				break;
			default:
				printf("Chyba: Neočekávaný symbol!\n");
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
				printf("Chyba: Neočekávaný symbol!\n");
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
				printf("Chyba: Neočekávaný symbol!\n");
				break;
		}
	}

	else if(S->a[S->last] == N_EXPR){
		switch((*token)->type){
			case T_INTEGER:
				*token = (*token)->nextToken;
				break;
			case T_DOUBLE:
				*token = (*token)->nextToken;
				break;
			case T_DOUBLE_E:
				*token = (*token)->nextToken;
				break;
			case T_ID:
				*token = (*token)->nextToken;
				break;
			case T_RBRCKT:
				*token = (*token)->nextToken;
				break;
			case T_LBRCKT:
				*token = (*token)->nextToken;
				break;
			case T_ADD:
				*token = (*token)->nextToken;
				break;
			case T_SUB:
				*token = (*token)->nextToken;
				break;
			case T_MUL:
				*token = (*token)->nextToken;
				break;
			case T_DIV:
				*token = (*token)->nextToken;
				break;
			case T_EQL:
				*token = (*token)->nextToken;
				break;
			case T_NEQ:
				*token = (*token)->nextToken;
				break;
			case T_LT:
				*token = (*token)->nextToken;
				break;
			case T_GT:
				*token = (*token)->nextToken;
				break;
			case T_LTE:
				*token = (*token)->nextToken;
				break;
			case T_GTE:
				*token = (*token)->nextToken;
				break;
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
				printf("Chyba: Neočekávaný symbol!\n");
				break;
		}
	}

	else printf("Chyba: Neočekávaný token na stacku!\n");

	return;
}

void parserStackInit(tStack *S){
	S->top = 1;
	S->last = 0;
	S->a[0] = N_PROG; 		//výchozí neterminál je <prog>
}	

void parserStackPush(tStack *S, tType type){
	if (S->top==MAXSTACK) 
		printf("Interní chyba: Došlo k přetečení zásobníku!\n");
	else {
		S->a[S->top]=type;
		S->top++;
		S->last++;  
	}	
}

tType parserStackPop(tStack *S){
	if (S->top==0) {
		printf("Interní chyba: Došlo k podtečení zásobníku!\n");
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

void parserTestIDFN(pToken *id){
	//TODO - zjistit, které identifikátory patří funkcím
	id = id;
	return;
}