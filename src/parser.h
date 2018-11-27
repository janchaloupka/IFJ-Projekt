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
 * Pomocný zásobník k rekurzivnímu sestupu, probíhá na něm rozklad neterminálů
 */
typedef struct{
    tType *a;
    int size;
    int top;    // Index prvního volného místa
	int last;   // Index naposledy vloženého prvku
} SyntaxStack;

/**
 * Pomocný zásobník sémantických errorů 
 */
typedef struct SemanticsStack{
    struct Error *ErrorArray;
    int top;
} *pSemanticsStack;

/**
 * Sémantická chyba
 */
typedef struct Error{
    int error;  // Kód chyby
    int line;
    int col;
    char *name; // Název proměnné s chybou
} *pError; 


/**
 * Vlastní tělo parseru, v průběhu procházení token-listu zkontroluje syntax i sémantiku
 * 
 * @param List Výstup lexikálního analyzátorů, obousměrně provázaný seznam tokenů
 * @return int 99 po interní chybě, 2, 3, 4, 5, 6, 9 podle příslušného výskytu chyby, jinak 0
 */
int parser(pToken *List);




/******************************************************SYNTAX******************************************************************************/

/**
 * Je-li na stacku terminál, porovná jej se vstupním tokenem
 * 
 * @param S Zásobník terminálů/neterminálů určených ke zpracování
 * @param token Momentálně zpracovávaný token
 * @param error Ukazatel na integerovou error hodnotu, do které zapíše dvojku, dojde-li k syntaktickému erroru
 */
void parserSyntaxCompare(SyntaxStack S, pToken token, int *error);

/**
 * Je-li na zásobníku neterminál, podle tabulky LL gramatiky určí, jak jej dále derivovat, části vloží v pořadí na zásobník
 * 
 * @param S Ukazatel na zásobník terminálů/neterminálů určených ke zpracování
 * @param token Momentálně zpracovávaný token
 * @param error Ukazatel na integerovou error hodnotu, do které zapíše dvojku, dojde-li k syntaktickému erroru
 * @param internalError Ukazatel na integerovou error hodnotu, kterou předává stackovým funkcím, k zapsání selhání malloců
 * @param localTable Předává se funkci exprParse
 */
void parserSyntaxExpand(SyntaxStack *S, pToken *token, int *error, int *internalError, psTree localTable);

/**
 * Vyhodnocení chyb po každém novém cyklu, výstupní hodnota využita i jako návratová hodnota parseru (a potažmo celého programu)
 * 
 * @param error Hodnota udávající, zda-li již došlo k syntaktické chybě, a které
 * @param internalError Hodnota udávající, zda-li již došlo k interní chybě, a které
 * @param prevToken Hodnota tokenu, u kterého došlo k chybě (k výpisu)
 * @param S Zásobník rekurzivního sestupu, určen k výpisu lokace chyby
 * @return Návratová hodnota užita jako návratová hodnota parseru
 */
int parserSyntaxError(int error, int internalError, pToken *prevToken, SyntaxStack *S);

/**
 * Kontrola, jestli nepoužíváme proměnné s vykřičníkem/otazníkem na konci
 * 
 * @param token Token, kterému kontrolujeme (konkrétně ID) data string pro přítomnost !/? na konci
 * @param funcTable Tabulka funkcí, kde by se mělo ID s vykřičníkem/otazníkem nacházet
 * @param error Zapíšeme 69, pokud se nenachází v tabulce IDček
 */
void parserSyntaxIDFNCheck(pToken token, psTree *funcTable, int *error);




/*******************************************************STACK*****************************************************************************/

/**
 * Inicializuje zásobník, na první místo vloží type N_PROG jakožto výchozí stav
 * 
 * @param S Ukazatel na zásobník
 * @return int Jedna po chybě mallocu, jinak nula
 */
void parserSyntaxStackInit(SyntaxStack *S, int *internalError);

/**
 * Uvolní paměť zásobníku
 * 
 * @param S Ukazatel na zásobník
 */
void parserSyntaxStackDelete(SyntaxStack *S);

/**
 * Pushne terminál/neterminál na zásobník
 * 
 * @param S Ukazatel na zásobník
 * @param type Ukazatel na zásobník
 * @param error Dojde-li k chybě mallocu, sem se vloží jedna
 */
void parserSyntaxStackPush(SyntaxStack *S, tType type, int *error);

