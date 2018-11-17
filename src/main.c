#include "main.h"

int main(int argc, char const *argv[])
{
	if(argc > 1){
		if(strcmp(argv[1], "j32") == 0) return janchDebug();
	}

	// Hlavní program
	return 0;
}
 
int janchDebug(){
	FILE *source = fopen("test-code", "r");
	
	pToken token = NULL;
	scannerGetTokenList(&token, source);
	scannerPrintTokenList(token);
	fclose(source);
	return 0;
}