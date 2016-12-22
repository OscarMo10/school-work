#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void readArray( char[][51] );
char **readWords( int* );
char *copyString( const char* );
char *removeWhitespace( char* );
void searchForWords( char[][51], char**, int, int[][51]);
int findWord( char[][51], char*, int, int, int, int[][51] );

int main( void ) 
{
	char charArray[50][51];			/* The array of letters read in */
	int inclusionArray[50][51] = {{0}};	/* Used to keep track of which chars in charArray are part of a word */
	char **words;
	int numOfWords = 0;
	int numOfWordsRead = 0;
	int widthOfCharArray = 0;
	int row;
	int column;
	int wordIndex;

	/* Construct matrix */
	readArray(charArray);

	/* Read in words from stdin */
	words = readWords( &numOfWordsRead );

	/* Search array for words */
	searchForWords(charArray, words, numOfWordsRead, inclusionArray);

	/* Find width of charArray */
	widthOfCharArray = strlen(charArray[0]);


	/* Print solution */
	for(row = 0; row < widthOfCharArray; ++row)
	{
		for(column = 0; column < widthOfCharArray; ++column)
		{
			if( inclusionArray[row][column] )
			{
				printf( "%c ", charArray[row][column] );
			}
			else
			{
				printf( "  " );
			}
		}

		printf( "\n" );
	}

	/* Free words memory */
	for(wordIndex = 0; wordIndex < numOfWords; ++wordIndex)
	{
		/* free each string */
		free(words[wordIndex]);
	}	

	/* free array of strings */
	free(words);

	return 0;

} /* end of function main */


/* Searches charArray for words. */
/* Sets position in inclusionArray to 1 if corresponding position in charArray contains letter of a word */
/* All elements of inclusionArray should be initialized to 0 */
/* return void */
void searchForWords( char charArray[][51], char **words, int numOfWords, int inclusionArray[][51])
{
	int row;
	int column;
	int direction;
	int wordIndex;
	int wordFound = 0;
	char *currentWord;
	size_t widthOfCharArray = 0;

	widthOfCharArray = strlen(charArray[0]);


	/* check entire array and every direction for words */
	for (row = 0; row < widthOfCharArray; ++row)
	{
		for(column = 0; column < widthOfCharArray; ++column)
		{
			for(direction = 0; direction < 8; ++direction)
			{
				for(wordIndex = 0; wordIndex < numOfWords; ++wordIndex)
				{
					currentWord = words[wordIndex];

					/* If current word is empty string; skip */
					if (currentWord[0] == '\0')
					{
						continue;
					}

					wordFound = findWord( charArray, currentWord, row, column, direction, inclusionArray );

					/* if word was found */
					if( wordFound )
					{
						/* set current word to empty string */
						currentWord[0] = '\0';
					}
				}

			}
		}
	}

	return;

} /* end of function searchForWords */


/* Recursive function that checks if word can be found at current position in a specific direction */
/* 0 <= dir <= 8 */
/* returns 1 if word is found, 0 if not. */
int findWord( char charArray[][51] , char *word, int row, int column, int dir, int inclusionArray[][51] )
{
	int matches = 0;
	int currentRow = row;
	int currentColumn = column;
	int charArrayWidth = strlen(charArray[0]);

	/* If row is outside valid range */
	if( row < 0 || row > (charArrayWidth - 1) )
	{
		/* return 0 to indicate no match */
		return 0;
	}

	/* If column is outside valid range */
	if( column < 0 || column > (charArrayWidth - 1) )
	{
		/* return 0 to indicate no match */
		return 0;
	}

	matches = word[0] == charArray[currentRow][currentColumn];

	/* check if were checking last char of word and that character matches */
	if( strlen(word) == 1 && matches )
	{
		inclusionArray[currentRow][currentColumn] = 1;
		return 1;
	}

	if( !matches ) 
	{
		return 0;

	}

	switch(dir)
	{
		/* North */
		case 0:
			--row;
			break;

		/* NE */
		case 1:
			--row;
			++column;
			break;

		/* East */
		case 2:
			++column;
			break;

		/* SE */
		case 3:
			++row;
			++column;
			break;

		/* South */
		case 4:
			++row;
			break;
		
		/* SW */
		case 5:
			++row;
			--column;
			break;

		/* West */
		case 6:
			--column;
			break;

		/* NW */
		case 7:
			--row;
			--column;
			break;

		default:
			printf("Direction provided to function searchForWords is not valid. Must be between 0-7. Was %d", dir);
			return -1;
	}
	
	/* increment pointer to char in string */
	++word;

	/* check if rest of word matches chars in charArray */
	matches = findWord(charArray, word, row, column, dir, inclusionArray);

	/* if findWord matched rest of string */
	if( matches )
	{
		/* Indicate current position is to be printed out */
		inclusionArray[currentRow][currentColumn] = 1;
	}

	return matches;

} /* end of function findWord */


