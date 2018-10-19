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
	IDENTIFIER, //!< Identifikátor funkce/proměnné
	KEYWORD,	//!< Rezervované slovo
	SEPARATOR,	//!< '(', ')', EOL
	OPERATOR,	//!< Matematické operátory a přiřazení
	COMMENT,	//!< Komentáře - celořádkové i blokové
	NUMBER,		//!< Celé kladné číslo
	DOUBLE,		//!< Desetinné kladné číslo, může být zapsané exponentem
	STRING		//!< Řetězec znaků
} tType;

/**
 * Token zpracovaný lexikálním analyzátorem
 * Funguje jako obousměrný seznam
 */
typedef struct Token{
	tType type;				//!< Typ tokenu
	char *value;			//!< Konkrétní hodnota tokenu
	struct Token *tPrev;	//!< Předchozí prvek seznamu, NULL pokud je první
	struct Token *tNext;	//!< Následující prvek seznamu, NULL pokud je poslední
} *pToken;

/**
 * TODO: Dokumentace této funkce
 * 
 * @param input 
 * @param output 
 * @return int 
 */
int scannerParse(char* input, pToken output);

/**
 * Odstraní všechny tokeny v seznamu a uvolní je z paměti
 * Jsou uvloněný všechny tokeny včetně zadaného v argumentu
 * 
 * @param token Ukazatel na libovolný prvek seznamu
 */
void disposeTokenList(pToken token);