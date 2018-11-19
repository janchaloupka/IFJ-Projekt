#include "main.h"

int main(int argc, char const *argv[])
{
	return yellDebug();

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
	FILE *source = fopen("test-code.3", "r");
	
	pToken token = NULL;
	scannerGetTokenList(&token, source);
	scannerPrintTokenList(token);
	fclose(source);
	return 0;
}