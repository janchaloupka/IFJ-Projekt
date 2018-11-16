/**
 * @file scanner.h
 * 
 * Lexikální analyzátor
 * 
 * @version 0.1
 * @date 2018-10-19
 */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

#define EOL '\n'

/**
 * Po kolika znacích se má alokovat pole pro string
 * použité pro procházení tokenů ve FSM scanneru
 */
#define STRING_CHUNK_LEN 100

/**
 * Typy tokenů 
 */
typedef enum{
	T_UNKNOWN, 	//!< Výchozí typ
	T_ID, 		//!< Identifikátor funkce/proměnné
	T_IDFN, 	//!< Identifikátor funkce
	T_DEF,		//!< Keyword - DEF
	T_DO,		//!< Keyword - DO
	T_ELSE,		//!< Keyword - ELSE
	T_END,		//!< Keyword - END
	T_IF,		//!< Keyword - IF
	T_NOT,		//!< Keyword - NOT
	T_NIL,		//!< Keyword - NIL
	T_THEN,		//!< Keyword - THEN
	T_WHILE,	//!< Keyword - WHILE
	T_RBRCKT,	//!< Levá závorka
	T_LBRCKT,	//!< Pravá závorka
	T_COMMA,	//!< Oddělovací čárka
	T_EOL,		//!< Konec řádku
	T_EOF,		//!< Konec souboru
	T_ADD,		//!< Matematika - Sčítání
	T_SUB,		//!< Matematika - Krácení
	T_MUL,		//!< Matematika - Násobení
	T_DIV,		//!< Matematika - Dělení
	T_EQL,		//!< Porovnání - Rovná se
	T_NEQ,		//!< Porovnání - Nerovná se
	T_LT,		//!< Porovnání - Menší
	T_GT,		//!< Porovnání - Větší
	T_LTE,		//!< Porovnání - Menší nebo se rovná
	T_GTE,		//!< Porovnání - Větší nebo se rovná
	T_ASSIGN,	//!< Přiřazení
	T_INTEGER,	//!< Celé kladné číslo
	T_DOUBLE,	//!< Desetinné kladné číslo
	T_DOUBLE_E,	//!< Desetinné kladné číslo zapsané exponentem
	T_STRING,	//!< Řetězec znaků
	N_PROG, 	//!< Neterminály
	N_BODY,
	N_NEWL,
	N_NEWLN,
	N_DEFUNC,
	N_CALFUNC,
	N_FUNC,
	N_PARS,
	N_PARSN,
	N_DEFVAR,
	N_VAR,
	N_IF,
	N_WHILE,
} tType;


/**
 * Stavy FSM lexikálního překladače
 * Názvy stavů odpovídají grafu
 */
typedef enum{
	STATE_NULL,
	STATE_ERROR,
	STATE_START,
	STATE_SPACE,
	STATE_NEQ,
	STATE_NEQ2,
	STATE_GT,
	STATE_GTE,
	STATE_LT,
	STATE_LTE,
	STATE_ASSIGN,
	STATE_EQL,
	STATE_ADD,
	STATE_SUB,
	STATE_MUL,
	STATE_DIV,
	STATE_EOL,
	STATE_EOF,
	STATE_LCMNT,
	STATE_LBR,
	STATE_RBR,
	STATE_COMMA,
	STATE_ID,
	STATE_ID_FN,
	STATE_STR,
	STATE_STR2,
	STATE_STR3,
	STATE_STR4,
	STATE_INT0,
	STATE_INT,
	STATE_DBLE,
	STATE_DBLE2,
	STATE_EXP,
	STATE_EXP2,
	STATE_EXP3,
	STATE_BCMT,
	STATE_BCMT2,
	STATE_BCMT3,
	STATE_BCMT4,
	STATE_BCMT5,
	STATE_BCMT6,
	STATE_BCMT7,
	STATE_BCMT8,
	STATE_BCMT9,
	STATE_BCMT10,
	STATE_BCMT11,
	STATE_BCMT12,
	STATE_BCMT13
} sState;


/**
 * Token zpracovaný lexikálním analyzátorem
 * Dá se použít jako obousměrný seznam
 */
typedef struct Token{
	tType type;	//!< Typ tokenu
	void *data;	//!< Konkrétní hodnota tokenu
				//!< Ukazatel na char* pokud je typ string nebo číslo
				//!< Ukazatel na tabulku id pokud je typ id
	unsigned int linePos;
	unsigned int colPos;
	struct Token *prevToken;
	struct Token *nextToken;
} *pToken;


/**
 * Načte všechny tokeny ze souboru
 * 
 * @param output Ukazatel na první získaný token
 * @param file Ukazatel na soubor, v případě hodnoty NULL bude použit stdin
 * @return int Stav operace - 0, pokud vše proběhlo v pořádku, 
 * jinak vrátí chybový kód podle zadání (str 2)
 */
int scannerGetTokenList(pToken *firstToken, FILE *file);


/**
 * Načte další token ze souboru
 * 
 * @param output Ukazatel na získaný token
 * @param file Ukazatel na soubor, v případě hodnoty NULL bude použit stdin
 * @return int Stav operace - 0, pokud vše proběhlo v pořádku, 
 * jinak vrátí chybový kód podle zadání (str 2)
 */
int scannerGetToken(pToken *output, FILE *file);


/**
 * Korektně uvolní token z paměti včetně dalších dat
 * 
 * @param token Ukazatel na token, který má být uvolněn
 */
void scannerFreeToken(pToken *token);


/**
 * Korektně uvolní seznam tokenů z paměti včetně dalších dat
 * 
 * @param token Ukazatel na token, který je součástí seznamu
 */
void scannerFreeTokenList(pToken *token);


/**
 * Vypíše informace o tokenu na stdin
 * 
 * @param token Token, který má být vypsán
 */
void scannerPrintToken(pToken token);

/**
 * Vypíše informace o všech tokenech v seznamu na stdin
 * 
 * @param token Token, který je součástí seznamu
 */
void scannerPrintTokenList(pToken token);


// ***************************************** //
//    Interní funkce výhradně pro scanner    //
// ***************************************** //


/**
 * Stavový automat lexikálního analyzátoru implementovaný 
 * podle grafu v dokumentaci. Tato funkce neodlilšuje mezi
 * id a keyword a vrací všechny data bez modifikací
 * 
 * @param file Ukzatel na soubor, ze kterého se má číst
 * @param token Token který má být vyplněn daty
 * @return int Stav operace - 0, pokud vše proběhlo v pořádku, 
 * jinak vrátí chybový kód podle zadání (str 2)
 */
int _scannerFSM(FILE *file, pToken token);


/**
 * Kontrola, zda je token typu ID klíčovým slovem. V případě, že 
 * je klíčové slovo bude tokenu změnen typ
 * 
 * @param token Token, který má být zkontrolován
 * @return true Token je klíčové slovo a byl modifikován
 * @return false Token není klíčové slovo (bez modifikace) 
 */
bool _scannerIsKeyword(pToken token);


/**
 * Vypsání chybové hlášky na stderr
 * 
 * @param state Stav automatu, při kterém chyba nastala
 * @param currChar Aktuálně čtený znak ze souboru
 * @param line Číslo aktuálního řádku
 * @param col Pozice písmena na řádku
 */
void _scannerHandleError(sState state, char currChar, unsigned int line, unsigned int col);