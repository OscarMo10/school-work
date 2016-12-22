#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int genQuestion( int, int);
int answerQuestion( int );
void response( int, int );

int main(void)
{
	int numOfQuestions;
	int difficulty;
	int args_matched;
	int count;
	int answerToQuestion;
	int wasAnswerCorrect;
	int numOfQuestionsCorrect = 0;
	char* userInput = NULL;	/* Buffer used for user input will be allocated by getline function */
	size_t numOfBytes = 0; 	/* Initial size of buffer used for user input */

	/* seed random number generator */
	srand(time(NULL));

	/* Ask user for num of questions, between 1 and 20. Verify and re-prompt if necessary. */
	do {
		puts( "How many questions for this test(1 - 20)? " );
		getline( &userInput, &numOfBytes, stdin );
		args_matched = sscanf( userInput, "%d", &numOfQuestions );
	} while( args_matched != 1 ||  !(numOfQuestions <= 20 && numOfQuestions >= 1) ); 

	/* Ask user for a difficulty level, between 1 and 4. Verify and re-prompt if necessary. */
	do {
		puts( "Select difficulty (1 - 4): " );
		getline( &userInput, &numOfBytes, stdin );
		args_matched = sscanf( userInput, "%d", &difficulty );
	} while( args_matched != 1 || !( difficulty <= 4 && difficulty >= 1 ) );

	/* deallocate memory allocated by getline() */
	free(userInput);

	/* For each question */
	for(count = 1; count <= numOfQuestions; count++) 
	{
		/* Randomly generate and output question based on difficulty chosen. */
		answerToQuestion = genQuestion( count, difficulty );

		/* Prompt user to answer question */
		wasAnswerCorrect = answerQuestion( answerToQuestion );

		/* if user answered correctly, increment num of questions answered correctly */
		if ( wasAnswerCorrect )
		{
			numOfQuestionsCorrect++;
		}

		/* Give user feedback based on if the user correctly or incorrectly answered question */
		response( wasAnswerCorrect, answerToQuestion );
	}

	/* Provide score by stating the number of questions correct/number of questions asked. */
	printf( "Your score was %d/%d\n", numOfQuestionsCorrect, numOfQuestions );

} /* End function main */


/* Randomly generate and output question based on difficulty chosen. */
/* Returns answer to question as int */
int genQuestion(int questionNum, int difficulty)
{
	enum operation {
		ADDITION,
		SUBTRACTION,
		MULTIPLICATION,
		DIVISION
	};

	int operand1;
	int operand2;
	char operationChar;
	enum operation currentOperation;

	/* randomly decide operation for question */
	int op = rand() % 4;

	switch( op )
	{
		case ADDITION:
			operationChar = '+';
			currentOperation = ADDITION;
			break;
		case SUBTRACTION:
			operationChar = '-';
			currentOperation = SUBTRACTION;
			break;
		case MULTIPLICATION:
			operationChar = '*';
			currentOperation = MULTIPLICATION;
			break;
		case DIVISION:
			operationChar = '/';
			currentOperation = DIVISION;
			break;

	}

	/* set operands for question */
	switch( difficulty ) {
		case 1:
			operand1 = rand() % 10 + 1;

			do {
				operand2 = rand() % 10 + 1;
			} while ( currentOperation == DIVISION && operand2 == 0 );

			break;
		case 2: 
			operand1 = rand() % 50 + 1;

			do {
				operand2 = rand() % 50 + 1;
			} while ( currentOperation == DIVISION && operand2 == 0 );

			break;
		case 3: 
			operand1 = rand() % 100 + 1;
			do {
				operand2 = rand() % 100 + 1;
			} while ( currentOperation == DIVISION && operand2 == 0 );


			break;
		case 4:
			operand1 = rand() % 201 - 100;

			do {
				operand2 = rand() % 201 - 100;
			} while ( currentOperation == DIVISION && operand2 == 0 );

			break;
	}

	/* print question */
	printf("Question %d: %d %c %d\n", questionNum, operand1, operationChar, operand2);
	
	/* return answer to question */
	switch ( currentOperation )
	{
		case ADDITION:
			return operand1 + operand2;
		case SUBTRACTION:
			return operand1 - operand2;
		case MULTIPLICATION:
			return operand1 * operand2;
		case DIVISION:
			return operand1 / operand2;
	}
	
} /* End function genQuestion */



/* Prompts user for answer. Gets answer from user */
/* Returns 1 or 0 depending on if user answered question correctly */
int answerQuestion(int correctAnswer)
{
	int argsMatched;
	int userAnswer;
	char* userInput = NULL; /* Buffer for user input will be allocated by getline function */
	size_t size = 0; /* Initial size of buffer for user input */


	/* Prompt user for answer */
	printf("Enter answer: ");
	getline(&userInput, &size, stdin);
	argsMatched = sscanf( userInput, "%d", &userAnswer );

	/* Free memory allocated by getline function */
	free(userInput);

	/* Return whether user answered question correctly */
	if( argsMatched != 1 || correctAnswer != userAnswer )
	{
		return 0;
	}
	else {
		return 1;
	}

} /* End function answerQuestion */


/* Prints response to user. Prints correct answer if user answered question wrong */
/* Return void */
void response(int isAnswerCorrect, int correctAnswer)
{
	int responseIndex;
	char* currentResponse;
	char* correctResponses[] = { "Nice!", "Jaw Dropping Skills!", "Amazing!" };
	char* incorrectResponses[] = { "That's Embarassing. Wrong!", "How'd you get that wrong?", "Not even close!" };

	/* Get index of response randomly */
	responseIndex = rand() % 3;

	/* Output response to user */
	if( isAnswerCorrect )
	{
		currentResponse = correctResponses[responseIndex];
		puts(currentResponse);
	}
	else
	{
		currentResponse = incorrectResponses[responseIndex];
		puts( currentResponse );
		printf( "The correct answer was %d\n", correctAnswer );
	}

	return;

} /* End function response */
