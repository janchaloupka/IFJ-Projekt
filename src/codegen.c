#include "codegen.h"

void codeFromToken(tType type, pToken token, psTree table){
	
	static int ifWhile; // 1 = if, 2 = while
	static int ifCounter = 0;
	static int whileCounter = 0;
	static bool defFunc = false;

	switch(type){
		char *result; //výsledek expressionu
		
		case N_DEFVAR:
			break;

		case N_DEFUNC:
			defFunc = true;
			break;

		case T_ID:
			if(defFunc == true){ //je to id funkce
				char *funcId = token->data;
				printf("LABEL %s\n", funcId);
				printf("PUSHFRAME\n");
				defFunc = false;	
			}
			break;

		case T_THEN:
			result = "expr"; //výsledek expressionu
			ifCounter++; //kolikátej je to if 
			ifWhile = 1; //=> je to if
			printf("JUMPIFNEQ else$%i %s bool@true\n", ifCounter, result);
			break;

		case T_ELSE:
			result = "expr"; //výsledek expressionu
			printf("JUMP end$if$%i\n", ifCounter);
			printf("LABEL else$%i %s bool@true\n", ifCounter, result);
			break;
		
		case T_END:
			if(ifWhile == 1) //end v těle ifu
				printf("LABEL end$if$%i\n", ifCounter);
			else{ //end v těle whilu
				printf("JUMP while$%i\n", whileCounter);
				printf("LABEL end$while$%i\n", whileCounter);
			}
			break;

		case N_WHILE:
			ifWhile = 2; //je to while
			whileCounter++; //kolikátej je to while
			printf("LABEL while$%i\n", whileCounter);
			break;

		case T_DO:
			result = "expr";//výsledek expressionu
			printf("JUMPIFNEQ end$while$%i %s bool@true\n", whileCounter, result);
			break;
		default: break;
	}
}