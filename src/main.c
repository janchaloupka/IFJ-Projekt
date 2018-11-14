#include "main.h"

int main(int argc, char const *argv[])
{
	pToken token = NULL;
	while(token == NULL || token->type != SEP_EOF){
		scannerGetToken(stdin, &token);
		printf("\nTyp: ");
		scannerPrintType(token);
	}
	return 0;
}
