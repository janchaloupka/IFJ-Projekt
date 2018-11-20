#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "symtable.h"


//Inicializace seznamu stromu
void listInit(TSymTab **table){
	*table = malloc(sizeof(TSymTab));
	(*table)->first = NULL;
	(*table)->next = NULL;
}

//Pridani noveho stromu do seznamu
void listInsert(TSymTab **table){
	TSymTab *newSymTab = malloc(sizeof(TSymTab));
	newSymTab->first = NULL;
	newSymTab->next = (*table);
	*table = newSymTab;
}

//Vlozeni hodnot do stromu 
bool symTabInsert(TSymTab *table, char *key, TData *newData){
	if(table == NULL)
		return false;
	
	//pokud je seznam prazdny
	if(table->first == NULL){
		//novy strom
		table->first = malloc(sizeof(TNode));
		if(table->first == NULL)
			return false;
		//prirazeni
		memset(table->first, 0, sizeof(TNode));
		table->first->lptr = NULL;
		table->first->rptr = NULL;
		
		//novy klic pro uzel
		table->first->key = malloc(sizeof(char) * strlen(key) + 1);
		strcpy(table->first->key, key);
		
		//nova data pro uzel
		table->first->data = malloc(sizeof(TData));
		memcpy(table->first->data, newData, sizeof(TData));
		
		return true;
	}
	//pokud strom neni prazdny
	
	int diff; //rozdil klicu
	TNode *currNode = table->first;
	
	while(currNode){
		diff = strcmp(key, currNode->key);
		//diff == 0, klice jsou stejny
		if(diff == 0){
			//zkopiruju data
			memcpy(currNode, newData, sizeof(TData));
			return true;
		}
		
		//klic z argumentu je vetsi nez aktualni klic
		else if(diff > 0){
			if(currNode->rptr == NULL){
				currNode->rptr = malloc(sizeof(TNode));
				memset(currNode->rptr, 0, sizeof(TNode));
				currNode = currNode->rptr;
				break;
			}
			currNode = currNode->rptr;
		}
		
		//klic z argumentu je mensi nez aktualni klic
		else if(diff < 0){
			if(currNode->lptr == NULL){
				currNode->lptr = malloc(sizeof(TNode));
				memset(currNode->lptr, 0, sizeof(TNode));
				currNode = currNode->lptr;
				break;	
			}
			currNode = currNode->lptr;
		}
	}
	//nova data pro uzel
	currNode->data = malloc(sizeof(TData));
	memcpy(currNode->data, newData, sizeof(TData));
	
	//novy klic pro uzel
	currNode->key = malloc(sizeof(char) * strlen(key + 1));
	strcpy(currNode->key, key);
	
	return true;
}

//Vyhleda strom s danym klicem
TData *symTabSearch(TSymTab *table, char *key){
	if(table == NULL || key == NULL)
		return NULL;
	
	TNode *node;
	
	while(table){
		node = table->first;
		
		while(node){
			int diff = strcmp(key, node->key);
			if(diff > 0)
				node = node->rptr;
			else if (diff < 0)
				node = node->lptr;
			else
				return node->data;
		}
		table = table->next;
	}
	return NULL;
}

//vlozi novy parametr do seznamu parametru
bool paramInsert(TParam **list, TParam *newParam){
	TParam *currParam = (*list);
	TParam *prevParam = NULL;
	
	if(newParam == NULL)
		return false;
	
	TParam *item = malloc(sizeof(TParam));
	memcpy(item, newParam, sizeof(TParam));
	if(currParam == NULL){
		(*list) = item;
		return true;
	}
	
	while(currParam){
		prevParam = currParam;
		currParam = currParam->next;
	}
	prevParam->next = item; 
	return true;
}

//odstrani seznam paramentru
void paramDispose(TParam *parameters){
	TParam *curr;
	
	while(parameters){
		curr = parameters;
		parameters = parameters->next;
		free(curr->id);
		free(curr);
	}
}

//kontrola jestli byly funkce definovany
void defCheck(TNode *node){
	if(node == NULL)
		return;
	
	TData *currData = node->data;
	if(node->lptr == NULL && node->rptr == NULL){
		if(currData->defined == false){
			//ten error ma by asi zpracovanej jinak----------!!
			fprintf(stderr, "Function was not defined.");
		}	
	}
	else{
		if(node->rptr)
			defCheck(node->rptr);
		if(node->lptr)
			defCheck(node->lptr);
		
		if(currData->defined == false){
			//tenhle taky-----------------------------!!
			fprintf(stderr, "Function was not defined.");
		}
	}
}

//odstraneni celeho stromu
void symTabDispose(TNode *node){
	if(node == NULL)
		return;
	
	if(node->lptr == NULL && node->rptr == NULL){
		free(node->key);
		TData *nD = node->data;
		paramDispose(nD->params);
		free(node->data);
		free(node);
		node = NULL;
	}
	else{
		if(node->lptr)
			symTabDispose(node->lptr);
		if(node->rptr)
			symTabDispose(node->rptr);
		
		free(node->key);
		TData *nD = node->data;
		paramDispose(nD->params);
		free(node->data);
		free(node);
	}
}

//Odstraneni stromu z prvniho mista v seznamu
bool firstDispose(TSymTab **table){
	if(table == NULL || (*table) == NULL)
		return false;
	
	symTabDispose((*table)->first);
	TSymTab *curr = (*table)->next;
	free(*table);
	*table = curr;
	
	return true;
}

//Odstrani cely seznam stromu
void listDispose(TSymTab **table){
	while(firstDispose (table));
}

