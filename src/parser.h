/**
 * @file parser.h
 * 
 * Syntaktický analyzátor
 * 
 * IFJ Projekt 2018, Tým 13
 * 
 * @author <xforma04> Klára Formánková
 * @author <xlanco00> Jan Láncoš
 * @author <xsebel04> Vít Šebela
 * @author <xchalo16> Jan Chaloupka
 */

#pragma once
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "scanner.h"
#include "symtable.h"
#include "codegen.h"

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
 * Vlastní tělo parseru, v průběhu procházení token-listu zkontroluje syntax, sémantiku, a volá generátor z codegen.c
 * 
 * @param List Výstup lexikálního analyzátorů, obousměrně provázaný seznam tokenů
 * @return int 99 po interní chybě, 2, 3, 4, 5, 6, 9 podle příslušného výskytu chyby, jinak 0
 */
int parser(pToken *List);

/**
 * Vyhodnocení chyb po každém novém cyklu, výstupní hodnota využita i jako návratová hodnota parseru (a potažmo celého programu)
 * 
 * @param error Hodnota udávající, zda-li již došlo k syntaktické chybě, a které
 * @param internalError Hodnota udávající, zda-li již došlo k interní chybě, a které
 * @param prevToken Hodnota tokenu, u kterého došlo k chybě (k výpisu)
 * @return Návratová hodnota užita jako návratová hodnota parseru
 */
int parserError(int error, int internalError, pToken *prevToken);




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
 * @param internalError Jedna po chybě mallocu, jinak nula
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
 * @param error Syntaktický error pro zapsání syntaktické chyby při dosazení něčeho jiného než proměnné do hlavičky definice funkce
 */
void parserSemanticsPreRun(pToken *token, psTree *funcTable, int *error);

/**
 * Funkce kontrolující sémantické vlastnosti kódu. Dojde-li k syntax erroru, neříká nic, jinak si sémantické errory ukládá na stack a poté vypíše
 * 
 * @param token Momentálně zpracovávaný token
 * @param func Ukazatel na v současnosti zpracovávanou funkci
 * @param funcTable Tabulka definicí funkcí
 * @param varTable Tabulka definicí proměnných
 * @param localTable Tabulka definicí lokálních proměnných
 * @param error Vypsání erroru
 * @param inFunc Hodnota určující, nacházíme-li se momentálně ve funkci
 */
void parserSemanticsCheck(pToken token, pToken *func, psTree *funcTable, psTree *varTable, psTree *localTable, int *error, bool inFunc);

/**
 * Pomocná funkce, vytvoří položku typu psData s daty, které jí poskytneme
 * 
 * @param type Proměnná, nebo funkce
 * @param localFrame Ukazatel na lokální rámec proměnných
 * @param params Počet parametrů
 * @param defined Byla již proměnná definována a nyní se jedná pouze o redefinici
 * @return psData Ukazatel na datovou položku
 */
psData parserSemanticsInitData(sType type, struct sTree *localFrame, int params, bool defined);

/**
 * Pomocná funkce vyhodnocující pomocí (geniálního, musím říct) využití semaforu, nacházíme-li se ve funkci (mezi DEF a příslušným END)
 * 
 * @param inFunc Hodnota udávající, zda se nacházíme ve funkci
 * @param inAux Pomocná hodnota, semafor (za každý IF/WHILE inAux++, za každý END inAux--)
 * @param token Současný zpracovávaný token
 */
void parserSemanticsInFunc(bool *inFunc, int *inAux, pToken token);