/**
 * @file expressions.h
 * 
 * Zpracování výrazů precedenční analýzou
 * 
 * IFJ Projekt 2018, Tým 13
 * 
 * @author <xforma04> Klára Formánková
 * @author <xlanco00> Jan Láncoš
 * @author <xsebel04> Vít Šebela
 * @author <xchalo16> Jan Chaloupka
 */

#pragma once
#include <stdbool.h>
#include "scanner.h"
#include "symtable.h"
#include "common.h"

/**
 * Kolik položek v zásobníku se má alikovat v případě rozšíření
 */
#define EXPR_STACK_CHUNK_SIZE 100

/**
 * Typ relace mezi terminály
 */
typedef enum{
	E_OPEN,		//!< Otevírající (<)
	E_CLOSE,	//!< Zavírající (>)
	E_EQUAL,	//!< Stejná (=)
	E_EMPTY		//!< Neexistuje ( )
} eRelation;

/**
 * Typ terminálu
 */
typedef enum{
	E_INT,		//!< Celé číslo
	E_FLOAT,	//!< Desetinné číslo
	E_STRING,	//!< Řetězec
	E_NIL,		//!< Nil
	E_BOOL,		//!< True/false
	E_UNKNOWN	//!< Neznámý (v případě použítí proměnné)
} eTermType;

/**
 * Typ relace
 */
typedef enum{
	E_MULDIV,	//!< Násobení / dělení
	E_ADDSUB,	//!< Sčítaní / odčítání
	E_NOT,		//!< Booleovsá operace not
	E_ANDOR,	//!< Booleovsá operace and / or
	E_LTGT,		//!< Porovnání větší / menší (nebo se rovná)
	E_EQL,		//!< Porovnání (ne)rovná se
	E_LB,		//!< Levá závorka
	E_RB,		//!< Pravá závorka
	E_VAL,		//!< Proměnná
	E_$			//!< Jiné (není součástí výrazu)
} eRelTerm;

/**
 * Typ položky na zásobníku
 */
typedef enum{
	IT_OPEN,	//!< Otevírající relace (<)
	IT_TERM,	//!< Terminál
	IT_NONTERM,	//!< Neterminál
} eItemType;

/**
 * Hodnota položky zásobníku
 */
typedef union{
	pToken term;	//!< Terminál (token)
	eTermType type;	//!< Typ neterminálu
} eItemVal;

/**
 * Položka zásobníku
 */
typedef struct eItem{
	eItemType type;	//!< Typ položky
	eItemVal val;	//!< Hodnota
} *peItem;

/**
 * Zásobník precedenční analýzy
 */
typedef struct eStack{
	int size;	//!< Velikost zásobníku
	int top;	//!< Index nejvyššího prvku
	peItem *s;	//!< Pole prvků
} *peStack;

/**
 * Hlavní funkce výrazů
 * Převede posloupnost tokenů na kód pro výpočet výrazu + provede
 * kontrolu syntaxe a sémantiky výrazu. Je zde využita precedenční 
 * analýza. Po ukončení bude ukazatel na token ukazovat za výraz
 * 
 * @param token Ukazatel na první token v seznamu, který je součástí výrazu
 * @param idTable Tabulka lokálních poměnných
 * @return int Stavový kód (0 - bez chyby nebo dle zadání)
 */
int exprParse(pToken *token, psTree idTable);

/**
 * Převedení typu tokenu na typ relačního terminálu
 * 
 * @param tokenType Typ tokenu
 * @return eRelTerm Typ terminálu
 */
eRelTerm exprConvTypeToTerm(tType tokenType);

/**
 * Podle precedenční tabulky výrazů vrátí patřičný relační typ
 * 
 * @param currTerm Aktuální terminál na zásobníku
 * @param newTerm Nový (příchozí) terminál
 * @return eRelation Relace mezi terminály
 */
eRelation exprGetRelation(eRelTerm currTerm, eRelTerm newTerm);

/**
 * Inicializace zásobníku
 * 
 * @param stack Ukazatel na zásobník
 */
void exprStackInit(peStack *stack);

/**
 * Uvolnení zásobníku a jeho položek z paměti
 * 
 * @param stack Ukazatel na zásobník
 */
void exprStackDispose(peStack *stack);

/**
 * Vloží na vrchol zásobníku položku
 * 
 * @param stack Zásobník
 * @param item Položka ke vložení
 */
void exprStackPush(peStack stack, peItem item);

/**
 * Vyjme položku na vrcholu zásobníku a vrátí ji
 * 
 * @param stack Zásobník
 * @return peItem Položka na vrcholu zásobníku (NULL pokud je zásobník prázdný)
 */
peItem exprStackPop(peStack stack);

/**
 * Vrátí index prvního terminálu na zásobníku (od vrchu)
 * 
 * @param stack Zásobník
 * @return int Index terminálu (0 pokud na zásobníku není terminál)
 */
int exprStackFindTerm(peStack stack);

/**
 * Vloží otevírací ukazatel na pozici a všechny prvky nad ním posune
 * 
 * @param stack Zásobník
 * @param pos Pozice na zásobníku
 */
void exprStackInsertOpen(peStack stack, int pos);

/**
 * Převede výraz obraničený relačníma operátorama (< a >)
 * na kód interpretu + zkontroluje gramatiku a správnost typů
 * 
 * @param stack Zásobník
 * @param idTable Lokální tabulka proměnných
 * @return int Chybový kód podle zadání (0, 2, 3, 4, 99)
 */
int exprStackParse(peStack stack, psTree idTable);

/**
 * Vrátí řetězec reprezentující typ terminálu (používá se při výpisu chyby)
 * 
 * @param type Typ terminálu
 * @return const char* Řetězec na vypsání
 */
const char *exprTermTypeToString(eTermType type);

/**
 * Vypíše chybu stackParse na stderr
 * 
 * @param etype číselný kód chyby (2 = syntax, 4 = sémantická)
 * @param isSingle Byl dán jen jeden operand
 * @param isSame Jsou operandy stejného typu
 * @param lt Typ levého operandu (neexistuje pokud byl dán jen jeden operand)
 * @param rt Typ pravého operandu
 * @param op Token reprezentující operátor
 */
void exprSPPrintError(int etype, bool isSingle, bool isSame, eTermType lt, eTermType rt, pToken op);
