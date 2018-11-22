#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "symtable.h"


void symTabInit(psTree *tree){
	if(*tree != NULL) 
		*tree = NULL;
}

bool symTabInsert(psTree *tree, char *key, psData data){
	if(tree == NULL) return false;

	psTree *node = tree;
	while(*node != NULL){
		int cmp = strcmp(key, (*node)->key);
		
		if(cmp < 0) 
			node = &(*node)->lptr;
		else if(cmp > 0) 
			node = &(*node)->rptr;
		else 
			return false;
	}

	psTree newTree = malloc(sizeof(struct sTree));
	// TODO Ošetřit malloc

	newTree->data = data;
	newTree->key = key;
	newTree->lptr = NULL;
	newTree->rptr = NULL;

	*node = newTree;

	return true;
}

psData symTabSearch(psTree *tree, char *key){
	if(tree == NULL) return false;

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