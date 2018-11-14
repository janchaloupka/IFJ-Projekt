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

#define KEYWORDS_LEN 9
/**
 * Seznam rezervovaných klíčových slov
 */
char const *keywords[KEYWORDS_LEN];

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
	T_ERROR		//!< Speciální typ, při parsování tohoto úseku nastala chyba
} tType;



/**
 * Stavy FSM lexikálního překladače
 */
typedef enum{
	STATE_ERROR,
	STATE_START,
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
 * Všechny typy které mužou být jako data v Tokenu
 */
union TokenData{
	int i;
	double d;
	char *str;
	void *id;
};

/**
 * Token zpracovaný lexikálním analyzátorem
 */
typedef struct Token{
	tType type;				//!< Typ tokenu
	union TokenData data;	//!< Konkrétní hodnota tokenu
} *pToken;

bool scannerFoundError;

void scannerGetToken(FILE *file, pToken *output);
void scannerFSM(FILE *file, pToken *output);
void scannerPrintToken(pToken token);
void scannerHandleError(tType state, char currChar, unsigned int line, unsigned int pos);