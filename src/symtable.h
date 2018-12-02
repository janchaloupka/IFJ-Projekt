/**
 * @file symtable.h
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

#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "common.h"

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
 * Zruší celý strom (tabulku symbolů)
 * 
 * @param tree Strom pro zrušení
 */
void symTabDispose(psTree *tree);
