/*
 * symtab.h
 *
 * Hlavičkový soubor pro symtab.c
 *
 */


#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>

#define TRUE 1
#define FALSE 0


/* uzel stromu */

typedef struct tBSTNode {
	char Key;			                                     /* klíč */
	int Content;                                             /* užitečný obsah uzlu */
	struct tBSTNode *LPtr;                                   /* levý podstrom */
	struct tBSTNode *RPtr;                                   /* pravý podstrom */
} *tBSTNodePtr;

/* prototypy funkcí */

void BSTInit   (tBSTNodePtr *);
int BSTSearch  (tBSTNodePtr, char, int *);
void BSTInsert (tBSTNodePtr *, char, int);
void BSTDelete (tBSTNodePtr *, char);
void BSTDispose(tBSTNodePtr *);