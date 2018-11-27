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

void generateBaseCode(){
	printf(".IFJcode18\n\
\n\
DEFVAR GF@$_line\n\
DEFVAR GF@$_col\n\
DEFVAR GF@$tmp\n\
\n\
MOVE GF@$_line string@TBD\n\
MOVE GF@$_col string@TBD\n\
JUMP $main\n\
\n\
LABEL $checkIfBool\n\
	CALL $getType\n\
	JUMPIFNEQ $printTypeError TF@%%return string@bool\n\
	RETURN\n\
\n\
LABEL $checkIfLtGt\n\
	CALL $getType\n\
	JUMPIFEQ $checkIfLtGt$end TF@%%return string@string\n\
	JUMPIFEQ $checkIfLtGt$end TF@%%return string@int\n\
	JUMPIFNEQ $printTypeError TF@%%return string@float\n\
	LABEL $checkIfLtGt$end\n\
	RETURN\n\
\n\
LABEL $checkIfEql\n\
	CALL $getType\n\
	\n\
	JUMPIFNEQ $decideEqlOp$diff TF@%%return string@different\n\
	PUSHS TF@rv\n\
	PUSHS TF@lv\n\
	RETURN\n\
	\n\
	LABEL $decideEqlOp$diff\n\
	PUSHS bool@false\n\
	PUSHS bool@false\n\
	RETURN\n\
\n\
LABEL $decideDivOp\n\
	JUMPIFEQ $decideDivOp$float TF@%%return string@float\n\
	IDIVS\n\
	RETURN\n\
	LABEL $decideDivOp$float\n\
	DIVS\n\
	RETURN\n\
\n\
LABEL $checkIfNum\n\
	CALL $getType\n\
	JUMPIFEQ $checkIfNum$end TF@%%return string@int\n\
	JUMPIFNEQ $printTypeError TF@%%return string@float\n\
	LABEL $checkIfNum$end\n\
	RETURN\n\
\n\
LABEL $getType\n\
	CREATEFRAME\n\
	DEFVAR TF@%%return\n\
	DEFVAR TF@lv\n\
	DEFVAR TF@lt\n\
	DEFVAR TF@rv\n\
	DEFVAR TF@rt\n\
	POPS TF@rv\n\
	POPS TF@lv\n\
	TYPE TF@lt TF@lv\n\
	TYPE TF@rt TF@rv\n\
	\n\
	# defaultne nejsou stejneho typu\n\
	MOVE TF@%%return string@different\n\
	\n\
	JUMPIFEQ $getType$if$1$1 TF@lt TF@rt\n\
	JUMPIFEQ $getType$if$1$2 TF@lt string@int\n\
	JUMPIFEQ $getType$if$1$3 TF@rt string@int\n\
	JUMP $getType$if$1$end\n\
	LABEL $getType$if$1$1 # jsou stejneho typu\n\
		MOVE TF@%%return TF@lt\n\
	JUMP $getType$if$1$end\n\
	LABEL $getType$if$1$2 # lt je int\n\
		JUMPIFNEQ $getType$if$2 TF@rt string@float\n\
			MOVE TF@%%return TF@rt\n\
			INT2FLOAT TF@lv TF@lv\n\
		LABEL $getType$if$2\n\
	JUMP $getType$if$1$end\n\
	LABEL $getType$if$1$3 # rt je int\n\
		JUMPIFNEQ $getType$if$3 TF@lt string@float\n\
			MOVE TF@%%return TF@lt\n\
			INT2FLOAT TF@rv TF@rv\n\
		LABEL $getType$if$3\n\
	LABEL $getType$if$1$end\n\
	\n\
	PUSHS TF@lv\n\
	PUSHS TF@rv\n\
	RETURN\n\
\n\
LABEL $printTypeError\n\
	WRITE string@\\010[RUNTIME]\\032Error\\032on\\032line\\032\n\
	WRITE GF@$_line\n\
	WRITE string@:\n\
	WRITE GF@$_col\n\
	WRITE string@\\032-\\032Type\\032error;\\032incompatible\\032types\\032(\n\
	WRITE TF@lt\n\
	WRITE string@\\032with\\032\n\
	WRITE TF@rt\n\
	WRITE string@)\n\
	EXIT int@4\n\
\n\
\n\
# Konec preddefinovanych funkci\n\
LABEL $main\n\
\n");
}