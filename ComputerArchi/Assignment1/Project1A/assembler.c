#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAXLINELENGTH 1000
#define MAXLABELNUM 65536 //2^16
#define ADD 0
#define NOR 1
#define LW 2
#define SW 3
#define BEQ 4
#define JALR 5
#define HALT 6
#define NOOP 7

int readAndParse(FILE *, char *, char *, char *, char *, char *);
int isNumber(char *);
void checkRegister(char *arg);
void checkAddrField(char *arg);
int
main(int argc, char *argv[])
{
	char *inFileString, *outFileString;
	FILE *inFilePtr, *outFilePtr;
	char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH],
		arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];

	char labelArray[MAXLABELNUM][7];
	int labelAddress[MAXLABELNUM];

	int machLang = 0;
	int labelNum = 0;
	int address = 0;

	if (argc != 3) {
		printf("error: usage: %s <assembly-code-file> <machine-code-file>\n",
			argv[0]);
		exit(1);
	}

	inFileString = argv[1];
	outFileString = argv[2];

	inFilePtr = fopen(inFileString, "r");
	if (inFilePtr == NULL) {
		printf("error in opening %s\n", inFileString);
		exit(1);
	}
	outFilePtr = fopen(outFileString, "w");
	if (outFilePtr == NULL) {
		printf("error in opening %s\n", outFileString);
		exit(1);
	}

	/* here is an example for how to use readAndParse to read a line from
	inFilePtr */
	while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)) {

		int i = 0;

		if (strcmp(opcode, "add") && strcmp(opcode, "nor") && strcmp(opcode, "lw") && strcmp(opcode, "sw") && strcmp(opcode, "beq") && strcmp(opcode, "jalr") && strcmp(opcode, "halt") && strcmp(opcode, "noop") && strcmp(opcode, ".fill")) {
			printf("ERROR: Unrecognized opcodes");
			exit(1);
		}

		if ((strcmp(opcode, "halt") && strcmp(opcode, "noop") && strcmp(opcode, ".fill") && strcmp(opcode, "jalr") && arg2[0] == '\0') || (!strcmp(opcode, "jalr") && arg1[0] == '\0') || (!strcmp(opcode, ".fill") && arg0[0] == '\0')) {
			printf("ERROR: Not enough arguments");
			exit(1);
		}

		if (!strcmp(opcode, "add") || !strcmp(opcode, "nor") || !strcmp(opcode, "lw") || !strcmp(opcode, "sw") || !strcmp(opcode, "beq") || !strcmp(opcode, "jalr")) {
			checkRegister(arg0);
			checkRegister(arg1);
		}
		if (!strcmp(opcode, "add") || !strcmp(opcode, "nor")) {
			checkRegister(arg2);
		}
		if (!strcmp(opcode, "lw") || !strcmp(opcode, "sw") || !strcmp(opcode, "beq")){
			checkAddrField(arg2);
		}
		if (!strcmp(opcode, ".fill")){
			checkAddrField(arg0);
		}

		if (label[0] != '\0'){
			
			if (!((label[0] >= 'A' && label[0] <= 'Z') || (label[0] >= 'a' && label[0] <= 'z'))) {
				printf("ERROR: Label doesn't start with letter");
				exit(1);
			}

			if (strlen(label) >= 7) {
				printf("ERROR: Label lengh is too long");
				exit(1);
			}

			while (i < strlen(label)) {
				char temp = label[i];				
				if (!((temp >= 'A' && temp <= 'Z') || (temp >= 'a' && temp <= 'z') || isNumber(&temp))) {
					printf("ERROR: Label contains other than char and num");
					exit(1);
				}
				i++;
			}

			for (i = 0; i < labelNum; i++) {
				if (!strcmp(labelArray[i], label)) {
					printf("ERROR: Duplicate label name");
					exit(1);
				}
			}

			strcpy(labelArray[labelNum], label);
			labelAddress[labelNum] = address;
			labelNum++;
		}

		address++;
	}

	/* this is how to rewind the file ptr so that you start reading from the
	beginning of the file */
	rewind(inFilePtr);

	/* after doing a readAndParse, you may want to do the following to test the
	opcode */
	address = 0;

	while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)) {

		if (!strcmp(opcode, "add")) {
			machLang = (ADD << 22) | (atoi(arg0) << 19) | (atoi(arg1) << 16) | atoi(arg2);
		}
		else if (!strcmp(opcode, "nor")) {
			machLang = (NOR << 22) | (atoi(arg0) << 19) | (atoi(arg1) << 16) | atoi(arg2);
		}
		else if (!strcmp(opcode, "jalr")) {
			machLang = (JALR << 22) | (atoi(arg0) << 19) | (atoi(arg1) << 16);
		}
		else if (!strcmp(opcode, "halt")) {
			machLang = (HALT << 22);
		}
		else if (!strcmp(opcode, "noop")) {
			machLang = (NOOP << 22);
		}
		else if (!strcmp(opcode, ".fill")) {

			long long rangeCheck;

			if (isNumber(arg0)) {
				rangeCheck = atoll(arg0);
				if(rangeCheck > 2147483647 || rangeCheck < -2147483648){
					printf("ERROR : .fill number overflow");
					exit(1);
				}
				machLang = atoi(arg0);
			}
			else {
				int i = 0;
				for (i = 0; i < labelNum; i++) {
					if (!strcmp(labelArray[i], arg0)) break;
				}
				if (i >= labelNum) {
					printf("ERROR: Undefined label");
					exit(1);
				}
				machLang = labelAddress[i];
			}
		}
		else if (!strcmp(opcode, "lw") || !strcmp(opcode, "sw") || !strcmp(opcode, "beq")) {
			int offsetField;

			if (isNumber(arg2)) {
				offsetField = atoi(arg2);
			}
			else {
				int i = 0;
				for (i = 0; i < labelNum; i++) {
					if (!strcmp(labelArray[i], arg2)) break;
				}
				if (i >= labelNum) {
					printf("ERROR: Undefined label");
					exit(1);
				}
				offsetField = labelAddress[i];

				if (!strcmp(opcode, "beq")) {
					offsetField = offsetField - address - 1;
				}
			}

			if (offsetField < -32768 || offsetField > 32767) {
				printf("ERROR: Offset range is out");
				exit(1);
			}

			offsetField = offsetField & 0xFFFF;

			if (!strcmp(opcode, "beq")) {
				machLang = (BEQ << 22) | (atoi(arg0) << 19) | (atoi(arg1) << 16) | offsetField;
			}
			else if (!strcmp(opcode, "lw")) {
				machLang = (LW << 22) | (atoi(arg0) << 19) | (atoi(arg1) << 16) | offsetField;
			}
			else if (!strcmp(opcode, "sw")) {
				machLang = (SW << 22) | (atoi(arg0) << 19) | (atoi(arg1) << 16) | offsetField;
			}
		}
		fprintf(outFilePtr, "%d\n", machLang);
		address++;
	}

	return(0);
}



