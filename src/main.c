/**
 * @file main.c
 *
 * Hlavní část překladače
 *
 * IFJ Projekt 2018, Tým 13
 *
 * @author <xforma04> Klára Formánková
 * @author <xlanco00> Jan Láncoš
 * @author <xsebel04> Vít Šebela
 * @author <xchalo16> Jan Chaloupka
 */

#include "main.h"

#define SYNTAX_TESTS 11
#define SEMANTIC_TESTS 13

int main(int argc, char const *argv[]){

	if(argc > 1){
		if(strcmp(argv[1], "j32") == 0) return janchDebug();

		if(strcmp(argv[1], "yell") == 0) return yellDebug();

		if(strcmp(argv[1], "vita") == 0) return vitaDebug();
	}

	return 0;
}

int vitaDebug(){

	FILE *file_test[SEMANTIC_TESTS];
	FILE *file_expected[SEMANTIC_TESTS];

	char c = ' ';
	char test[30];
 	char testOut[30];
    pToken token = NULL;

	for (int i = 0; i < SYNTAX_TESTS; i++)
	{
		printf("\033[1;33m");
	  	printf("------------------- TEST_%d -------------------|\n", i+1);
	  	printf("\033[0m");
	  	sprintf(test, "tests/tests_syn/test%d", i+1);
	  	sprintf(testOut, "tests/tests_syn/test%d.out", i+1);

	  	if ((file_expected[i] = fopen(testOut, "r")) == NULL){
	  		printf("nepodarilo se otevrit soubor %s", testOut);
	  		continue;
	  	}
	  	if ((file_test[i] = fopen(test, "r")) == NULL){
	  		printf("nepodarilo se otevrit soubor %s", test);
	  		continue;
	  	}

	  	c = ' ';
	  	while (c != EOF) {
		    c = getc(file_expected[i]);
		    printf("%c",c);
	  	}
	  	printf("\n\n");

	  	scannerGetTokenList(&token, file_test[i]);
	  	parser(&token);
	  	fclose(file_test[i]);
	  	fclose(file_expected[i]);
		scannerFreeTokenList(&token);
		_scannerFSM(NULL, NULL);
		printf("\033[1;31m");
		printf("\n________________END OF TEST_%d_________________|\n", i+1);
		printf("\033[0m");
		if (i < SYNTAX_TESTS-1) printf("|\n|\n");
	}

	for (int i = 0; i < SEMANTIC_TESTS; i++)
	{
		printf("\x1B[32m");
	  	printf("------------------- TEST_%d -------------------|\n", i+1);
	  	printf("\033[0m");
	  	sprintf(test, "tests/tests_sem/test%d", i+1);
	  	sprintf(testOut, "tests/tests_sem/test%d.out", i+1);

	  	if ((file_expected[i] = fopen(testOut, "r")) == NULL){
	  		printf("nepodarilo se otevrit soubor %s", testOut);
	  		continue;
	  	}
	  	if ((file_test[i] = fopen(test, "r")) == NULL){
	  	printf("nepodarilo se otevrit soubor %s", test);
	  		continue;
	  	}

	  	c = ' ';
	  	while (c != EOF) {
		    c = getc(file_expected[i]);
		    printf("%c",c);
	  	}
	  	printf("\n\n");

	  	scannerGetTokenList(&token, file_test[i]);
	  	parser(&token);
	  	fclose(file_test[i]);
	  	fclose(file_expected[i]);
		scannerFreeTokenList(&token);
		_scannerFSM(NULL, NULL);
		printf("\x1B[34m");
		printf("\n________________END OF TEST_%d_________________|\n", i+1);
		printf("\033[0m");
		if (i < SEMANTIC_TESTS-1) printf("|\n|\n");
	}
  return 0;
}

int yellDebug(){
	FILE *source1 = fopen("tests/test-code.1", "r");

	pToken token1 = NULL;

	scannerGetTokenList(&token1, source1);
	parser(&token1);
	fclose(source1);
	scannerFreeTokenList(&token1);

	return 0;
}

int janchDebug(){
	FILE *source = fopen("tests/test-expr", "r");

	//psTree localId;
	//symTabInit(&localId);
	//psData data = malloc(sizeof(struct sData));
	//data->localFrame = NULL;
	//symTabInsert(&localId, "a", data);

	pToken token = NULL;
	scannerGetTokenList(&token, source);
	generateBaseCode();
	//printf("CREATEFRAME\nPUSHFRAME\nDEFVAR LF@a\nMOVE LF@a string@hello\n");
	//parser(&token);
	exprParse(&token, NULL/*localId*/);
	printf("\nPOPS GF@$tmp\nWRITE GF@$tmp\nWRITE string@\\010\n");

	fclose(source);
	scannerFreeTokenList(&token);
	//symTabDispose(&localId);
	return 0;
}
