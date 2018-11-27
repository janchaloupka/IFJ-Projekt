/**
 * @file common.h
 * 
 * Knihovna pro obecné věci
 * 
 * @version 0.1
 * @date 2018-11-25
 */

#pragma once
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "scanner.h"


char *stringToInterpret(char *rawString);

char *intToInterpret(char *rawInt);

char *floatToInterpret(char *rawFloat);

char *trueToInterpret();

char *falseToInterpret();

char *nilToInterpret();

char *varToInterpret(char *id);

char *funcToInterpret(char *id);

void generateBaseCode();