/* Constructs square array by reading char matrix from stdin  */
/* Returns void */
void readArray(char charArray[][51])
{
	char lineBuffer[102] = { '\0' }; /* 50 chars, 50 spaces, newline, and null char */
	int width = 0;
	int numOfLinesLeft = -1;
	int index = 0;

	for(;; ++index)
	{
		/* Read first line from stdin */
		fgets( lineBuffer, 102, stdin );

		/* Remove spaces and newline */
		removeWhitespace( lineBuffer ); 

		strcpy( charArray[index], lineBuffer );

		/* If first iteration of loop */
		if( numOfLinesLeft == -1)
		{
			width = strlen( lineBuffer );
			numOfLinesLeft = width;
		}
		
		--numOfLinesLeft;

		/* Break when no more lines left to read for char array */
		if( numOfLinesLeft == 0 )
		{
			break;
		}
	}

	return;
}


/* Removes space and newline character from source. Result placed in dest */
/* Function changes source */
/* Function returns source on success, NULL on failure */
char *removeWhitespace(char *source)
{
	char delimiterString[] = " \n";
	char *copyOfSource;
	char *copyPtr;
	char *token;

	/* Allocate memory for copy of source */
	copyOfSource = malloc( strlen(source) + 1 );

	copyPtr = copyOfSource;

	/* if copyOfSource is NULL */
	if ( !copyOfSource )
	{
		return NULL;
	}

	/* Copy source */
	strcpy( copyOfSource, source );

	/* Make source equal to empty string */
	source[0] = '\0';

	/* Tokenize line */
	while( (token = strtok( copyPtr, delimiterString )) )
	{
		strcat(source, token);
		copyPtr = NULL;
	}

	free( copyOfSource );

	return source;
}


/* Read words into *wordBuffer from stdin */
/* Pointer to buffer should be NULL. Function will allocate memory. */
/* Returns number of words read */ 
char **readWords( int *wordsRead )
{
	char **words = NULL;
	int wordsSize = 10;
	char currentWord[52];
	int numOfWordsAvailable = wordsSize;
	int index;

	*wordsRead = 0;

	words = malloc( sizeof(char*) * wordsSize );

	/* If equal to NULL */
	if ( !words )
	{
		/* Return -1 to indicate error */
		return NULL;
	}

	for( index = 0; 1; ++index )
	{
		/* If no space in wordsBuffer */
		if( numOfWordsAvailable == 0 )
		{
			wordsSize *= 2;
			words = realloc( words, sizeof(char*) * wordsSize );
			numOfWordsAvailable = wordsSize - *wordsRead;

			/* If equal to NULL */
			if( !words )
			{
				return NULL;
			}

		}

		/* set currentWord to empty string */
		currentWord[0] = '\0';
		
		fgets(currentWord, 52, stdin);

		/* Check if EOF has been reached */
		if( strlen( currentWord ) == 0)
		{
			break;
		}
		else
		{
			/* Get rid of newline */
			currentWord[ strlen(currentWord) - 1 ] = '\0';

		}


		words[index] = copyString(currentWord);
		--numOfWordsAvailable;
		++(*wordsRead);
	}

	return words;

} /* end of function readWords */


/* Makes copy of str1. */
/* Return char pointer to newly allocated copy string */
char *copyString(const char *str1)
{
	char *returnString = malloc( strlen(str1) + 1 );

	/* If returnString is NULL */
	if( !returnString )
	{
		return NULL;
	}

	return strcpy( returnString, str1 );

} /* end of function copyString */



