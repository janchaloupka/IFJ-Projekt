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

#define KEYWORDS_LEN 9
/**
 * Seznam rezervovaných klíčových slov
 * TODO: Možná přesunout jinam na obecnější místo, potřebuje to víc součástí
 */
char const *keywords[KEYWORDS_LEN];

/**
 * Typy tokenů 
 */
typedef enum{
	UNKNOWN, 	//!< Výchozí typ
	IDENTIFIER, //!< Identifikátor funkce/proměnné
	BLOCK_KW_START, 	//!< Začátek bloku klíčových slov
	KW_DEF,		//!< Keyword - DEF
	KW_DO,		//!< Keyword - DO
	KW_ELSE,	//!< Keyword - ELSE
	KW_END,		//!< Keyword - END
	KW_IF,		//!< Keyword - IF
	KW_NOT,		//!< Keyword - NOT
	KW_NIL,		//!< Keyword - NIL
	KW_THEN,	//!< Keyword - THEN
	KW_WHILE,	//!< Keyword - WHILE
	BLOCK_KW_END, 		//!< Konec bloku klíčových slov
	BLOCK_SEP_START, 	//!< Začátek bloku oddělovačů
	SEP_RB,		//!< Levá závorka
	SEP_LB,		//!< Pravá závorka
	SEP_EOL,	//!< Konec řádku
	SEP_EOF,	//!< Konec řádku
	BLOCK_SEP_END, 		//!< Konec bloku oddělovačů
	BLOCK_OP_START, 	//!< Začátek bloku operátorů
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
	BLOCK_OP_END, 		//!< Konec bloku operátorů
	BLOCK_NUM_START, 	//!< Začátek bloku čísel
	INTEGER,	//!< Celé kladné číslo
	DOUBLE,		//!< Desetinné kladné číslo
	DOUBLE_EXP,	//!< Desetinné kladné číslo zapsané exponentem
	BLOCK_NUM_END, 		//!< Konec bloku čísel
	COMMENT,	//!< Komentáře - celořádkové i blokové
	STRING,		//!< Řetězec znaků
	ERROR		//!< Speciální typ, při parsování tohoto úseku nastala chyba
} tType;



/**
 * Stavy FSM lexikálního překladače
 */
typedef enum{
	STATE_START,
	STATE_EQL,
	STATE_LT,
	STATE_GT,
	STATE_NEQ,
	STATE_LCOMNT,
	STATE_INT0,
	STATE_INT,
	STATE_DBLE,
	STATE_DBLE2,
	STATE_EXP,
	STATE_EXP2,
	STATE_EXP3
} sState;

/**
 * Token zpracovaný lexikálním analyzátorem
 * Funguje jako obousměrný seznam
 */
typedef struct Token{
	tType type;				//!< Typ tokenu
	//char *value;			//!< Konkrétní hodnota tokenu
	int line;				//!< Na kterém řádku v původním souboru se token nachází, 
							//!< hodí se při vypisování chyby v dalších parserech
	//struct Token *tPrev;	//!< Předchozí prvek seznamu, NULL pokud je první
	struct Token *tNext;	//!< Následující prvek seznamu, NULL pokud je poslední
} *pToken;

/**
 * TODO: Dokumentace této funkce
 * 
 * @param input 
 * @param output 
 * @return int 
 */
int scannerParse(char* input, pToken *output);

void scannerDebugPrint(pToken firstToken);

/**
 * Odstraní všechny tokeny v seznamu a uvolní je z paměti
 * Jsou uvloněný všechny tokeny včetně zadaného v argumentu
 * 
 * @param token Ukazatel na libovolný prvek seznamu
 */
void disposeTokenList(pToken token);


void scannerFSM_EQL		(char input, pToken token, sState *nextState, bool *requestNextChar);
void scannerFSM_LT		(char input, pToken token, sState *nextState, bool *requestNextChar);
void scannerFSM_GT		(char input, pToken token, sState *nextState, bool *requestNextChar);
void scannerFSM_NEQ		(char input, pToken token, sState *nextState, bool *requestNextChar);
void scannerFSM_LCOMNT	(char input, pToken token, sState *nextState, bool *requestNextChar);
void scannerFSM_INT0	(char input, pToken token, sState *nextState, bool *requestNextChar);
void scannerFSM_INT		(char input, pToken token, sState *nextState, bool *requestNextChar);
void scannerFSM_DBLE	(char input, pToken token, sState *nextState, bool *requestNextChar);
void scannerFSM_DBLE2	(char input, pToken token, sState *nextState, bool *requestNextChar);
void scannerFSM_EXP		(char input, pToken token, sState *nextState, bool *requestNextChar);
void scannerFSM_EXP2	(char input, pToken token, sState *nextState, bool *requestNextChar);
void scannerFSM_EXP3	(char input, pToken token, sState *nextState, bool *requestNextChar);
void scannerFSM_START	(char input, pToken token, sState *nextState, bool *requestNextChar);
