/**
 * @file codegen.h
 * 
 * 
 * 
 * @version 0.1
 * @date 2018-11-29
 */

#include <stdio.h>
#include <string.h>
#include "scanner.h"
#include "symtable.h"

/**
 * Velikost alokační jednotky pole znaků
 */
#define IFWHILE_STACK_CHUNK_SIZE 100

/**
 * TODO
 * 
 * @param type 
 * @param token 
 * @param table 
 */
void codeFromToken(tType type, pToken token, psTree table);