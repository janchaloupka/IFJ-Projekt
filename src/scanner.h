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
	UNKNOWN, 	//!< Výchozí typ
	ID, 		//!< Identifikátor funkce/proměnné
	ID_FN, 		//!< Identifikátor funkce
	KW_DEF,		//!< Keyword - DEF
	KW_DO,		//!< Keyword - DO
	KW_ELSE,	//!< Keyword - ELSE
	KW_END,		//!< Keyword - END
	KW_IF,		//!< Keyword - IF
	KW_NOT,		//!< Keyword - NOT
	KW_NIL,		//!< Keyword - NIL
	KW_THEN,	//!< Keyword - THEN
	KW_WHILE,	//!< Keyword - WHILE
	SEP_RB,		//!< Levá závorka
	SEP_LB,		//!< Pravá závorka
	SEP_EOL,	//!< Konec řádku
	SEP_EOF,	//!< Konec souboru
	OP_ADD,		//!< Matematika - Sčítání
	OP_SUB,		//!< Matematika - Krácení
	OP_MUL,		//!< Matematika - Násobení
	OP_DIV,		//!< Matematika - Dělení
	OP_EQL,		//!< Porovnání - Rovná se
	OP_NEQ,		//!< Porovnání - Nerovná se
	OP_LT,		//!< Porovnání - Menší
	OP_GT,		//!< Porovnání - Větší
	OP_LTE,		//!< Porovnání - Menší nebo se rovná
	OP_GTE,		//!< Porovnání - Větší nebo se rovná
	OP_ASGMT,	//!< Přiřazení
	INTEGER,	//!< Celé kladné číslo
	DOUBLE,		//!< Desetinné kladné číslo
	DOUBLE_EXP,	//!< Desetinné kladné číslo zapsané exponentem
	STRING,		//!< Řetězec znaků
	ERROR		//!< Speciální typ, při parsování tohoto úseku nastala chyba
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

void scannerGetToken(FILE *file, pToken *output);
void scannerFSM(FILE *file, pToken *output);
void scannerPrintType(pToken token);