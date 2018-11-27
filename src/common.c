#include "common.h"

char *stringToInterpret(char *rawString){
	int rawLen = strlen(rawString);
	
	int max = rawLen + 1;
	int pos = 7;
	char *out = malloc(sizeof(char) * (max + 20));
	strcpy(out, "string@");

	for(int i = 1; i < rawLen - 1; i++){
		if(max <= pos + 4){
			max += 100;
			out = realloc(out, sizeof(char) * max);
		}
		
		out[pos] = '\\';
		if(rawString[i] == '\\'){
			i++;
			if(rawString[i] == '"'){
				out[pos] = '"';
				pos -= 3;
			}else if(rawString[i] == 'n'){
				out[pos+1] = '0';
				out[pos+2] = '1';
				out[pos+3] = '0';
			}else if(rawString[i] == 't'){
				out[pos+1] = '0';
				out[pos+2] = '0';
				out[pos+3] = '9';
			}else if(rawString[i] == '\\'){
				out[pos+1] = '0';
				out[pos+2] = '9';
				out[pos+3] = '2';
			}else if(rawString[i] == 'x'){
				int l = isxdigit(rawString[i+2])?2:1;
				char hex[3] = {0};
				hex[0] = rawString[i+1];
				if(l > 1) hex[1] = rawString[i+2];
				i += l;
				sprintf(&out[pos+1], "%03ld", strtol(hex, NULL, 16));
			}else{
				out[pos+1] = '0';
				out[pos+2] = '3';
				out[pos+3] = '2';
			}
			pos += 3;
		}else if(rawString[i] == '#'){
			out[pos+1] = '0';
			out[pos+2] = '3';
			out[pos+3] = '5';
			pos += 3;
		}else if(rawString[i] <= ' '){
			out[pos+1] = '0';
			out[pos+2] = '3';
			out[pos+3] = '2';
			pos += 3;
		}else{
			out[pos] = rawString[i];
		}

		pos++;
	}

	out[pos] = '\0';
	out = realloc(out, sizeof(char) * (pos + 1));
	return out;
}

char *intToInterpret(char *rawInt){
	char *out = malloc(sizeof(char) * (strlen(rawInt) + 5));
	
	strcpy(out, "int@");
	strcat(out, rawInt);
	
	return out;
}

char *floatToInterpret(char *rawFloat){
	char *out = malloc(sizeof(char) * (50));
	sprintf(out, "float@%a", strtod(rawFloat, NULL));
	return realloc(out, strlen(out));
}

char *trueToInterpret(){
	char *out = malloc(sizeof(char) * 10);
	strcpy(out, "bool@true");
	return out;
}

char *falseToInterpret(){
	char *out = malloc(sizeof(char) * 11);
	strcpy(out, "bool@false");
	return out;
}

char *nilToInterpret(){
	char *out = malloc(sizeof(char) * 8);
	strcpy(out, "nil@nil");
	return out;
}

char *varToInterpret(char *id){
	char *out = malloc(sizeof(char) * (strlen(id) + 4));

	strcpy(out, "LF@");
	strcat(out, id);

	return out;
}

char *funcToInterpret(char *id){
	char *out = malloc(sizeof(char) * (strlen(id) + 4));

	strcpy(out, "GF@");
	strcat(out, id);

	return out;
}