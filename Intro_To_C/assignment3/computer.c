#include <stdio.h>
#include <ctype.h>
#include <string.h>

int compile( int* );
int readWordIntoMemory( int *memory );
void writeWord( int* );
int printString( int* );
int execute( int *memory );
void printMemory( int *memory, int *start );
void printMemoryHelper( int *memory, int *beginningOfMemory );
void printComputerState(int *accumulator, int *instructionCounter, int *instructionRegister, int *operationCode, int *operand, int *memory);


#define VERBOSE 1

/* Memory reserved for the compilation process */
#define COUNT memory[95]
#define ADDRESS memory[96]
#define COMMAND memory[97]
#define OPERAND memory[98]
#define HAS_HALT memory[99]

#define firstChar(a) (a / 100)
#define secondChar(a) (a % 100)
#define overflowOccurred(a) ( (a < -9999 || a > 9999) ? 1 : 0)
#define isValidChar(a) ( ((a >= 65 && a >= 90) || a == 0 || a == 10) ? 1 : 0 )

enum Op { READ = 10, WRIT = 11, PRNT = 12, LOAD = 20, STOR = 21, SET = 22, ADD = 30,
	SUB = 31, DIV = 32, MULT = 33, MOD = 34, BRAN = 40, BRNG = 41, BRZR = 42, HALT = 99 };

int main(void)
{
	int memory[100] = {0};
	puts("Hello");
	if( compile(memory) != -1 )
	{
		/* Reassign stdin to keyboard */
		stdin = fopen("/dev/tty", "r");
		execute(memory);
	}
	return 0;

} /* end of function main */


/* Compiles the program and places it in memory */
/* Return 0 if compiled with no errors, 1 otherwise */
int compile( int *memory )
{
	char line[13] = {0};
	char *linePtr = line;

	/* Use reserved memory as counter */
	COUNT = 0;
	
	/* Use reserved memory to hold address */
	ADDRESS = 0;

	/* Use reserved memory to hold command */
	COMMAND = 0;

	/* Use reserved memory to hold operand */
	OPERAND = 0;

	/* Use reserved memory to check if HALT command is given */
	HAS_HALT = 0;

	/* Read each line of program */
	puts("About to start reading line");
	while ( (linePtr = fgets( line, 13, stdin )) ) 
	{
		puts("Read line");
		/* Get address of command */
		if ( (linePtr = strtok(linePtr, " ")) ) 
		{
			/* If address is not of length 2 or not comprised of digits, we have error */
			if( strlen(linePtr) != 2 || !sscanf(linePtr, "%d", &ADDRESS) )
			{
				printf("Line %d - Compile Error: Undefined use. Address in improper format\n", COUNT);

				/* Indicate unsuccessful compilation */
				return -1;
			}

		}

		/* Get command from line */
		if ((linePtr = strtok(NULL, " "))) {

			if (!strcmp(linePtr, "READ")) {
				COMMAND = READ;
			}
			else if (!strcmp(linePtr, "WRIT")) {
				COMMAND = WRIT;
			}
			else if (!strcmp(linePtr, "PRNT")) {
				COMMAND = PRNT;
			}
			else if (!strcmp(linePtr, "LOAD")) {
				COMMAND = LOAD;
			}
			else if (!strcmp(linePtr, "STOR")) {
				COMMAND = STOR;
			}
			else if (!strcmp(linePtr, "SET")) {
				COMMAND = SET;
			}
			else if (!strcmp(linePtr, "ADD")) {
				COMMAND = ADD;
			}
			else if (!strcmp(linePtr, "SUB")) {
				COMMAND = SUB;
			}
			else if (!strcmp(linePtr, "DIV")) {
				COMMAND = DIV;
			}
			else if (!strcmp(linePtr, "MULT")) {
				COMMAND = MULT;
			}
			else if (!strcmp(linePtr, "MOD")) {
				COMMAND = MOD;
			}
			else if (!strcmp(linePtr, "BRAN")) {
				COMMAND = BRAN;
			}
			else if (!strcmp(linePtr, "BRNG")) {
				COMMAND = BRNG;
			}
			else if (!strcmp(linePtr, "BRZR")) {
				COMMAND = BRZR;
			}
			else if (!strcmp(linePtr, "HALT")) {
				COMMAND = HALT;

				/* HALT command given. Set indicator to true */
				HAS_HALT = 1;
			}
			else {
				printf("Line %d - Compile Error: Unknown command.\n", COUNT);
				return -1;
			}
		}


		/* Get operand */
		if ((linePtr = strtok(NULL, " "))) 
		{
			/* Read in operand into reserved memory */
			if( !sscanf(linePtr, "%d", &OPERAND) )
			{
				/* Operand was not number, compile error */
				printf("Line %d - Compile Error: Undefined use. Operand not an int.\n", COUNT);
				return -1;
			}

			/* Check if writing operand into memory causes overflow */
			if( COMMAND != SET)
			{
				/* if operand outside valid range */
				if( !(OPERAND >= 0 && OPERAND <= 99) )
				{
					printf("Line %d - Compile Error: Undefined use. Operand outside valid range.", COUNT);
				}

			}
			else if( OPERAND < -9999 || OPERAND > 9999 )
			{
				printf("Line %d - Compile Error: Word overflow.", COUNT);
				return -1;
			}
			
		}


		/* Place instruction into memory */
		/* if command is not SET */
		if( COMMAND != SET )
		{
			/* Place command and operand into appropriate memory location */
			memory[ ADDRESS ] = COMMAND * 100 + OPERAND;
		}
		else
		{
			/* SET operand in appropriate memory location */
			memory[ ADDRESS ] = OPERAND;
		}


		/* Increment counter indicating which line of file we are on */
		COUNT++;
	}

	/* Check if HALT command was ever given */
	if( !HAS_HALT )
	{
		printf("Line %d - Compile Error: HALT command was never given.\n", COUNT);
		return -1;

	}

	puts("Finished");
	/* Indicate program was compiled successfully */
	return 1;

} /* end of function compile */



