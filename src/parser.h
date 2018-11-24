/**
 * @file parser.h
 * 
 * Syntaktický analyzátor
 * 
 * @version 0.1
 * @date 2018-11-16
 */

#pragma once
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "scanner.h"
#include "symtable.h"

#define STACK_CHUNK_SIZE 1000                      // Velikost alokační jednotky zásobníku

/**
 * Pomocný zásobník 
 */
typedef struct {
    tType *a;
    int size;
    int top;					// Index prvního volného místa
	int last;					// Index naposledy vloženého prvku
} tStack;

typedef struct tNode	{                     // Uzel stromu
    tType type;
    struct tNode **tChild;                    // Pole ukazatelů na tNode
} *pNode;                                     // Ukazatel na uzel stromu

/**
 * Vlastní tělo parseru, zatím má pouze kontrolovat syntax
 * 
 * @param List Výstup lexikálního analyzátorů, obousměrně provázaný seznam tokenů
 * @return int 99 po interní chybě, 2 po chybě překladu, jinak 0
 */
int parser(pToken *List);

/**
 * Je-li na stacku terminál, porovná jej se vstupním tokenem
 * 
 * @param S Zásobník terminálů/neterminálů určených ke zpracování
 * @param token Momentálně zpracovávaný token
 * @return bool Jsou-li typy ekvivalentní, vrací true
 */
int parserCompare(tStack S, pToken token);

/**
 * Je-li na zásobníku neterminál, podle tabulky LL gramatiky určí, jak jej dále derivovat, části vloží v pořadí na zásobník
 * 
 * @param S Ukazatel na zásobník terminálů/neterminálů určených ke zpracování
 * @param token Momentálně zpracovávaný token
 * @return int Jedna po chybě na stacku, jinak nula
 */
int parserExpand(tStack *S, pToken *token, int *correct, int *error);

/**
 * TODO
 * 
 * 
 * 
 * 
 */
int parserSemanticsPreRun(pToken *token, psTree *main_tree);

/**
 * TODO
 * 
 * 
 * 
 * 
 */
psData parserSemanticsInitData(sType type, struct sTree *localFrame, int params);

/**
 * TODO
 * 
 * 
 * 
 * 
 */
void parserSemanticsInit(psTree *main_table);

/**
 * TODO
 * 
 * 
 * 
 * 
 */
int parserError(int error, int int_error, pToken *prevToken, tStack *S);

/**
 * TODO
 * 
 * 
 * 
 * 
 */
void parserSemanticsInFunc(bool *inFunc, int *inAux, pToken semanToken);

/**
 * Inicializuje zásobník, na první místo vloží type N_PROG jakožto výchozí stav
 * 
 * @param S Ukazatel na zásobník
 * @return int Jedna po chybě mallocu, jinak nula
 */
int parserStackInit (tStack *S);

/**
 * Pushne terminál/neterminál na zásobník
 * 
 * @param S Ukazatel na zásobník
 * @param type Ukazatel na zásobník
 * @param error Dojde-li k chybě mallocu, sem se vloží jedna
 */
void parserStackPush (tStack *S, tType type, int *error);

/**
 * Popne zásobník
 * 
 * @param S Ukazatel na zásobník
 * @param error Dojde-li k chybě reallocu, sem se vloží jedna
 * @return tType Hodnota na vrcholu zásobníku
 */
tType parserStackPop (tStack *S, int *error);

/**
 * Uvolní paměť zásobníku
 * 
 * @param S Ukazatel na zásobník
 */
void parserStackDelete(tStack *S);