/**
 * Popne zásobník
 * 
 * @param S Ukazatel na zásobník
 * @param error Dojde-li k chybě reallocu, sem se vloží jedna
 * @return tType Hodnota na vrcholu zásobníku
 */
tType parserSyntaxStackPop(SyntaxStack *S, int *error);




/*****************************************************SÉMANTIKA***************************************************************************/

/**
 * Začátek sémantického běhu, naplní tabulku funkcí vestavěnými funkcemi (print, ...)
 * 
 * @param funcTable Ukazatel na tabulku funkcí
 */
void parserSemanticsInitBuiltIn(psTree *funcTable);

/**
 * Projde seznam tokenů předem, a zapíše si definice funkcí a počty jejich parametrů
 * 
 * @param token Momentálně zpracovávaný token
 * @param funcTable Tabulka definicí funkcí
 * @param semanticError Zásobník sémantických errorů k pozdějšímu vypsání
 * @param internalError Hodnota k poznamenání si interního erroru (chyba mallocu)
 * @param error Syntaktický error pro zapsání syntaktické chyby při dosazení něčeho jiného než proměnné do hlavičky definice funkce
 */
void parserSemanticsPreRun(pToken *token, psTree *funcTable, pSemanticsStack semanticError, int *internalError, int *error);

/**
 * Funkce kontrolující sémantické vlastnosti kódu. Dojde-li k syntax erroru, neříká nic, jinak si sémantické errory ukládá na stack a poté vypíše
 * 
 * @param token Momentálně zpracovávaný token
 * @param func Ukazatel na v současnosti zpracovávanou funkci
 * @param funcTable Tabulka definicí funkcí
 * @param varTable Tabulka definicí proměnných
 * @param localTable Tabulka definicí lokálních proměnných
 * @param semanticError Zásobník sémantických errorů k pozdějšímu vypsání
 * @param inFunc Hodnota určující, nacházíme-li se momentálně ve funkci
 * @param internalError Hodnota k poznamenání si interního erroru (chyba mallocu)
 */
void parserSemanticsCheck(pToken token, pToken *func, psTree *funcTable, psTree *varTable, psTree *localTable, pSemanticsStack semanticError, bool inFunc, int *internalError);

/**
 * Pomocná funkce, vytvoří položku typu psData s daty, které jí poskytneme
 * 
 * @param type Proměnná, nebo funkce
 * @param localFrame Ukazatel na lokální rámec proměnných
 * @param params Počet parametrů
 * @return psData Ukazatel na datovou položku
 */
psData parserSemanticsInitData(sType type, struct sTree *localFrame, int params);

/**
 * Pomocná funkce vyhodnocující pomocí (geniálního, musím říct) využití semaforu, nacházíme-li se ve funkci (mezi DEF a příslušným END)
 * 
 * @param inFunc Hodnota udávající, zda se nacházíme ve funkci
 * @param inAux Pomocná hodnota, semafor (za každý IF/WHILE inAux++, za každý END inAux--)
 * @param token Současný zpracovávaný token
 */
void parserSemanticsInFunc(bool *inFunc, int *inAux, pToken token);

/**
 * Vyhodnocení sémantických errorů v případě, že syntax je v pořádku
 * 
 * @param semanticError Zásobník chyb, který projdeme, vypíšeme na stderr
 * @return int Není-li zásobník prázdný, vracíme 3 (sémantická chyba), popř 0, nedošlo-li k chybě
 */
int parserSemanticError(pSemanticsStack semanticError);




/*******************************************************STACK*****************************************************************************/

/**
 * Inicializace zásobníku pro sémantické chyby
 * 
 * @param S Zásobník k inicializaci
 * @param internalError Pro případ, že by nevyšel malloc, k poznamenání si příslušného chybového kódu
 */
void parserSemanticStackInit(pSemanticsStack *S, int *internalError);

/**
 * Uvolnění paměti sémantického zásobníku
 * 
 * @param S Zásobník z uvolnění
 */
void parserSemanticStackDelete(pSemanticsStack *S);

/**
 * K přidání sémantické chyby na zásobník chyb k pozdějšímu výpisu
 * 
 * @param S Zásobník, na který pushujeme
 * @param error Kódové číslo chyby
 * @param name Název tokenu, který chybu vyvolal
 * @param internalError Pro případ, že dojde k chybě mallocu
 * @param line Pozice chybného tokenu
 * @param col Pozice chybného tokenu
 */
void parserSemanticStackPush(pSemanticsStack S, int error, char *name, int *internalError, int line, int col);