#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <ctype.h>

#define SERVER_COMMAND_EXECUTION_RESPONSE_LENGTH 30

static const char * GetCommand = "get";
static const char * PutCommand = "put";
static const char * DeleteCommand = "delete";
static const char * StopCommand = "stop";
static const char * StudentIdTitle = "SID";
static const char * LastNameTitle = "Lname";
static const char * FirstNameTitle = "Fname";
static const char * MiddleInitialTitle = "M";
static const char * GPA_Title = "GPA";
static const char * RecordHeaderPrintFormat = "| %-5s | %-9s | %-9s | %s | %-4s |\n";
static const char * RecordBorderPrintFormat = "+-------+-----------+-----------+---+------+";
static const char * RecordDataPrintFormat = "| %05lu | %-9s | %-9s | %c | %1.2f |\n";

typedef struct student {
    char lname[10], initial, fname[10];
    unsigned long SID;
    float GPA;
} SREC;

enum COMMAND {
    Invalid, Get, Put, Delete, Stop
};

typedef enum COMMAND Command;

Command getCommand(const char *command);
void error(char *msg);
void startDatabaseSession(int sockfd);
void removeTrailingWhitespace(char *buffer);
void handleGetCommandResponse(int sockfd);
void printStudentRecords(SREC *studentRecords, int numberOfRecords);
void handleResponse(int sockfd, Command command);
void handleDeleteCommandResponse(int sockfd);
void handleStopCommandResponse(int sockfd);
void handlePutCommandResponse(int sockfd);
void printServerResponse(int sockfd);




int main(int argc, char *argv[])
{
    int sockfd, portno;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    
    if (argc < 3){
        fprintf(stderr,"usage %s hostname port\n", argv[0]);
        exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket (AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){ error("ERROR opening socket");}
    server = gethostbyname(argv[1]);
    if (server == NULL){
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy((char *)server->h_addr_list, (char*)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    
    if (connect(sockfd,(struct sockaddr*) &serv_addr,sizeof(serv_addr)) < 0){
        error("ERROR connecting");
    }
    
    startDatabaseSession(sockfd);
    
    close(sockfd);
    
    return 0;
} /* end of main */

/* Start issuing commands to database */
/* input "stop" to end database session */
/* returns void */
void startDatabaseSession(int sockfd)
{
    char commandBuffer[100];
    Command command;
    ssize_t n;

    do {
        printf("Enter next command> ");
        memset(commandBuffer, 0, 100);
        fgets(commandBuffer, 256, stdin);
        removeTrailingWhitespace(commandBuffer);
        command = getCommand(commandBuffer);
        
        if (command == Invalid) {
            printf("Command entered not valid.\n");
            /* printUsage(); */
            continue;
        }
        
        n = write(sockfd, commandBuffer, strlen(commandBuffer));
        if (n < 0){
            error("ERROR writing to socket");
        }
        
        handleResponse(sockfd, command);
    
    } while ( command != Stop);
    
    return;
} /* end of startDatabaseSession */


/* handles response from server after issuing command */
/* returns void */
void handleResponse(int sockfd, Command command)
{
    switch (command) {
        case Invalid:
            puts("Invalid command should not have been sent to client");
            exit(1);
            
        case Get:
            handleGetCommandResponse(sockfd);
            return;
            
        case Put:
        case Delete:
        case Stop:
            printServerResponse(sockfd);
            return;
            
        default:
            puts("Invalid command was passed to handleResponse function.");
            exit(1);
    }
} /* end of handleResponse */

/* Used to handle reponse from server after issuing 
 * a put, delete, or stop command
 * return void
 */
void printServerResponse(int sockfd)
{
    char buffer[30];
    memset(buffer, '\0', 30);
    
    read(sockfd, buffer, 30);
    puts(buffer);
    
    return;
} /* end of printServerResponseFunction */

/* Handles response from server after client issued a get command */
/* returns void */
void handleGetCommandResponse(int sockfd)
{
    ssize_t n;
    int numberOfRecords = -1;
    SREC *studentRecords = NULL;
    int index;
    
    /* Read number of records to be read */
    n = read(sockfd, &numberOfRecords, sizeof(int));
    if (n < 0){
        error("ERROR writing to socket");
    }
    
    /* If number of records read is 0 */
    if (!numberOfRecords) {
        printStudentRecords(NULL, numberOfRecords);
        return;
    }
    
    /* Allocate array of student records */
    studentRecords = malloc(sizeof(SREC) * numberOfRecords);
    
    /* Read in record data into struct */
    for (index = 0; index < numberOfRecords; index++)
    {
        n = read(sockfd, &studentRecords[index], sizeof(SREC));
        if (n < 0) {
            error("ERROR reading from socket");
        }
    }
    
    printStudentRecords(studentRecords, numberOfRecords);
} /* end of handleGetCommandResponse */


/* prints student records in appropriate format to stdout */
/* returns void */
void printStudentRecords(SREC *studentRecords, int numberOfRecords)
{
    int index;
    
    printf(RecordHeaderPrintFormat, StudentIdTitle, LastNameTitle, FirstNameTitle, MiddleInitialTitle, GPA_Title);
    puts(RecordBorderPrintFormat);
    
    for (index = 0; index < numberOfRecords; index++) {
        printf(RecordDataPrintFormat, studentRecords[index].SID, studentRecords[index].lname, studentRecords[index].fname, studentRecords[index].initial, studentRecords[index].GPA);
    }
    
    puts(RecordBorderPrintFormat);
    
} /* end of printStudentRecords */


/* return what kind of command the string command issues */
/* returns Command */
Command getCommand(const char *fullCommand)
{
    char *token = NULL;
    char *copy = NULL;
    
    copy = malloc(strlen(fullCommand) + 1);
    strcpy(copy, fullCommand);
    
    /* get command from fullCommand */
    token = strtok(copy, " ");
    
    /* return type of command issued */
    if (!strcmp(token, GetCommand))
    {
        /* Check if arg passed to get is valid */
        token = strtok(NULL, " ");
        
        /* if token NULL */
        if ( !token ) {
            return Invalid;
        }
        
        if (!strcmp(token, "lname"));
        else if (!strcmp(token, "fname"));
        else if (!strcmp(token, "SID"));
        else if (!strcmp(token, "GPA"));
        else
        {
            return Invalid;
        }
        
        free(copy);
        copy = NULL;
        
        return Get;
    }
    else if (!strcmp(token, PutCommand))
    {
        return Put;
    }
    else if (!strcmp(token, DeleteCommand))
    {
        return Delete;
    }
    else if (!strcmp(token, StopCommand))
    {
        return Stop;
    }
    else {
        return Invalid;
    }
} /* end of getCommand */


/* Note: Not sure if we need this yet. */
void removeTrailingWhitespace(char *buffer)
{
    ssize_t length = strlen(buffer);
    while (length - 1 >= 0 && isspace(buffer[length -1])) {
        buffer[length - 1] = '\0';
        length--;
    }
    
    return;
}


void error(char *msg) {
    perror(msg);
    exit(0);
}
