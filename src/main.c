#include "main.h"

int main(int argc, char const *argv[])
{
	pToken token = NULL;
	while(token == NULL || token->type != T_EOF){
		scannerGetToken(stdin, &token);
		printf("type: ");
		scannerPrintToken(token);
	}
	return scannerFoundError ? 1 : 0;
}
