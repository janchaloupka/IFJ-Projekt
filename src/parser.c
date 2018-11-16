#include "parser.h"

void parser(pToken *List){

	pToken token = (*List);

	bool correct = true;

	while(token != NULL){
		
		if(token->type == T_ID) parserTestIDFN(token); // Zatím nedělá nic, TODO

		tStack S;
		parserStackInit(&S);

		if(S.a[S.last] > T_STRING){
			parserExpand(&S, token);
		}

		else{
			correct = (parserCompare(S, token));
			parserStackPop(&S);
		}

		if(!correct){
			printf("Chyba: Neočekávaný symbol!\n");
			return;
		}

		token = (token)->nextToken;
	}
}

bool parserCompare(tStack S, pToken token){
	return (S.a[S.last] == token->type);
}

void parserExpand(tStack *S, pToken token){
	if(S->a[S->last] == N_PROG){
		switch(token->type){
			case T_ID:
				break;
			case T_IDFN:
				break;
			case T_EOF:
				break;
			case T_DEF:
				break;
			case T_IF:
				break;
			case T_WHILE:
				break;
			default:
				printf("Chyba: Neočekávaný symbol!\n");
				break;
		}
	}

	else if(S->a[S->last] == N_BODY){
		switch(token->type){
			case T_ID:
				break;
			case T_IDFN:
				break;
			case T_END:
				break;
			case T_IF:
				break;
			case T_ELSE:
				break;
			case T_WHILE:
				break;
			default:
				printf("Chyba: Neočekávaný symbol!\n");
				break;
		}
	}

	else if(S->a[S->last] == N_NEWL){
		switch(token->type){
			case T_EOL:
				break;
			default:
				printf("Chyba: Neočekávaný symbol!\n");
				break;
		}
	}

	else if(S->a[S->last] == N_NEWLN){
		switch(token->type){
			case T_ID:
				break;
			case T_IDFN:
				break;
			case T_EOL:
				break;
			case T_IF:
				break;
			case T_WHILE:
				break;
			default:
				printf("Chyba: Neočekávaný symbol!\n");
				break;
		}
	}

	else if(S->a[S->last] == N_PARS){
		switch(token->type){
			case T_ID:
				break;
			case T_RBRCKT:
				break;
			default:
				printf("Chyba: Neočekávaný symbol!\n");
				break;
		}
	}

	else if(S->a[S->last] == N_PARSN){
		switch(token->type){
			case T_COMMA:
				break;
			case T_RBRCKT:
				break;
			default:
				printf("Chyba: Neočekávaný symbol!\n");
				break;
		}
	}

	else if(S->a[S->last] == N_DEFUNC){
		switch(token->type){
			case T_DEF:
				break;
			default:
				printf("Chyba: Neočekávaný symbol!\n");
				break;
		}
	}

	else if(S->a[S->last] == N_CALFUNC){
		switch(token->type){
			case T_IDFN:
				break;
			default:
				printf("Chyba: Neočekávaný symbol!\n");
				break;
		}
	}

	else if(S->a[S->last] == N_FUNC){
		switch(token->type){
			case T_ID:
				break;
			case T_RBRCKT:
				break;
			default:
				printf("Chyba: Neočekávaný symbol!\n");
				break;
		}
	}

	else if(S->a[S->last] == N_DEFVAR){
		switch(token->type){
			case T_DEF:
				break;
			default:
				printf("Chyba: Neočekávaný symbol!\n");
				break;
		}
	}

	else if(S->a[S->last] == N_VAR){
		switch(token->type){
			case T_IDFN:
				break;
			default:
				printf("Chyba: Neočekávaný symbol!\n");
				break;
		}
	}

	else if(S->a[S->last] == N_IF){
		switch(token->type){
			case T_IF:
				break;
			default:
				printf("Chyba: Neočekávaný symbol!\n");
				break;
		}
	}

	else if(S->a[S->last] == N_WHILE){
		switch(token->type){
			case T_WHILE:
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
		S->top++;
		S->last++;  
		S->a[S->top]=type;
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