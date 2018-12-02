#include "codegen.h"

void codeFromToken(tType type, pToken token, psTree table){
	
	static char *ifWhile; 
	static int ifCounter = 0;
	static int whileCounter = 0;
	static bool defFunc = false;
	static bool alloc = false;
	static int endIndex = 0;
	static char *id;
	static int params = 1;
	static bool inParams = false;
	static char *funcId;
	static bool func = false;
	static bool print = false;
	static bool assign = false;

	if(!alloc){ //pokud ještě neproběhla alokace pro ifWhile
		ifWhile = malloc(STRING_CHUNK_SIZE * sizeof(char)); 
		alloc = true;
	}

	if((strlen(ifWhile) - 1) == 100){ //pokud je ifWhile plný
		ifWhile = realloc(ifWhile, STRING_CHUNK_SIZE);
	}

	switch(type){
		char *result; //TODO: doplnit výsledek expressionu

		case T_ASSIGN:
			printf("DEFVAR %s\n", id);
			assign = true; //pro výpis move
			break;

		case T_DEF:
			defFunc = true; //->pro výpis volání funkce - možná není potřeba???
			break;

		case T_ID:
			if(defFunc == true){ //je to id u definice funkce
				funcId = token->data;
			}
			else if(strcmp(token->data, "print") == 0){ //->výpis write instrukce
				print = true;
			}
			else{
				id = token->data; //->pro volání funkce
			}
			break;


		case N_FUNC: //TODO: další vestavěné funkce
			if(!print){ //pokud nejde o funkci print, vytvořím rámec
				printf("CREATEFRAME\n");
				func = true;
				inParams = true;
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

				else if(strcmp(type, "string") == 0) //TODO: tohle není podle zadání, upravit!!!
					printf("MOVE TF@%%%i string@%s\n", params, prevTokenData);

				else if(strcmp(type, "nil") == 0)
					printf("MOVE TF@%%%i nil@nil\n", params);

				else if(strcmp(type, "true") == 0)
					printf("MOVE TF@%%%i bool@true\n", params);

				else if(strcmp(type, "false") == 0)
					printf("MOVE TF@%%%i bool@false\n", params);
				params++;
			}
			else{ //jde o print -> WRITE
				if(strcmp(type, "int") == 0)
					printf("WRITE int@%s\n", prevTokenData);
				
				else if(strcmp(type, "float") == 0)
					printf("WRITE float@%s\n", prevTokenData);

				else if(strcmp(type, "string") == 0) //TODO: tohle není podle zadání, upravit!!!
					printf("WRITE string@%s\n", prevTokenData);

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
				if(defFunc){ 
					printf("CALL $%s\n", funcId);
				}
				else if(func){
					printf("CALL $%s\n", id);
				}
				inParams = false;
			}
			if(print){ //řádek s print funkcí
				print = false;
			}

			if(assign){ //řádek s přiřazením 
				result = "expr"; //výsledek expressionu
				printf("MOVE LF@%s %s\n", id, result);
				assign = false;
			}
			break;

		case N_BODY: //-> generuju návěští funkcí
			if(defFunc){
				printf("LABEL $%s\n", funcId);
				printf("PUSHFRAME\n");
				defFunc = false;

			}
			else if(func){
				printf("LABEL $%s\n", id);
				printf("PUSHFRAME\n");
				func = false;
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
