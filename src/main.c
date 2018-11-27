#include "main.h"

int main(int argc, char const *argv[])
{

	if(argc > 1){
		if(strcmp(argv[1], "j32") == 0) return janchDebug();

		if(strcmp(argv[1], "yell") == 0) return yellDebug();
	}
	
	return 0;
}

int yellDebug(){
	FILE *source = fopen("test-code", "r");
	
	pToken token = NULL;
	scannerGetTokenList(&token, source);
	parser(&token);
	fclose(source);
	scannerFreeTokenList(&token);

	return 0;
}
 
int janchDebug(){
	FILE *source = fopen("tests/test-expr", "r");
	
	pToken token = NULL;
	scannerGetTokenList(&token, source);
	printf(".IFJcode18\nDEFVAR GF@$tmp\nCLEARS\n\n");
	exprParse(&token, NULL);
	printf("\nPOPS GF@$tmp\nWRITE GF@$tmp\n");
	
	fclose(source);
	scannerFreeTokenList(&token);
	return 0;
}