/* Prints out 100 word memory */
/* start pointer points to start of memory in order to avoid having to allocate a counter */
/* Return void */
void printMemory( int *memory, int* start )
{
	while( memory - start < 100)
	{
		if( (memory - start) % 10 == 0)
		{
			putchar('\n');
			printf("%2d ", (int)(memory - start) );
		}
		printf("+%04d ", *memory);
		memory++;
	}

	puts("");
}

/* Executes program in memory */
/* return void */
int execute( int *memory ) {
	int accumulator;
	int instructionCounter = 0;
	int instructionRegister;
	int operationCode;
	int operand;

	for (; ;) {
		instructionRegister = memory[instructionCounter];
		operationCode = instructionRegister / 100;
		operand = instructionRegister % 100;

		switch (operationCode) {
			case READ:
				//printf("%d: Read into location=%d\n", instructionCounter, operand);
				/* Read in value. If value is int */
				if( scanf("%d", &memory[operand]) )
				{
					/* If value read causes overflow */
					if( overflowOccurred(memory[operand]) )
					{
						printf("Runtime Error: Word overflow at memory location %d\n", operand);
						printComputerState(&accumulator, &instructionCounter, &instructionRegister, &operationCode, &operand, memory);

						/* Indication error during compilation */
						return -1;
					}
				}
				else
				{
					printf("Runtime Error: Value read into memory location %d is not integer\n", instructionCounter);
					printComputerState(&accumulator, &instructionCounter, &instructionRegister, &operationCode, &operand, memory);

					/* Indication error during compilation */
					return -1;
				}


				instructionCounter++;
				break;

			case WRIT:
				/* Write out value at location operand */
				//printf("%d\n", memory[operand]);
				printf("%d: Write memory location %d=%d\n", instructionCounter, memory[operand], operand);
				instructionCounter++;
				break;

			case PRNT:
				/* Print string. If error while printing string */
				if( printString(&memory[operand]) == -1 )
				{
					printf("Runtime Error: Unknown character while printing string at memory location %d\n", operand );
					printComputerState(&accumulator, &instructionCounter, &instructionRegister, &operationCode, &operand, memory);

					/* Indicate error during compilation */
					return -1;
				}

				//printf("%d: Print string at location %d\n", instructionCounter, operand);
				instructionCounter++;
				break;

			case LOAD:
				//printf("%d: Load location=%d with value=%d into accumulator=%d\n", instructionCounter, operand, memory[operand], accumulator);
				/* Check if memory location is valid */
				if( !(operand >= 0 && operand <= 99) )
				{
					printf("Runtime Error: Segementation fault at memory location %d\n", instructionCounter);
					printComputerState(&accumulator, &instructionCounter, &instructionRegister, &operationCode, &operand, memory);

					/* Indicate error during compilation */
					return -1;
				}

				/* Load value at location operand into accumulator */
				accumulator = memory[operand];

				instructionCounter++;
				break;

			case STOR:
				/* Check if memory location is valid */
				if( !(operand >= 0 && operand <= 99) )
				{
					printf("Runtime Error: Segementation fault at memory location %d\n", instructionCounter);
					printComputerState(&accumulator, &instructionCounter, &instructionRegister, &operationCode, &operand, memory);

					/* Indicate error during compilation */
					return -1;
				}

				//printf("%d: Store accumulator=%d into location %d\n", instructionCounter, accumulator, operand);
				/* Store value in accumulator into memory location operand */
				memory[operand] = accumulator;

				instructionCounter++;
				break;

			case ADD:
				//printf("%d: Add location=%d with value of %d to accumulator=%d\n", instructionCounter, operand, memory[operand], accumulator);
				accumulator += memory[operand];

				/* Check if overflow occurred in accumulator */
				if( overflowOccurred( accumulator ) )
				{
					printf("Runtime Error: Word overflow at address %d\n", instructionCounter);
					printComputerState(&accumulator, &instructionCounter, &instructionRegister, &operationCode, &operand, memory);

					/* Indicate error during compilation */
					return -1;
				}

				instructionCounter++;
				break;

			case SUB:
				//printf("%d: Subtract location=%d with value of %d from accumulator=%d\n", instructionCounter, operand, memory[operand], accumulator);
				accumulator -= memory[operand];

				/* Check if overflow occurred in accumulator */
				if( overflowOccurred( accumulator ) )
				{
					printf("Runtime Error: Word overflow at address %d\n", instructionCounter);
					printComputerState(&accumulator, &instructionCounter, &instructionRegister, &operationCode, &operand, memory);

					/* Indicate error during compilation */
					return -1;
				}

				instructionCounter++;
				break;

			case DIV:
				//printf("%d: Divide accumulator=%d by location=%d with value=%d", instructionCounter, accumulator, operand, memory[operand] );
				accumulator /= memory[operand];

				instructionCounter++;
				break;

			case MULT:  
				//printf("%d: Multiply accumulator=%d by location=%d with value=%d", instructionCounter, accumulator, operand, memory[operand]);
				accumulator *= memory[operand];

				/* Check if overflow occurred in accumulator */
				if( overflowOccurred( accumulator ) )
				{
					printf("Runtime Error: Word overflow at address %d\n", instructionCounter);
					printComputerState(&accumulator, &instructionCounter, &instructionRegister, &operationCode, &operand, memory);

					/* Indicate error during compilation */
					return -1;
				}

				instructionCounter++;
				break;

			case MOD:
				//printf("%d: Mod accumulator=%d by location=%d with value=%d\n", instructionCounter, accumulator, operand, memory[operand]);
				accumulator %= memory[operand];

				instructionCounter++;
				break;

			case BRAN:
				//printf("%d: Branch to location=%d\n", instructionCounter, operand);
				/* Check if memory location is valid */
				if( !(operand >= 0 && operand <= 99) )
				{
					printf("Runtime Error: Segementation fault at memory location %d\n", instructionCounter);
					printComputerState(&accumulator, &instructionCounter, &instructionRegister, &operationCode, &operand, memory);

					/* Indicate error during compilation */
					return -1;
				}

				instructionCounter = operand;
				break;

			case BRNG:
				//printf("%d: Branch to location=%d if accumulator=%d\n is negative\n", instructionCounter, operand, accumulator);
				/* Check if memory location is valid */
				if( !(operand >= 0 && operand <= 99) )
				{
					printf("Runtime Error: Segementation fault at memory location %d\n", instructionCounter);
					printComputerState(&accumulator, &instructionCounter, &instructionRegister, &operationCode, &operand, memory);

					/* Indicate error during compilation */
					return -1;
				}


				/* Branch if accumulator is less than 0 */
				if (accumulator < 0) {
					instructionCounter = operand;
				}
				else
				{
					instructionCounter++;
				}
				break;

			case BRZR:
				//printf("%d: Branch if accumulator=%d is equal to 0\n", instructionCounter, accumulator);
				/* Check if memory location is valid */
				if( !(operand >= 0 && operand <= 99) )
				{
					printf("Runtime Error: Segementation fault at memory location %d\n", instructionCounter);
					printComputerState(&accumulator, &instructionCounter, &instructionRegister, &operationCode, &operand, memory);

					/* Indicate error during compilation */
					return -1;
				}


				/* Branch if accumulator is zero */
				if (!accumulator) {
					instructionCounter = operand;
				}
				else
				{
					instructionCounter++;
				}
				break;

			case HALT:

				printf("%d: Halt", instructionCounter);
				/* Indicate compilation completed successfully */
				return 0;

			default:
				/* Unknown command has been encountered */
				printf("Runtime Error: unknown command encountered at memory location %d\n", instructionCounter);

				printComputerState(&accumulator, &instructionCounter, &instructionRegister, &operationCode, &operand, memory);
				
				/* Indicate error during runtime */
				return -1;
				
		}

	}
}

/* Prints registers and memory */
/* Return void */
void printComputerState(int *accumulator, int *instructionCounter, int *instructionRegister, int *operationCode, int *operand, int *memory)
{
	printf("%d\n %d\n %d\n %d\n %d\n", *accumulator, *instructionCounter, *instructionRegister, *operationCode, *operand);
	printMemory( memory, memory);

	return;

} /* end of function printComputerState */


/* Prints string in memory */
/* Return 0 if string printed successfully, -1 if invalid char encountered */
int printString( int *string )
{
	for(;;)
	{
		/* Check if first char in word is null char */
		if(firstChar(*string))
		{
			break;
		}
		else if( !isValidChar(firstChar(*string)) )
		{
			/* Indicate invalid char encountered */
			return -1;
		}

		putchar( firstChar(*string) );

		/* Check if second char in word is null char */
		if( secondChar(*string) )
		{
			break;
		}
		else if( !isValidChar( secondChar(*string) ) )
		{
			/* Indicate invalid char encountered */
			return -1;

		}

		putchar( secondChar(*string) );

		/* Increment pointer to next word in memory */
		string++;
	}

	/* Indicate string was successfully printed */
	return 0;

} /* end of function printString */

