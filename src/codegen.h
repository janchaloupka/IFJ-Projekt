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


void codeFromToken(tType type, pToken token, psTree table);