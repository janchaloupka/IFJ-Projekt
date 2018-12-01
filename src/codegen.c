#include "codegen.h"

void codeFromToken(tType type, pToken token, psTree table){
	
	char *ifWhile; 
	static int ifCounter = 0;
	static int whileCounter = 0;
	static bool defFunc = false;
	static bool alloc = false;
	//static char *id;

	if(!alloc){ //pokud ještě neproběhla alokace pro ifWhile
		ifWhile = malloc(STRING_CHUNK_SIZE * sizeof(char));
		alloc = true;
	}

	if((strlen(ifWhile) - 1) == 100){ //pokud je ifWhile plný
		ifWhile = realloc(ifWhile, STRING_CHUNK_SIZE);
	}

	switch(type){
		char *result; //výsledek expressionu
		case N_DEFVAR: //TODO
			printf("DEFVAR\n");
			break;


		case N_DEFUNC:
			defFunc = true;
			break;

		case T_ID:
			if(defFunc == true){ //je to id u definice funkce
				char *funcId = token->data;
				printf("LABEL %s\n", funcId);
				printf("PUSHFRAME\n");
				defFunc = false;	
			}
			break;

		case T_THEN:
			result = "expr"; //výsledek expressionu 

			ifWhile[strlen(ifWhile)] = 'i';
			ifCounter++; //kolikátej je to if
			printf("JUMPIFNEQ else$%i %s bool@true\n", ifCounter, result);
			break;

		case T_ELSE:
			result = "expr"; //výsledek expressionu
			printf("JUMP end$if$%i\n", ifCounter);
			printf("LABEL else$%i\n", ifCounter);
			break;
		
		case T_END:
			printf("%s\n", ifWhile);
			int endIndex; //jak daleko od konce je písmeno
			for(endIndex = 1; ifWhile[strlen(ifWhile) - endIndex] == '0'; endIndex++); //projde nuly na konci ifWhile 
			
			if(ifWhile[strlen(ifWhile) - endIndex] == 'i'){ //je to if
				ifWhile[strlen(ifWhile) - endIndex] = '0';
				printf("LABEL end$if$%i\n", ifCounter);
			}
			else{ //je to while
				ifWhile[strlen(ifWhile) - endIndex] = '0';
				printf("JUMP while$%i\n", whileCounter);
				printf("LABEL end$while$%i\n", whileCounter);
			}
			break;

		case N_WHILE:
			ifWhile[strlen(ifWhile)] = 'w';
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