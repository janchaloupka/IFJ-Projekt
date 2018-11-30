/**
 * @file common.h
 * 
 * Knihovna pro obecné věci
 * 
 * IFJ Projekt 2018, Tým 13
 * 
 * @author <xforma04> Klára Formánková
 * @author <xlanco00> Jan Láncoš
 * @author <xsebel04> Vít Šebela
 * @author <xchalo16> Jan Chaloupka
 */

#pragma once
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "scanner.h"

void *safeMalloc(size_t _Size);
void *safeRealloc(void *_Block, size_t _Size);

char *stringToInterpret(char *rawString);

char *intToInterpret(char *rawInt);

char *floatToInterpret(char *rawFloat);

char *trueToInterpret();

char *falseToInterpret();

char *nilToInterpret();

char *varToInterpret(char *id);

char *funcToInterpret(char *id);

void generateBaseCode();