/*
* Read and parse a line of the assembly-language file.  Fields are returned
* in label, opcode, arg0, arg1, arg2 (these strings must have memory already
* allocated to them).
*
* Return values:
*     0 if reached end of file
*     1 if all went well
*
* exit(1) if line is too long.
*/
int
readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0,
	char *arg1, char *arg2)
{
	char line[MAXLINELENGTH];
	char *ptr = line;

	/* delete prior values */
	label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';

	/* read the line from the assembly-language file */
	if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL) {
		/* reached end of file */
		return(0);
	}

	/* check for line too long (by looking for a \n) */
	if (strchr(line, '\n') == NULL) {
		/* line too long */
		printf("error: line too long\n");
		exit(1);
	}

	/* is there a label? */
	ptr = line;
	if (sscanf(ptr, "%[^\t\n\r ]", label)) {
		/* successfully read label; advance pointer over the label */
		ptr += strlen(label);
	}

	/*
	* Parse the rest of the line.  Would be nice to have real regular
	* expressions, but scanf will suffice.
	*/
	sscanf(ptr, "%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]",
		opcode, arg0, arg1, arg2);
	return(1);
}

int
isNumber(char *string)
{
	/* return 1 if string is a number */
	int i;
	
	return((sscanf(string, "%d", &i)) == 1);
}

void checkRegister(char *arg) {

	int num;
	char c;

	if (sscanf(arg, "%d%c", &num, &c) != 1) {
		printf("ERROR: Register is not number");
		exit(1);
	}
	
	if (atoi(arg) < 0 || atoi(arg) > 7) {
		printf("ERROR: Register range out");
		exit(1);
	}
}

void checkAddrField(char *arg){

	int num;
	char c;

	if(isNumber(arg)){
		if(sscanf(arg, "%d%c", &num, &c) != 1){
			printf("ERROR: Address field is not appropriate");
			exit(1);
		}
	}
}
