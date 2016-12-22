#include <stdio.h>

int main(int argc, char *argv[])
{
	char *fileName = NULL;
	char lineReadFromFile[61];
	char lineToWriteToFile[61];
	char *token = line;
	int maxLineLength = 0;
	int currentLineLength = 0;

	/* Read line */
	while( fgets(line, 61, stdout) )
	{
		/* for each word */
		while( token = strtok(token, " \n") )
		{
			/* if word fits on line */
			if (currentLineLength + strlen(token) <= maxLineLength )
			{
				/* if first word of current line */
				if ( currentLineLength == 0)
				{
					/* add word to current line */
					strcat(writeLine, token);

					/* update length of current line */
					currentLineLength += strlen(token);
				}
				else
				{
					/* overwrite null char with space */
					writeLine[currentLineLength - 1] = ' ';

					/* delimit writeLine with null char */
					writeLine[currentLineLength] = '\0';

					strcat(writeLine, token);

					currentLineLength += strlen(token) + 1;
				}
			}
			else
			{
				strcat(writeLine, "\n");
				/* write to file */
				writeLine[0] = '\0';
				strcat(writeLine, token);
			}

			token = NULL;
		}
	}

}
