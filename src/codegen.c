#include "codegen.h"


void codeFromToken(tType type, pToken token, psTree table){
	static int ifCounter = 0;
	static int whileCounter = 0;
	static bool stackSize = IFWHILE_STACK_CHUNK_SIZE;
	static bool *stack = NULL; // Stack if/while, true = if
	static int stackTop = 0;

	static char *id; // Poslední načtené id
	
	static int assign = false; // Provádí se přiřazení do proměnné
	static char *assignId; // Id proměnné, do které se přiřazuje
	
	static int params = 1; // Počet parametrů
	
	static bool callFunc = false; // Volá se funkce
	static bool callPrint = false; // Jedná se o volání print
	static char *callId; // Id volané funkce
	
	static bool defFunc = false; // Definujeme funkci
	static bool defTerm = false; // Přečetli jsme terminál def
	static char *defId = false; // Id definované funkce
	static bool defParams = false; // Definujeme parametry funkce


	if(stack == NULL){ //pokud ještě neproběhla alokace pro ifWhile -> jen poprvé
		stack = safeMalloc(stackSize * sizeof(bool)); 
	}

	if(stackTop + 1 >= stackSize){ //pokud je ifWhile plný
		stackSize += IFWHILE_STACK_CHUNK_SIZE;
		stack = safeRealloc(stack, stackSize * sizeof(bool));
	}

	switch(type){
		case T_ASSIGN:
			assign = true; //pro výpis move
			assignId = id;
			break;

		case T_ID:
			if(strcmp(token->data, "print") == 0){ // Výpis write instrukce
				callPrint = true;
			}else if(defTerm){ // Je to id definice funkce
				defId = token->data;
				printf("JUMP %s$end\nLABEL %s$body\n", defId, defId);
				defTerm = false;
			}else{ // Id funkcí a proměnných
				id = token->data;
			}
			break;

		case T_DEF:
			defParams = true;
			defFunc = true;
			defTerm = true;
			break;

		case N_DEFVARID:
		case N_BODY_ID:
			if(token->type == T_EOL){
				pToken prev = token->prevToken;
				if(prev->type == T_ID){
					printf("CREATEFRAME\n");
					if(symTabSearch(&table, prev->data) == NULL){
						// je to funkce
						printf("CALL %s\n", prev->data);
					}else{
						// je to proměnná
						printf("DEFVAR TF@$return\nMOVE TF@$return LF@%s\n", prev->data);
					}
				}
			}
			break;

		case N_FUNC:
			printf("CREATEFRAME\n");
			if(!callPrint){ //pokud nejde o funkci print
				callId = id;
				callFunc = true;
			}
			break;

		case N_PARSN:
			{}//-> aby si překladač nestěžoval
			pToken prevToken = token->prevToken;

			char *tokenVal = NULL;
			switch(prevToken->type){ //zjistím typ předchozího tokenu (terminál)
				case T_INTEGER:
					tokenVal = intToInterpret(prevToken->data);
					break;
				case T_FLOAT:
					tokenVal = floatToInterpret(prevToken->data);
					break;
				case T_STRING:
					tokenVal = stringToInterpret(prevToken->data);
					break;
				case T_NIL:
					tokenVal = nilToInterpret();
					break;
				case T_TRUE:
					tokenVal = trueToInterpret();
					break;
				case T_FALSE:
					tokenVal = falseToInterpret();
					break;
				case T_ID:
					tokenVal = varToInterpret(prevToken->data);
				default: break;
			}
			
			if(tokenVal == NULL) break;
			else if(callPrint){ // Jde o volání funkce print -> WRITE <hodnota>
				printf("WRITE %s\n", tokenVal);
			}else if(defParams){ // Jde o definici funkce
				//printf("DEFVAR %s\nMOVE %s LF@%%%i\n", tokenVal, tokenVal, params);
				printf("MOVE %s LF@%%%i\n", tokenVal, params);
				params++;
			}else{ // Jde o volání funkce -> DEFVAR + MOVE
				printf("DEFVAR TF@%%%i\n", params);
				printf("MOVE TF@%%%i %s\n", params, tokenVal);
				params++;
			}
			
			free(tokenVal);
			break;

		case T_EOL: //nulování
			params = 1; // "vynuluju" počet parametrů 

			if(callFunc){ // Volání funkce
				printf("CALL %s\n", callId);
				callFunc = false;
			}

			if(callPrint){ //řádek s print funkcí
				printf("DEFVAR TF@$return\nMOVE TF@$return nil@nil\n");
				callPrint = false;
			}

			if(defParams){
				printf("DEFVAR LF@$return\nCREATEFRAME\nDEFVAR TF@$return\nMOVE TF@$return nil@nil\n");
				defParams = false;
			}

			if(assign){ //řádek s přiřazením
				printf("MOVE LF@%s TF@$return\n", assignId);
				assign = false;
			}
			break;

		case T_THEN:
			stack[stackTop] = true;
			stackTop++;
			ifCounter++; //kolikátej je to if
			printf("CALL $checkIfReturnBool\n");
			if(defFunc)
				 printf("JUMPIFNEQ %s$if$%i$else TF@$return bool@true\nMOVE TF@$return nil@nil\n", defId, ifCounter);
			else
				 printf("JUMPIFNEQ $body$if$%i$else TF@$return bool@true\nMOVE TF@$return nil@nil\n", ifCounter);
			break;

		case T_ELSE:
			if(defFunc){
				printf("JUMP %s$if$%i$end\n", defId, ifCounter);
				printf("LABEL %s$if$%i$else\nMOVE TF@$return nil@nil\n", defId, ifCounter);
			}else{
				printf("JUMP $body$if$%i$end\n", ifCounter);
				printf("LABEL $body$if$%i$else\nMOVE TF@$return nil@nil\n", ifCounter);
			}
			break;
		
		case T_END:
			if(stackTop > 0){
				stackTop--;
				if(stack[stackTop]){
					// je if
					if(defFunc)
						printf("LABEL %s$if$%i$end\n", defId, ifCounter);
					else
						printf("LABEL $body$if$%i$end\n", ifCounter);
					//ifCounter--;
				}else{
					// je while
					if(defFunc){
						printf("JUMP %s$while$%i$start\n", defId, whileCounter);
						printf("LABEL %s$while$%i$end\n", defId, whileCounter);
					}else{
						printf("JUMP $body$while$%i$start\n", whileCounter);
						printf("LABEL $body$while$%i$end\n", whileCounter);
					}
					// While vždycky returnuje nil
					printf("CREATEFRAME\nDEFVAR TF@$return\nMOVE TF@$return nil@nil\n");
					//whileCounter--;
				}
			}else if(defFunc){//je to end funkce
				printf("MOVE LF@$return TF@$return\nPOPFRAME\nRETURN\nLABEL %s\nPUSHFRAME\n", defId);
				symTabDefvarPre(table);
				printf("JUMP %s$body\nLABEL %s$end\n", defId, defId);
				defFunc = false;
			}
			break;

		case T_WHILE:
			stack[stackTop] = false;
			stackTop++;
			whileCounter++; //kolikátej je to while
			if(defFunc)
				printf("LABEL %s$while$%i$start\n", defId, whileCounter);
			else
				printf("LABEL $body$while$%i$start\n", whileCounter);
			break;

		case T_DO:
			if(defFunc)
				 printf("JUMPIFNEQ %s$while$%i$end TF@$return bool@true\n", defId, whileCounter);
			else
				 printf("JUMPIFNEQ $body$while$%i$end TF@$return bool@true\n", whileCounter);
			break;

		case T_EOF:
			if(stack != NULL){ //uvolnění alokované paměti
				free(stack);
				stack = NULL;

				printf("EXIT int@0\nLABEL $main\nCREATEFRAME\nPUSHFRAME\n");
				symTabDefvarPre(table);
				printf("JUMP $main$main\n");
			}
			break;
		default: break;
	}
}