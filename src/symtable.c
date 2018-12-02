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