/**
 * @file symtable.c
 * 
 * Tabulka symbolů (implementovaná jako binární strom)
 * 
 * IFJ Projekt 2018, Tým 13
 * 
 * @author <xforma04> Klára Formánková
 * @author <xlanco00> Jan Láncoš
 * @author <xsebel04> Vít Šebela
 * @author <xchalo16> Jan Chaloupka
 */

#include "symtable.h"

void symTabInit(psTree *tree){
	if(*tree != NULL) 
		*tree = NULL;
}

void symTabInsert(psTree *tree, char *key, psData data){
	if(tree == NULL) return;

	psTree *node = tree;
	while(*node != NULL){
		int cmp = strcmp(key, (*node)->key);
		
		if(cmp < 0) 
			node = &(*node)->lptr;
		else if(cmp > 0) 
			node = &(*node)->rptr;
		else{
			free((*node)->data);
			(*node)->data = data;
			return;
		}
	}

	psTree newTree = safeMalloc(sizeof(struct sTree));

	newTree->data = data;
	newTree->key = key;
	newTree->lptr = NULL;
	newTree->rptr = NULL;

	*node = newTree;
}

psData symTabSearch(psTree *tree, char *key){
	if(tree == NULL) return NULL;

	while(*tree != NULL){
		int cmp = strcmp(key, (*tree)->key);
		
		if(cmp < 0) 
			tree = &(*tree)->lptr;
		else if(cmp > 0) 
			tree = &(*tree)->rptr;
		else 
			return (*tree)->data;
	}
	
	return NULL;
}

void symTabLefmostPre(psTree tree, psStack stack){
	while(tree != NULL){
		symStackPush(stack, tree);
		printf("DEFVAR LF@%s\nMOVE LF@%s nil@nil\n", tree->key, tree->key);
		tree = tree->lptr;
	}
}

void symTabDefvarPre(psTree tree){
	psStack stack;
	symStackInit(&stack);
	symTabLefmostPre(tree, stack);
	while(stack->top >= 0){
		tree = symStackPop(stack);
		symTabLefmostPre(tree->rptr, stack);
	}
	symStackDispose(&stack);
}

void symTabDispose(psTree *tree){
	if(tree == NULL || (*tree) == NULL) return;
	
	// Průchod stromem
	symTabDispose(&(*tree)->lptr);
	symTabDispose(&(*tree)->rptr);
	
	// Uvolnění klíče
	// free((*tree)->key);

	// Uvolnění dat
	if((*tree)->data->localFrame != NULL)
		symTabDispose(&(*tree)->data->localFrame);
	free((*tree)->data);
	
	// Uvolnění uzlu
	free(*tree);
	*tree = NULL;
}

void symStackInit(psStack *stack){
	if(stack == NULL) return;

	*stack = safeMalloc(sizeof(struct sStack));
	(*stack)->top = -1;
	(*stack)->size = 0;
	(*stack)->S = NULL;
}

void symStackPush(psStack stack, psTree tree){
	if(stack == NULL) return;
	
	stack->top++;
	if(stack->size <= stack->top){
		stack->size += SYMTABLE_STACK_CHUNK;
		stack->S = safeRealloc(stack->S, sizeof(psTree) * stack->size);
	}

	stack->S[stack->top] = tree;
}

psTree symStackPop(psStack stack){
	if(stack == NULL || stack->top < 0)
		return NULL;

	stack->top--;
	return stack->S[stack->top + 1];
}

void symStackDispose(psStack *stack){
	if(stack == NULL) return;

	free((*stack)->S);
	free(*stack);
	*stack = NULL;
}