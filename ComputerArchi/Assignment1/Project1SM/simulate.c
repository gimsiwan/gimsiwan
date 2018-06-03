#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NUMMEMORY 65536 /* maximum number of words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000

#define ADD 0
#define NOR 1
#define LW 2
#define SW 3
#define BEQ 4
#define JALR 5
#define HALT 6
#define NOOP 7

typedef struct stateStruct {
	int pc;
	int mem[NUMMEMORY];
	int reg[NUMREGS];
	int numMemory;
} stateType;

void noticeMachHalted(int count);
void printState(stateType *);
int convertNum(int num);

int
main(int argc, char *argv[])
{
	char line[MAXLINELENGTH];
	stateType state;
	FILE *filePtr;
	int count = 0;

	if (argc != 2) {
		printf("error: usage: %s <machine-code file>\n", argv[0]);
		exit(1);
	}

	filePtr = fopen(argv[1], "r");
	if (filePtr == NULL) {
		printf("error: can't open file %s", argv[1]);
		perror("fopen");
		exit(1);
	}

	memset(&state, 0, sizeof(stateType));

	/* read in the entire machine-code file into memory */
	for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL;
		state.numMemory++) {

		if (sscanf(line, "%d", state.mem + state.numMemory) != 1) {
			printf("error in reading address %d\n", state.numMemory);
			exit(1);
		}
		state.mem[state.numMemory] = atoi(line);
		printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
	}

	while (1) {
		int opcode = (state.mem[state.pc] & 0x1C00000) >> 22;
		int regA, regB, destReg, offsetField;

		printState(&state);
		count++;

		if(opcode == ADD){
			regA = (state.mem[state.pc] & 0x380000) >> 19;
			regB = (state.mem[state.pc] & 0x70000) >> 16;
			destReg = (state.mem[state.pc] & 0x7);
			
			state.reg[destReg] = state.reg[regA] + state.reg[regB];
			state.pc++;
		}
		else if (opcode == NOR) {
			regA = (state.mem[state.pc] & 0x380000) >> 19;
			regB = (state.mem[state.pc] & 0x70000) >> 16;
			destReg = (state.mem[state.pc] & 0x7);
			state.reg[destReg] = ~(state.reg[regA] | state.reg[regB]);
			state.pc++;
		}
		else if (opcode == LW) {
			regA = (state.mem[state.pc] & 0x380000) >> 19;
			regB = (state.mem[state.pc] & 0x70000) >> 16;
			offsetField = (state.mem[state.pc] & 0xFFFF);
			offsetField = convertNum(offsetField);
			state.reg[regB] = state.mem[offsetField + state.reg[regA]];
			state.pc++;

		}
		else if (opcode == SW) {
			regA = (state.mem[state.pc] & 0x380000) >> 19;
			regB = (state.mem[state.pc] & 0x70000) >> 16;
			offsetField = (state.mem[state.pc] & 0xFFFF);
			offsetField = convertNum(offsetField);
			state.mem[offsetField + state.reg[regA]] = state.reg[regB];
			state.pc++;
		}
		else if (opcode == BEQ) {
			regA = (state.mem[state.pc] & 0x380000) >> 19;
			regB = (state.mem[state.pc] & 0x70000) >> 16;
			if (state.reg[regA] == state.reg[regB]) {
				offsetField = (state.mem[state.pc] & 0xFFFF);
				offsetField = convertNum(offsetField);
				state.pc = state.pc + 1 + offsetField;
			}
			else state.pc++;
		}
		else if (opcode == JALR) {
			regA = (state.mem[state.pc] & 0x380000) >> 19;
			regB = (state.mem[state.pc] & 0x70000) >> 16;
			state.reg[regB] = state.pc + 1;
			state.pc = state.reg[regA];
		}
		else if (opcode == HALT) {
			state.pc++;
			noticeMachHalted(count);
			break;
		}
		else if (opcode == NOOP) {
			state.pc++;
		}
	}
	printState(&state);
	return(0);
}

void noticeMachHalted(int count) {

	printf("machine halted\n");
	printf("total of %d instructions executed\n", count);
	printf("final state of machine :\n");
}

void
printState(stateType *statePtr)
{
	int i;
	printf("\n@@@\nstate:\n");
	printf("\tpc %d\n", statePtr->pc);
	printf("\tmemory:\n");
	for (i = 0; i<statePtr->numMemory; i++) {
		printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]);
	}
	printf("\tregisters:\n");
	for (i = 0; i<NUMREGS; i++) {
		printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
	}
	printf("end state\n");
}

int
convertNum(int num)
{
	/* convert a 16-bit number into a 32-bit Linux integer */
	if (num & (1 << 15)) {
		num -= (1 << 16);
	}
	return(num);
}
