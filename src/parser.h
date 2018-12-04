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
 * Pomocný zásobník k rekurzivnímu sestupu, probíhá na něm rozklad neterminálů na terminály
 */
typedef struct{
    tType *a;   // Pole neterminálů/terminálů
    int size;
    int top;    // Index prvního volného místa
	int last;   // Index naposledy vloženého prvku
} SyntaxStack;

/**
 * Vlastní tělo parseru, v průběhu procházení token-listu zkontroluje syntax (za pomoci externí funkce exprParse z knihovny 
 * expressions.c), sémantiku, a volá generátor kódu z codegen.c
 * 
 * @param List Výstup lexikálního analyzátorů, obousměrně provázaný seznam tokenů
 * @return int 99 po interní chybě, 2, 3, 4, 5, 6 podle příslušného výskytu chyby ve vstupním kódu, jinak 0
 */
int parser(pToken *List);

/**
 * Vyhodnocení chyb po každém cyklu včetně pre-runu, výstupní hodnota využita i jako návratová hodnota parseru (a potažmo celého programu)
 * 
 * @param error Hodnota udávající, zda-li již došlo k chybě, a ke které
 * @param internalError Hodnota udávající, zda-li již došlo k interní chybě, a ke které
 * @param prevToken Hodnota tokenu, u kterého došlo k chybě (využito k výpisu)
 * @return int Návratová hodnota využita jako návratová hodnota parseru
 */
int parserError(int error, int internalError, pToken *prevToken);




/******************************************************SYNTAX******************************************************************************/

/**
 * Je-li na stacku terminál, porovná jej se vstupním tokenem
 * 
 * @param S Zásobník terminálů/neterminálů určených ke zpracování (v tomto případě je na vrcholu vždy terminál)
 * @param token Momentálně zpracovávaný token
 * @param error Ukazatel na integerovou error hodnotu, do které zapíše dvojku, nejsou-li porovnávané tokeny stejné
 */
void parserSyntaxCompare(SyntaxStack S, pToken token, int *error);

/**
 * Je-li na zásobníku neterminál, podle tabulky LL(1) gramatiky určí, jak jej dále rozložit, části vloží v opačném pořadí na zásobník
 * 
 * @param S Ukazatel na zásobník terminálů/neterminálů určených ke zpracování
 * @param token Momentálně zpracovávaný token
 * @param error Ukazatel na integerovou error hodnotu, do které zapíše dvojku, nedojde-li k možnosti aplikovat rozkládací pravidlo
 * @param internalError Ukazatel na integerovou error hodnotu, kterou předává stackovým funkcím, k zapsání selhání malloců
 * @param localTable Předává se funkci exprParse
 */
void parserSyntaxExpand(SyntaxStack *S, pToken *token, int *error, int *internalError, psTree localTable);

/**
 * Kontrola, jestli nepoužíváme proměnné s vykřičníkem/otazníkem na konci
 * 
 * @param token Token (konkrétně typu T_ID), kterému kontrolujeme data string pro přítomnost !/? na konci
 * @param funcTable Tabulka funkcí, kde by se mělo ID s vykřičníkem/otazníkem nacházet
 * @param error Zapíšeme 69, pokud se v tabulce ID funkcí nenachází
 */
void parserSyntaxIDFNCheck(pToken token, psTree *funcTable, int *error);




/*******************************************************STACK*****************************************************************************/

/**
 * Inicializuje zásobník, na první místo vloží type N_PROG jakožto výchozí stav
 * 
 * @param S Ukazatel na inicializovaný zásobník
 * @param internalError Na tuto adresu zapíše 1 po chybě mallocu
 */
void parserSyntaxStackInit(SyntaxStack *S, int *internalError);

/**
 * Korektně uvolní paměť zásobníku
 * 
 * @param S Ukazatel na zásobník
 */
void parserSyntaxStackDelete(SyntaxStack *S);

/**
 * Vloží terminál/neterminál na vrchol zásobníku
 * 
 * @param S Ukazatel na zásobník
 * @param type Terminál/neterminál vkládáný na zásobník
 * @param internalError Na tuto adresu zapíše 1 po chybě mallocu
 */
void parserSyntaxStackPush(SyntaxStack *S, tType type, int *internalError);

/**
 * Popne zásobník
 * 
 * @param S Ukazatel na zásobník
 * @param internalError Na tuto adresu se zapíše 1 při podtečení zásobníku
 */
void parserSyntaxStackPop(SyntaxStack *S, int *error);




/*****************************************************SÉMANTIKA***************************************************************************/

/**
 * Začátek sémantického běhu, naplní tabulku funkcí vestavěnými funkcemi (print, ...)
 * 
 * @param funcTable Ukazatel na tabulku funkcí
 */
void parserSemanticsInitBuiltIn(psTree *funcTable);

/**
 * Projde seznam tokenů před oficiálním syntaktickým během, a zapíše si definice funkcí a počty jejich parametrů
 * 
 * @param token Momentálně zpracovávaný token
 * @param funcTable Tabulka definicí funkcí
 * @param error Pro zapsání chyby při dosazení něčeho jiného než proměnné do hlavičky definice funkce, nebo pokusu o redefinici funkce
 */
void parserSemanticsPreRun(pToken *token, psTree *funcTable, int *error);

/**
 * Funkce kontrolující sémantické vlastnosti kódu
 * 
 * @param token Momentálně zpracovávaný token
 * @param func Ukazatel na v současnosti zpracovávanou funkci
 * @param funcTable Tabulka definicí funkcí
 * @param varTable Tabulka definicí proměnných
 * @param localTable Tabulka definicí lokálních proměnných
 * @param error Pro vypsání erroru
 * @param inFunc Hodnota určující, nacházíme-li se momentálně ve funkci
 */
void parserSemanticsCheck(pToken token, pToken *func, psTree *funcTable, psTree *varTable, psTree *localTable, int *error, bool inFunc);

/**
 * Pomocná funkce, vytvoří položku typu psData s daty, které jí poskytneme, slouží k zapisování do tabulky
 * 
 * @param type Proměnná, nebo funkce
 * @param localFrame Ukazatel na lokální rámec proměnných
 * @param params Počet parametrů
 * @param defined Byla-li již proměnná definována a nyní se jedná pouze o redefinici/přiřazení
 * @return psData Ukazatel na incializovanou datovou položku
 */
psData parserSemanticsInitData(sType type, struct sTree *localFrame, int params, bool defined);

/**
 * Pomocná funkce vyhodnocující pomocí využití semaforu, nacházíme-li se ve funkci (mezi DEF a příslušným END)
 * 
 * @param inFunc Hodnota udávající, zda se nacházíme ve funkci
 * @param inAux Pomocná hodnota, semafor (za každý IF/WHILE inAux++, za každý END inAux--)
 * @param token V současnosti zpracovávaný token
 */
void parserSemanticsInFunc(bool *inFunc, int *inAux, pToken token);