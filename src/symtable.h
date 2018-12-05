/**
 * @file symtable.h
 * 
 * Tabulka symbolů (implementovaná jako binární strom)
 * 
 * IFJ Projekt 2018, Tým 13
 * 
 * @author <xforma14> Klára Formánková
 * @author <xlanco00> Jan Láncoš
 * @author <xsebel04> Vít Šebela
 * @author <xchalo16> Jan Chaloupka
 */

#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "common.h"

/**
 * Po kolika blocích se má alokovat zásobník
 */
#define SYMTABLE_STACK_CHUNK 100

/**
 * Typ identifikátoru 
 */
typedef enum{
	VAR,	//!< Identifikátor je proměnná
	FUNC	//!< Identifikátor je funkce
} sType;

/**
 * Data v uzlu
 */
typedef struct sData{
	sType type;					//!< Typ identifikátoru
	bool defined;				//!< Byl již identifikátor definovaný
	struct sTree *localFrame;	//!< Ukazatel na lokální tabulku funkce
	int params;					//!< Počet parametrů funkce
} *psData;

/**
 * Uzel stromu
 */
typedef struct sTree{
	char *key;			//!< Identifikátor
	struct sData *data;	//!< Data uzlu
	struct sTree *lptr;	//!< Levá větev stromu (menší klíč)
	struct sTree *rptr;	//!< Pravá větev stromu (větší klíč)
} *psTree;

/**
 * Zásobník procházení stromu
 */
typedef struct sStack{
	int size;	//!< Velikost zásobníku
	int top;	//!< Index nevyššího prvku
	psTree *S;	//!< Zásobník ukazatelů na položky
} *psStack;

/**
 * Inicializuje tabulku symbolů (binární vyhledávací strom) 
 * 
 * @param tree Ukazatel na strom pro inicializaci
 */
void symTabInit(psTree *tree);

/**
 * Vloží do stromu nový uzel s hodnotou data
 * 
 * @param tree Strom, do kterého se bude vkládat
 * @param key Klíč uzlu, podle kterého se bude vyhledávat ve stromě
 * @param data Data pro vložení do stromu
 */
void symTabInsert(psTree *tree, char *key, psData data);

/**
 * Podle klíče vyhledá uzel ve stromě 
 * 
 * @param tree Strom, ve kterém se bude vyhledávat
 * @param key Klíč uzlu, podle kterého se bude vyhledávat ve stromě
 * @return psData Vrací data nalezeného uzlu
 */
psData symTabSearch(psTree *tree, char *key);

/**
 * Projde levou stranu stromu a po cestě definuje proměnné
 * 
 * @param tree Strom proměnných
 * @param stack Zásobník ukazatelů na položky
 */
void symTabLefmostPre(psTree tree, psStack stack);

/**
 * Zadefinuje všechny proměnné ve stromu a inicializuje na nil
 * 
 * @param tree Strom proměnných
 */
void symTabDefvarPre(psTree tree);

/**
 * Zruší celý strom (tabulku symbolů)
 * 
 * @param tree Strom pro zrušení
 */
void symTabDispose(psTree *tree);

/**
 * Inicializace zásobníku
 * 
 * @param stack Ukazatel na zásobník
 */
void symStackInit(psStack *stack);

/**
 * Vloží položku na vrchol zásobníku
 * 
 * @param stack Ukazatel na zásobník
 * @param tree Položka, která se má vložit na zásobník
 */
void symStackPush(psStack stack, psTree tree);

/**
 * Vyjme položku z vrcholu zásobníku (NULL, pokud je zásobník prázdný)
 * 
 * @param stack Ukazatel na zásobník
 * @return psTree Položka, která byla na vrcholu zásobníku 
 */
psTree symStackPop(psStack stack);

/**
 * Uvolní zásobník z paměti
 * 
 * @param stack Ukazatel na zásobník
 */
void symStackDispose(psStack *stack);