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

#define MAXSTACK 100                      /* maximální počet prvků v zásobníku */

/**
 * Pomocný zásobník 
 */
typedef struct {
    tType a[MAXSTACK];
    int top;					// Index prvního volného místa
	int last;					// Index naposledy vloženého prvku
} tStack;

/**
 * Vlastní tělo parseru, zatím má pouze kontrolovat syntax
 * 
 * @param List Výstup lexikálního analyzátorů, obousměrně provázaný seznam tokenů
 */
void parser(pToken *List);

/**
 * Test identifikátoru, jestli patří funkci, zatím neimplementováno
 * 
 */
void parserTestIDFN();

/**
 * Je-li na stacku terminál, porovná jej se vstupním tokenem
 * 
 * @param S Zásobník terminálů/neterminálů určených ke zpracování
 * @param token Momentálně zpracovávaný token
 * @return bool Jsou-li typy ekvivalentní, vrací true
 */
bool parserCompare(tStack S, pToken token);

/**
 * Je-li na zásobníku neterminál, podle tabulky LL gramatiky určí, jak jej dále derivovat, části vloží v pořadí na zásobník
 * 
 * @param S Ukazatel na zásobník terminálů/neterminálů určených ke zpracování
 * @param token Momentálně zpracovávaný token
 */
void parserExpand(tStack *S, pToken token);

/**
 * Inicializuje zásobník, na první místo vloží type N_PROG jakožto výchozí stav
 * 
 * @param S Ukazatel na zásobník
 */
void parserStackInit (tStack *S);

/**
 * Pushne terminál/neterminál na zásobník
 * 
 * @param S Ukazatel na zásobník
 * @param type Ukazatel na zásobník
 */
void parserStackPush (tStack *S, tType type);

/**
 * Popne zásobník
 * 
 * @param S Ukazatel na zásobník
 * @return tType Hodnota na vrcholu zásobníku
 */
tType parserStackPop (tStack *S);

/**
 * Vrací true pokud je zásobník prázdný
 * 
 * @param S Ukazatel na zásobník
 * @return bool Vrací true pokud je zásobník prázdný
 */
bool parserStackEmpty (tStack *S);
