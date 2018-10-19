#include "main.h"

int main(int argc, char const *argv[])
{
	pToken firstToken = NULL;
	scannerParse("body = zapocet / 8 + 2", firstToken);
	return 0;
}
