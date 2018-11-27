/**
 * @file expressions.h
 * 
 * Zpracování výrazů precedenční analýzou
 * 
 * @version 0.1
 * @date 2018-11-20
 */

#pragma once
#include <stdbool.h>
#include "scanner.h"
#include "symtable.h"
#include "common.h"

#define EXPR_STACK_CHUNK_SIZE 100

typedef enum{
	E_OPEN,
	E_CLOSE,
	E_EQUAL,
	E_EMPTY
} eRelation;

typedef enum{
	E_INT,
	E_FLOAT,
	E_STRING,
	E_NIL,
	E_BOOL,
	E_UNKNOWN
} eTermType;

typedef enum{
	E_MULDIV,
	E_ADDSUB,
	E_NOT,
	E_ANDOR,
	E_LTGT,
	E_EQL,
	E_LB,
	E_RB,
	E_VAL,
	E_$
} eRelTerm;

typedef enum{
	IT_OPEN,
	IT_TERM,
	IT_NONTERM,
} eItemType;

typedef union{
	pToken term;
	eTermType type;
} eItemVal;

typedef struct eItem{
	eItemType type;
	eItemVal val;
} *peItem;

typedef struct eStack{
	int size;
	int top;
	peItem *s;
} *peStack;

int exprParse(pToken *token, psTree idTable);

eRelTerm exprConvTypeToTerm(tType tokenType);
eRelation exprGetRelation(eRelTerm currTerm, eRelTerm newTerm);

void exprStackInit(peStack *stack);
void exprStackDispose(peStack *stack);
void exprStackPush(peStack stack, peItem item);
peItem exprStackPop(peStack stack);
int exprStackFindTerm(peStack stack);
void exprStackInsertOpen(peStack stack, int pos);
int exprStackParse(peStack stack, psTree idTable);