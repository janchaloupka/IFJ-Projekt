/**
 * @file expressions.h
 * 
 * Zpracování výrazů metodou zespodu nahoru
 * 
 * @version 0.1
 * @date 2018-11-20
 */

#include <stdbool.h>
#include "scanner.h"

#define EXPR_STACK_CHUNK_SIZE 100

typedef enum{
	OPEN,
	CLOSE,
	EQUAL,
	ERROR
} eRelation;

typedef enum{
	E_MULDIV,
	E_ADDSUB,
	E_LTGT,
	E_EQL,
	E_LB,
	E_RB,
	E_VAL,
	E_$
} eTerm;

typedef enum{
	RT_VAL,
	RT_ADD,
	RT_SUB,
	RT_DIV,
	RT_MUL,
	RT_EQL,
	RT_NEQ,
	RT_LT,
	RT_GT,
	RT_LTE,
	RT_GTE,
	RT_BRACKET
} eRuleType;

typedef enum{
	NT_RELATION,
	NT_TOKEN,
	NT_RULE
} eNodeType;

typedef union{
	eRelation relation;
	pToken token;
	struct eRule *rule;
} eNodeVal;

typedef struct eNode{
	eNodeType type;
	eNodeVal val;
} *peNode;

typedef struct eRule{
	eRuleType type;
	struct eNode *lNode;
	struct eNode *rNode;
} *peRule;


typedef struct eStack{
	int size;
	int top;
	peNode *s;
} *peStack;

eTerm		exprConvTypeToTerm(tType tokenType);
eRelation	exprGetRelation(eTerm currTerm, eTerm newTerm);
int 		exprParse(pToken *token, peNode *nodeTree);

void	exprStackInit(peStack *stack);
void	exprStackPush(peStack stack, peNode node);
peNode	exprStackPop(peStack stack);
int		exprStackFirstTermPos(peStack stack);
void	exprStackInsertOpen(peStack stack, int pos);
int		exprStackToRule(peStack stack);