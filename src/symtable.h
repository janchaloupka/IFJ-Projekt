#include<stdio.h>
#include<stdbool.h>



typedef struct Param{
	int type;
	char *id;
	struct Param *next;
}TParam;

//data v uzlu
typedef struct Data{
	int type;
	TParam *params;
	bool defined;
}TData;

//uzel stromu
typedef struct Node{
	char *key;	
	struct TData *data;
	struct Node *lptr;
	struct Node *rptr;
}TNode;

//seznam stromu
typedef struct SymTab{
	TNode *first; //ukazatel na koren prvniho stromu
	struct SymTab *next; //ukazatel na dalsi strom
}TSymTab;