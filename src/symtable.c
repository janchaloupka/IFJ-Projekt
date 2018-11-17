/**
 * symtab.c
 *
 * Tabulka symbolu implementovana pomoci BVS
 *
 */
 
#include "symtab.h"


 //inicializace stromu
 void BSTInit(tBSTNodePtr *rootPtr){
	if(rootPtr == NULL)
		return;
	
	*rootPtr = NULL;
 }
 
 
 //Funkce vyhledá uzel v BVS s klíčem K
 int BSTSearch (tBSTNodePtr RootPtr, char K, int *Cont){
	if(RootPtr == NULL)
		return FALSE;
	
	//pokud je hlavni uzel hledanym uzlem 
	if(RootPtr->Key == K){
		*Cont = RootPtr->BSTNodeCont;
		return TRUE;
	}
	
	//jinak hledam vlevo
	if(K < RootPtr->Key)
		return BSTSearch(RootPtr->LPtr, K, Cont);
	
	//jinak hledam vravo
	else
		return BSTSearch(RootPtr->RPtr, K, Cont);
	
	return FALSE;
 }
 
 //Vloží do stromu RootPtr hodnotu newCont s klíčem K
 void BSTInsert (tBSTNodePtr* RootPtr, char K, int newCont){
	 if(RootPtr == NULL)
		return;
	
	//pokud je strom prazdny
	if(*RootPtr == NULL){
		//alokuju novy
		*RootPtr = malloc(sizeof(struct tBSTNode));
		if(*RootPtr == NULL)
			return;
		//priradim
		(*RootPtr)->Key = K;
		(*RootPtr)->BSTNodeCont = newCont;
		(*RootPtr)->LPtr = NULL;
		(*RootPtr)->RPtr = NULL;
		return;
	}
	//klic je stejny
	if((*RootPtr)->Key == K)
		(*RootPtr)->BSTNodeCont = newCont;
	//vlozim vlevo
	else if ((*RootPtr)->Key > K)
		BSTInsert(&(*RootPtr)->LPtr,K,newCont);
	//vlozim vpravo
	else
		BSTInsert(&(*RootPtr)->RPtr,K,newCont);
 }
 
 // Pomocná funkce pro vyhledání, přesun a uvolnění nejpravějšího uzlu.
 void ReplaceByRightmost (tBSTNodePtr PtrReplaced, tBSTNodePtr *RootPtr) {
	//pokud strom neni prazdny
	if(*RootPtr){
		tBSTNodePtr node = NULL;
		//pokud vpravo neni NULL, volej funkci znova
		if ((*RootPtr)->RPtr)
			ReplaceByRightmost(PtrReplaced,&(*RootPtr)->RPtr);
		//nejpravejsi nalezen
		else{
			PtrReplaced->BSTNodeCont = (*RootPtr)->BSTNodeCont;
			PtrReplaced->Key = (*RootPtr)->Key;
			node = *RootPtr;
			(*RootPtr) = node->LPtr;
			free(node);
		}
	}
}
 
 //Zruší uzel stromu, který obsahuje klíč K
 void BSTDelete (tBSTNodePtr *RootPtr, char K) 		{
	if(RootPtr == NULL || *RootPtr == NULL)
		return;
	
	//pomocny uzel
	tBSTNodePtr node;
	
	//klic je vetsi, jdu doprava
	if ((*RootPtr)->Key < K)
		BSTDelete(&(*RootPtr)->RPtr,K);
	//klic je mensi, jdu doleva
	else if((*RootPtr)->Key > K)
		BSTDelete(&(*RootPtr)->LPtr, K);
	//klic nalezen
	else{
		node = *RootPtr;
		//vpravo je NULL, do RootPtr priradim levy
		if (node->RPtr == NULL){
			*RootPtr = node->LPtr;
			free(node);
		}
		//vlevo je NULL, do RootPtr priradim pravy
		else if (node->LPtr == NULL){
			*RootPtr = node->RPtr;
			free(node);
		}
		//ma oba podstromy, ruseny uzel je nahrazen nejpravejsim uzlem leveho podstromu
		else ReplaceByRightmost((*RootPtr), &(*RootPtr)->LPtr);
	}
} 

//Zruší celý binární vyhledávací strom a korektně uvolní pamě
void BSTDispose (tBSTNodePtr *RootPtr) {	
	if(RootPtr == NULL || *RootPtr == NULL)
		return;
	
	BSTDispose(&(*RootPtr)->LPtr);
	BSTDispose(&(*RootPtr)->RPtr);
	free(*RootPtr);
	*RootPtr = NULL;
}