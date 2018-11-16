#include "main.h"

int main(int argc, char const *argv[])
{
	if(argc > 1){
		if(strcmp(argv[1], "j32") == 0) return janchDebug();

		if(strcmp(argv[1], "yell") == 0) return yellDebug();
	}

	// Hlavní program
	return 0;
}

int yellDebug(){
	FILE *source = fopen("test-code", "r");
	
	pToken token = NULL;
	scannerGetTokenList(&token, source);
	parser(&token);		// Zatím nedělá nic useful anyways

	return 0;
}
 
int janchDebug(){
	FILE *source = fopen("test-code", "r");
	
	pToken token = NULL;
	scannerGetTokenList(&token, source);
	scannerPrintTokenList(token);
	return 0;
}