#include "expressions.h"


eTerm exprConvTypeToTerm(tType tokenType){
	switch(tokenType){
		case T_MUL:
		case T_DIV:
			return E_MULDIV;
		case T_ADD:
		case T_SUB:
			return E_ADDSUB;
		case T_LT:
		case T_GT:
		case T_LTE:
		case T_GTE:
			return E_LTGT;
		case T_EQL:
		case T_NEQ:
			return E_EQL;
		case T_LBRCKT:
			return E_LB;
		case T_RBRCKT:
			return E_RB;
		case T_INTEGER:
		case T_FLOAT:
		case T_STRING:
		case T_ID:
		case T_NIL:
			return E_VAL;
		default:
			return E_$;
	}
}


eRelation exprGetRelation(eTerm currTerm, eTerm newTerm){
	switch(currTerm){
		case E_MULDIV:
			if(newTerm == E_LB || newTerm == E_VAL)
				return OPEN;
			return CLOSE;
		case E_ADDSUB:
			if(newTerm == E_LB || newTerm == E_VAL || newTerm == E_MULDIV)
				return OPEN;
			return CLOSE;
		case E_LTGT:
			if(newTerm == E_EQL || newTerm == E_RB || newTerm == E_$) 
				return CLOSE;
			else if(newTerm == E_LTGT) return ERROR;
			return OPEN;
		case E_EQL:
			if(newTerm == E_RB || newTerm == E_$)
				return CLOSE;
			else if(newTerm == E_EQL) return ERROR;
			return OPEN;
		case E_LB:
			if(newTerm == E_RB) return EQUAL;
			else if(newTerm == E_$) return ERROR;
			return OPEN;
		case E_RB:
		case E_VAL:
			if(newTerm == E_LB || newTerm == E_VAL)
				return ERROR;
			return CLOSE;
		case E_$:
			if(newTerm == E_RB || newTerm == E_$)
				return ERROR;
			return OPEN;
	}

	return ERROR;
}

int exprParse(pToken *token, peNode *nodeTree){
	peStack stack;
	exprStackInit(&stack);

	while(true){
		eTerm stackT, newT;

		int termPos = exprStackFirstTermPos(stack);
		if(termPos < 0) stackT = E_$;
		else stackT = exprConvTypeToTerm(stack->s[termPos]->val.token->type);
		newT = exprConvTypeToTerm((*token)->type);
		eRelation rel = exprGetRelation(stackT, newT);

		peNode node;
		switch(rel){
			case OPEN:
				exprStackInsertOpen(stack, termPos + 1);
				node = malloc(sizeof(struct eNode));
				node->type = NT_TOKEN;
				node->val.token = *token;

				exprStackPush(stack, node);
				*token = (*token)->nextToken;
				break;
			case CLOSE:
				if(1 == exprStackToRule(stack)){
					fprintf(stderr, "Nesouhlasí gramatika expr\n");
					return 2;
				}
				break;
			case EQUAL:
				node = malloc(sizeof(struct eNode));
				node->type = NT_TOKEN;
				node->val.token = *token;

				exprStackPush(stack, node);
				*token = (*token)->nextToken;
				break;
			case ERROR:
				if(stackT == newT && stackT == E_$){
					if(nodeTree != NULL)
						*nodeTree = stack->s[stack->top];
					return 0;
				}
				fprintf(stderr, "Nastala chyba při persování expr: %d->%d\n", stackT, newT);
				return 2;
		}
	}
	return 0;
}

void exprStackInit(peStack *stack){
	if(stack == NULL) return;
	*stack = malloc(sizeof(struct eStack));
	(*stack)->size = EXPR_STACK_CHUNK_SIZE;
	(*stack)->s = malloc(sizeof(peNode) * EXPR_STACK_CHUNK_SIZE);
	(*stack)->top = -1;
}

void exprStackPush(peStack stack, peNode node){
	stack->top++;
	
	if(stack->top >= stack->size){
		stack->size += EXPR_STACK_CHUNK_SIZE;
		stack->s = realloc(stack->s, sizeof(peNode) * stack->size);
	}

	stack->s[stack->top] = node;
}

peNode exprStackPop(peStack stack){
	if(stack->top < 0) return NULL;

	stack->top--;
	return stack->s[stack->top + 1];
}

int	exprStackFirstTermPos(peStack stack){
	int pos = stack->top;
	for(; pos >= 0; pos--){
		if(stack->s[pos]->type == NT_TOKEN) break;
	}

	return pos;
}

void exprStackInsertOpen(peStack stack, int pos){
	exprStackPush(stack, NULL);
	for(int i = stack->top - 1; i >= pos; i--){
		stack->s[i + 1] = stack->s[i];
	}

	peNode node = malloc(sizeof(struct eNode));
	node->type = NT_RELATION;
	node->val.relation = OPEN;
	stack->s[pos] = node;
}

int exprStackToRule(peStack stack){

	peRule newRule = malloc(sizeof(struct eRule));
	peNode newNode = malloc(sizeof(struct eNode));
	newNode->type = NT_RULE;
	newNode->val.rule = newRule;

	peNode node = exprStackPop(stack);
	if(node == NULL){
		return 1; // Chyba
	}

	if(node->type == NT_TOKEN){
		if(node->val.token->type == T_RBRCKT){
			node = exprStackPop(stack);
			if(node->type != NT_RULE){
				return 1; // Chyba
			}

			peNode nnode = exprStackPop(stack);
			if(nnode->type != NT_TOKEN || nnode->val.token->type != T_LBRCKT){
				return 1; // Chyba
			}
			
			nnode = exprStackPop(stack);
			if(nnode->type != NT_RELATION){
				return 1; // Chyba
			}

			exprStackPush(stack, node);			
			return 0;
		}else if(exprConvTypeToTerm(node->val.token->type) != E_VAL){
			return 1; // Chyba
		}
	}
	newRule->rNode = node;

	
	node = exprStackPop(stack);
	if(node->type == NT_RELATION){
		// Hotovo
		newRule->type = RT_VAL;
		exprStackPush(stack, newNode);
		return 0;
	}else if(node->type == NT_RULE){
		return 1; // Chyba
	}
	eRuleType type;
	switch(node->val.token->type){
		case T_ADD:
			type = RT_ADD; break;
		case T_SUB:
			type = RT_SUB; break;
		case T_MUL:
			type = RT_MUL; break;
		case T_DIV:
			type = RT_DIV; break;
		case T_EQL:
			type = RT_EQL; break;
		case T_NEQ:
			type = RT_NEQ; break;
		case T_LT:
			type = RT_LT; break;
		case T_GT:
			type = RT_GT; break;
		case T_LTE:
			type = RT_LTE; break;
		case T_GTE:
			type = RT_GTE; break;
		default: 
			break;
	}
	newRule->type = type;
	free(node);


	node = exprStackPop(stack);
	if(node->type == NT_RELATION){
		return 1; // Chyba
	}else if(node->type == NT_TOKEN){
		if(exprConvTypeToTerm(node->val.token->type) != E_VAL){
			return 1; // Chyba
		}
	}
	newRule->lNode = node;


	node = exprStackPop(stack);
	if(node->type != NT_RELATION){
		return 1; // Chyba
	}
	exprStackPush(stack, newNode);

	return 0;
}