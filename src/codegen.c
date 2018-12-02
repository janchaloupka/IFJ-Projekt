#include "codegen.h"

void codeFromToken(tType type, pToken token, psTree table){
	
	static char *ifWhile; 
	static int ifCounter = 0;
	static int whileCounter = 0;
	static bool alloc = false;
	static int endIndex = 0;
	static char *id;
	static char *funcId;
	static int params = 1;
	static bool inParams = false; //jsem v parametrech funkce
	static bool print = false;
	static int assign = false;
	static bool inFunc = false;//jsem ve funkci
	static bool defFunc = false;
	static bool defT = false; //jde o terminál def
	static char *assignId;
	static bool defVar = false; //false = proměnná je použita poporvé
	static bool builtIn = false;

	if(!alloc){ //pokud ještě neproběhla alokace pro ifWhile -> jen poprvé
		ifWhile = safeMalloc(STRING_CHUNK_SIZE * sizeof(char)); 
		alloc = true;
	}

	if((strlen(ifWhile) - 1) == 100){ //pokud je ifWhile plný
		ifWhile = safeRealloc(ifWhile, STRING_CHUNK_SIZE);
	}

	switch(type){
		char *result; //TODO: doplnit výsledek expressionu

		case T_ASSIGN:
			defVar = (symTabSearch(&table, id))->defined;
			if(!defVar){
				printf("DEFVAR LF@%s\n", id);
			}
			assign = true; //pro výpis move
			assignId = id;
			break;

		case T_ID:
			if(strcmp(token->data, "print") == 0){ //->výpis write instrukce
				print = true;
			}
			else if(strcmp(token->data, "inputi") == 0){ //LF nebo GF???
				printf("READ LF@%s int\n", id);
				assign = false; //protože nechci vypisovat move
				builtIn = true;
			}
			else if(strcmp(token->data, "inputf") == 0){ //LF nebo GF???
				printf("READ LF@%s float\n", id);
				assign = false; //protože nechci vypisovat move
				builtIn = true;
			}
			else if(strcmp(token->data, "inputs") == 0){
				printf("READ LF@%s string\n", id);
				assign = false; //protože nechci vypisovat move
				builtIn = true;
			}
			else if(defT){//je to id u definice funkce
				funcId = token->data;
				defT = false;
			}
			else{ //id funkcí a proměnných
				id = token->data;
			}
			break;

		case N_DEFVAR:

			break;

		case T_DEF:
			printf("CREATEFRAME\n");
			inParams = true;
			inFunc = true;
			defFunc = true;
			defT = true;
			break;

		case N_FUNC:
			if(!print && !builtIn){ //pokud nejde o funkci print
				funcId = id;
				inParams = true;
				inFunc = true;
			}
			break;

		case N_PARSN:
			if(0){}//-> aby si překladač nestěžoval
			tType prevTokenType = token->prevToken->type;
			char *prevTokenData = token->prevToken->data;
			char *type;
			switch(prevTokenType){//zjistím typ předchozího tokenu (terminál)
				case T_INTEGER:
				type = "int";
			 	break;
			case T_FLOAT:
				type = "float";
				break;
			case T_STRING:
				type = "string";
				break;
			case T_NIL:
				type = "nil";
				break;
			case T_TRUE:
				type = "true";
				break;
			case T_FALSE:
				type = "false";
				break;
			case T_ID:
				type = "id";
			default:
				break;
			}

			if(!print){//jde o funkci -> DEFVAR + MOVE
				printf("DEFVAR TF@%%%i\n", params);

				if(strcmp(type, "int") == 0)
					printf("MOVE TF@%%%i int@%s\n", params, prevTokenData);

				else if(strcmp(type, "float") == 0)
					printf("MOVE TF@%%%i float@%s\n", params, prevTokenData);

				else if(strcmp(type, "string") == 0){
					char *string = stringToInterpret(prevTokenData);
					printf("MOVE TF@%%%i %s\n", params, string);
				}

				else if(strcmp(type, "nil") == 0)
					printf("MOVE TF@%%%i nil@nil\n", params);

				else if(strcmp(type, "true") == 0)
					printf("MOVE TF@%%%i bool@true\n", params);

				else if(strcmp(type, "false") == 0)
					printf("MOVE TF@%%%i bool@false\n", params);

				else if(strcmp(type, "id") == 0) //-> výpis proměnné
					printf("MOVE TF@%%%i TF@%s\n", params, prevTokenData); 
				params++;
			}
			else{ //jde o print -> WRITE
				if(strcmp(type, "int") == 0)
					printf("WRITE int@%s\n", prevTokenData);
				
				else if(strcmp(type, "float") == 0)
					printf("WRITE float@%s\n", prevTokenData);

				else if(strcmp(type, "string") == 0){
					char *string = stringToInterpret(prevTokenData);
					printf("WRITE %s\n", string);
				}

				else if(strcmp(type, "nil") == 0)
					printf("WRITE nil@nil\n");	

				else if(strcmp(type, "true") == 0)
					printf("WRITE bool@true\n");

				else if(strcmp(type, "true") == 0)
					printf("WRITE bool@false\n");

				else if(strcmp(type, "id") == 0) //-> výpis proměnné
					printf("WRITE LF@%s\n", prevTokenData);  //bude tam vždycky LF???
			}
			break;

		case T_EOL: //nulování
			params = 1; //"vynuluju" počet parametrů 
			if(inParams){ //řádek s parametry
				printf("CALL $%s\n", funcId);
				//return hodnoty
				if(defFunc){ //TODO: tohle by mělo vypadat jinak (zatím nevím jak)
					//printf("JUMP $%s$end\n", funcId);
					printf("LABEL $%s\n", funcId);
					printf("PUSHFRAME\n");
					//návratová hodnota
					printf("DEFVAR LF@%%return\n");
					printf("MOVE LF@%%return nil@nil\n");
					//TODO: parametry

					defFunc = false;
				}
				inParams = false;
			}
			if(print){ //řádek s print funkcí
				print = false;
			}

			if(assign){ //řádek s přiřazením 
				//result = "expr"; //výsledek expressionu
				printf("POPS LF@%s\n", assignId);
				//printf("MOVE LF@%s %s\n", assignId, result);
				assign = false;
			}
			break;

		case T_THEN:
			result = "expr"; //výsledek expressionu 
			ifWhile[strlen(ifWhile) - endIndex] = 'i';
			ifCounter++; //kolikátej je to if
			printf("JUMPIFNEQ else$%i %s bool@true\n", ifCounter, result);
			break;

		case T_ELSE:
			result = "expr"; //výsledek expressionu
			printf("JUMP end$if$%i\n", ifCounter);
			printf("LABEL else$%i\n", ifCounter);
			break;
		
		case T_END:
			if(inFunc){//je to end funkce
				printf("LABEL $%s$end\n", funcId);
				inFunc = false;
			}
			else{
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
			}
			break;

		case N_WHILE:
			ifWhile[strlen(ifWhile) - endIndex] = 'w';
			whileCounter++; //kolikátej je to while
			printf("LABEL while$%i\n", whileCounter);
			break;

		case T_DO:
			result = "expr";//výsledek expressionu
			printf("JUMPIFNEQ end$while$%i %s bool@true\n", whileCounter, result);
			break;

		case T_EOF:
			if(alloc){ //uvolnění alokované paměti
				free(ifWhile);
				alloc = false;
			}
			break;
		default: break;
	}
}