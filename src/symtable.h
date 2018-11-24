#include <stdio.h>
#include <stdbool.h>

//typ id
typedef enum{
	VAR,
	FUNC
} sType;

//data v uzlu
typedef struct sData{
	sType type;
	struct sTree *localFrame;
	int params; 
} *psData;

//uzel stromu
typedef struct sTree{
	char *key;	
	struct sData *data;
	struct sTree *lptr;
	struct sTree *rptr;
} *psTree;

/**
 * Inicializuje tabulku symbolů (binární vyhledávací strom) 
 * 
 * @param tree Ukazatel na strom pro inicializaci
 */
void symTabInit(psTree *tree);

/**
 * Vloží do stromu nový uzel s hodnotou data
 * 
 * @param tree Strom, do kterého se bude vkládat
 * @param data Data pro vložení do stromu
 * @return bool Pokud se vložení povedlo, vrací true
 */
bool symTabInsert(psTree *tree, char *key, psData data);

/**
 * Podle klíče vyhledá uzel ve stromě 
 * 
 * @param tree Strom, ve kterém se bude vyhledávat
 * @param key Klíč uzlu, podle kterého se bude vyhledávat ve stromě
 * @return psData Vrací data nalezeného uzlu
 */
psData symTabSearch(psTree *tree, char *key);

/**
 * Zruší celý strom (tabulku symbolů)
 * 
 * @param tree Strom pro zrušení
 */
void symTabDispose(psTree *tree);
