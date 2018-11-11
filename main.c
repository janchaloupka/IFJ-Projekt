#include "main.h"

int main(int argc, char const *argv[])
{
	pToken firstToken = NULL;
	char *test = "8 * 2 > 10\n# Toto je comment\n10.25e-20";
	scannerParse(test, &firstToken);
	
	printf("Vysledek pro kod:");
	printf("\n------------\n");
	printf(test);
	printf("\n============\n");
	scannerDebugPrint(firstToken);

	return 0;
}
