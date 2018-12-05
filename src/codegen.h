/**
 * @file codegen.h
 * 
 * Generace kódu z tokenů
 * 
 * IFJ Projekt 2018, Tým 13
 * 
 * @author <xforma14> Klára Formánková
 * @author <xlanco00> Jan Láncoš
 * @author <xsebel04> Vít Šebela
 * @author <xchalo16> Jan Chaloupka
 */

#include <stdio.h>
#include <string.h>
#include "scanner.h"
#include "symtable.h"

/**
 * Velikost alokační jednotky pole znaků
 */
#define IFWHILE_STACK_CHUNK_SIZE 100

typedef struct cStackItem{
	bool isIf;
	int id;
} *pcStackItem;

/**
 * Vygeneruje kód z tokenu, funkce je volána syntaktickým analyzátorem při každém načtení tokenu
 * 
 * @param type Gramatický stav, ve kterém se parser nachází
 * @param token Aktuálně načtený token
 * @param table Lokální tabulka proměnných aktuální funkce
 */
void codeFromToken(tType type, pToken token, psTree table);