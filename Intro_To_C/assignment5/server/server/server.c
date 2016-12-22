#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <time.h>

#define FILE_NAME "data"
#define DEBUG 1

/* --- Structures --- */
typedef struct student {
    char lname[10], initial, fname[10];
    unsigned long SID;
    float GPA;
} SREC;

typedef struct node {
    struct node * next;
    SREC * data; /*SREC*/
} Node;

/* --- Function Prototypes --- */
/* IO */
int putRecord(Node * f, Node * l, Node * s, Node * g, const char * cmd, const char * s_gpa, const char * s_sid, const char * first, const char * init, const char * last);
int saveNodes(FILE * fp, Node * start);
void sendRecords(Node * start, int numberOfRecords, int handle);
void readRecords(FILE * fp, Node * first, Node * last, Node * SID, Node * GPA, int * numRec);
void error(char *msg);

/* Nodes */
Node * createNode();
Node * deleteNode(Node *, unsigned long, int *);
Node * insertAtFront(Node * front, SREC * record);
void printNodes(Node * start);
void add(Node * node, SREC * rec);
void swapNodes(Node * alpha, Node * beta);

/* Sorting */
Node * bubbleSortByFirst(Node * top);
Node * bubbleSortByLast(Node * top);
Node * bubbleSortBySID(Node * top);
Node * bubbleSortByGPA(Node * top);
void sortAll(Node * fName, Node * lName, Node * SID, Node * GPA);

/* Utility */
unsigned long getSID(const char * s_SID);
int getTypeID(char * type);
int getCommand(char * cmd);
SREC *newSREC(const char * last, char init, const char * first, unsigned long SID, float GPA);

/* --- Global Static Variables --- */
/* I had to modify some of these from the ones given since some were over the 30 char limit */
static const char *successfulPutResponse = "Record successfully added.";
static const char *unsuccessfulPutResponse = "Error while adding record.";
static const char *unsuccessfulStopResponse = "Error while saving database.";
static const char *successfulStopResponse = "Database saved successfully.";
static const char *successfulDeleteResponse = "Record deleted.";
static const char *unsuccessfulDeleteResponse = "Unable to delete record.";

int main(int argc, char *argv[]){
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    
    int numRecords = 0;
    int commandID = -1;
    unsigned long tempSID;
    
    /*Linked list node heads for the 4 different sorting types */
    Node * fNameHead = NULL;
    Node * lNameHead = NULL;
    Node * SIDHead = NULL;
    Node * GPAHead = NULL;
    
    FILE * dataFile = NULL;
    
    /* Make sure usage was correct, a port number was provided. */
    if (argc < 2){
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    
    /* Create a socket and make sure socket was successfully opened */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){
        error("ERROR opening socket");
    }
    
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
        error("ERROR on binding");
    }
    listen(sockfd,5);
    
    printf("Waiting for client... \n");
    
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr * ) & cli_addr, & clilen);
    
    if (newsockfd < 0){
        error("ERROR on accept");
    }
    
    /* Init Nodes */
    fNameHead = createNode();
    lNameHead = createNode();
    SIDHead = createNode();
    GPAHead = createNode();
    
    /* Load data from file */
    dataFile = fopen(FILE_NAME, "r");
    if(dataFile == NULL){
        /*Create it. */
        dataFile = fopen(FILE_NAME, "w+");
        if(dataFile == NULL){
            printf("Unable to create file! \n");
            exit(0);
        } else {
            fclose(dataFile);
        }
    } else {
        readRecords(dataFile, fNameHead, lNameHead, SIDHead, GPAHead, &numRecords);
        sortAll(fNameHead, lNameHead, SIDHead, GPAHead);
        fclose(dataFile);
    }
    /* Main Loop */
    
    printf("Client connected, reading... \n");
    while(1){
        memset(buffer, 0, 256); /*Otherwise we end up with wonkey stuff... */
        n = read(newsockfd,buffer,255);
        if(n<=0){ /*Negative */
            error("ERROR reading from socket");
        }
        
        commandID = getCommand(strtok(buffer, " ")); /*Assuming it's sent as a space! */
        
        if(commandID == 0){ /*Get command */
            n = getTypeID(strtok(NULL, " " )); /*Reuse n */
            if(n == 0){
                sendRecords(fNameHead, numRecords, newsockfd);
            } else if(n ==1){
                sendRecords(lNameHead, numRecords, newsockfd);
            } else if(n == 2){
                sendRecords(SIDHead, numRecords, newsockfd);
            } else if(n == 3){
                sendRecords(GPAHead, numRecords, newsockfd);
            } else {
                sendRecords(NULL, 0, newsockfd); /*No records in that cat...*/
            }
        }
        
        if(commandID == 1){ /*put*/
            if(putRecord(fNameHead, lNameHead, SIDHead, GPAHead,
                         strtok(buffer, ","), strtok(NULL, ","), strtok(NULL, ","),
                         strtok(NULL, ","),strtok(NULL, ","),strtok(NULL, ",")) == 1){
                /*Success!*/
                numRecords++;
                sortAll(fNameHead, lNameHead, SIDHead, GPAHead);
                write(newsockfd, successfulPutResponse, strlen(successfulPutResponse));
            } else {
                write(newsockfd, unsuccessfulPutResponse, strlen(unsuccessfulPutResponse));
            }
        }
        
        if(commandID == 2){ /*delete*/
            tempSID = getSID(strtok(NULL, " ")); /*SID, already toked it to get CommandID*/
            
            /* We're doing assignment since the head node might change...
             In the future might make another field like isDeleted
             that would save a lot of time.
             */
            fNameHead = deleteNode(fNameHead, tempSID, &n);
            lNameHead = deleteNode(lNameHead, tempSID, &n );
            SIDHead = deleteNode(SIDHead, tempSID, &n);
            GPAHead = deleteNode(GPAHead, tempSID, &n);
            
            if(n == 0){ /*Failed*/
                write(newsockfd, unsuccessfulDeleteResponse, strlen(unsuccessfulDeleteResponse));
            } else{
                numRecords--;
                sortAll(fNameHead, lNameHead, SIDHead, GPAHead);
                write(newsockfd, successfulDeleteResponse, strlen(successfulDeleteResponse));
            }
        }
        
        if(commandID == 3){ /*stop*/
            break;
        }
    }
    
    dataFile = fopen(FILE_NAME, "w");
    if(dataFile == NULL){
        printf("Unable to data file. Are you sure it exists? File Name: %s \n", FILE_NAME);
        write(newsockfd, unsuccessfulStopResponse, strlen(unsuccessfulStopResponse));
    } else {
        saveNodes(dataFile, SIDHead);
        fclose(dataFile);
        write(newsockfd, successfulStopResponse, strlen(successfulStopResponse));
    }
    
    close(newsockfd);
    return 0;
}

/* Utility function to print all info starting with a node */
void printNodes(Node * start){
    printf("LN | MI | FM | ID | GPA |\n");
    while(start != NULL && start->data != NULL){
        printf("%s | %c | %s | %lu | %f | \n", start->data->lname, start->data->initial, start->data->fname, start->data->SID, start->data->GPA);
        start = start->next;
    }
    printf("------------------------\n");
}

/* Saves all the nodes under the start node provided.
	Given a file pointer */
int saveNodes(FILE * fp, Node * start){
    while(start != NULL && start->data != NULL){
        fwrite(start->data, sizeof(SREC), 1, fp);
        start = start->next;
    }
    
    return 1;
}

/*Utility function convert from a string to an unsigned long */
unsigned long getSID(const char * s_SID){
    char * temp;  /*strtoul puts the rest of the text in temp after converting */
    return strtoul(s_SID, &temp, 10);
}

/*Sends records to the place specified by the handle. */
void sendRecords(Node * start, int numberOfRecords, int handle){
    int c = 0;
    Node * temp = start;
    
    write(handle, &numberOfRecords, sizeof(int));
    
    if(numberOfRecords == 0){ /*No records to write. */
        return;
    }
    
    while(c < numberOfRecords){
        write(handle, temp->data, sizeof(SREC) );
        temp = temp->next;
        c++;
    }
}

/* Reads records from the given file pointer, passed the 4 node types so we can modify them,
	and numRec to add to. */
void readRecords(FILE * fp, Node * first, Node * last, Node * SID, Node * GPA, int * numRec){
    
    SREC * tempSREC = malloc(sizeof(SREC));
    SREC * cyclingSREC;
    
    while(fread(tempSREC, sizeof(SREC), 1, fp) != 0){ /*Returns 0 when nothing was read */
        /*
         Data is an SREC pointer, so we need to make a new pointer since tempSREC's
         pointed to value is changed. Therefore we need to copy, like you would with strcpy
         */
        cyclingSREC = newSREC(tempSREC->lname, tempSREC->initial, tempSREC->fname, tempSREC->SID, tempSREC->GPA);
        add(first, cyclingSREC);
        add(last, cyclingSREC);
        add(SID, cyclingSREC);
        add(GPA, cyclingSREC);
        
        (*numRec)++;
        free(tempSREC);
        tempSREC = malloc( sizeof(SREC) );
    }
}

/* Creates a Node */
Node * createNode(){
    Node * t;
    t = malloc( sizeof(Node) );
    t->data = NULL;
    t->next = NULL;
    return t;
}

/*strtok calls are called right to left, so SREC is backwards. */
int putRecord(Node * f, Node * l, Node * s, Node * g, const char * cmd, const char * s_gpa, const char * s_sid, const char * init, const char * first, const char * last){
    unsigned long SID;
    float GPA;
    SREC * temp;
    
    if(cmd == NULL || s_gpa == NULL || s_sid == NULL || init == NULL || first == NULL || last == NULL){
        return 0;
    }
    
    
    SID = getSID(s_sid);
    GPA = atof(s_gpa);
    if(SID == 0 || GPA == 0){
        return 0;
    }
    
    temp = newSREC(last, init[0], first, SID, GPA);
    add(f, temp);
    add(l, temp);
    add(s, temp);
    add(g, temp);
    
    return 1;
}

/* Adds Data to a new node appended to the end of the last node */
void add(Node * node, SREC * rec){
    Node * temp;
    temp = node;
    
    /*Head node case. You need data, not another node. */
    if(temp->data == NULL){
        temp->data = rec;
        return;
    }
    
    while(temp->next != NULL){
        temp = temp->next;
    }
    
    temp->next = createNode();
    temp->next->data = rec;
}

Node * deleteNode(Node * head, unsigned long SID, int * result){
    Node * checking;
    
    checking = head;
    if(head->data->SID == SID){
        if(head->next == NULL){
            return createNode();
        }
        return head->next;
    }
    
    while(checking != NULL){
        if(checking->next != NULL){
            if(checking->next->data->SID == SID){ /*Delete it. */
                if(checking->next->next != NULL){ /*Is there one after the one we found? */
                    checking->next = checking->next->next;
                    (*result) = 1;
                    return head;
                } else { /*The last element is the one... */
                    checking->next = NULL;
                    (*result) = 1;
                    return head;
                }
            }
        }
        checking = checking->next;
    }
    
    (*result) = 0;
    return head; /*Nothing was there to delete...*/
}

void sortAll(Node * fName, Node * lName, Node * SID, Node * GPA){
    bubbleSortByFirst(fName);
    bubbleSortByLast(lName);
    bubbleSortBySID(SID);
    bubbleSortByGPA(GPA);
}

/* Inserts a new node at the FRONT. Choose to return
 rather than dealing with pointers to pointers...
 Currently not used.
 */
Node * insertAtFront(Node * front, SREC * record){
    Node * newNode = createNode();
    
    newNode->data = record;
    newNode->next = front;
    
    return newNode;
}

/*Swaps the data in the nodes, rather than the pointers. */
void swapNodes(Node * alpha, Node * beta){
    SREC * tempRecord = alpha->data;
    alpha->data = beta->data;
    beta->data = tempRecord;
}

SREC *newSREC(const char * last, char init, const char * first, unsigned long SID, float GPA){
    SREC * temp = malloc(sizeof(SREC));
    
    /* They're arrays they don't need mallocing*/
    strncpy(temp->lname, last, 9);
    
    /* They're arrays, we good.*/
    strncpy(temp->fname, first, 9);
    
    /* Addition made by OM. Initial wasn't initialized */
    temp->initial = init;
    
    temp->SID = SID;
    
    temp->GPA = GPA;
    
    return temp;
    
}

/* Utility that takes a string and identifies what command ID it is. */
int getCommand(char * cmd){
    if(strcmp(cmd, "get") == 0){
        return 0;
    }
    
    if(strcmp(cmd, "put") == 0){
        return 1;
    }
    
    if(strcmp(cmd, "delete") == 0){
        return 2;
    }
    
    if(strcmp(cmd, "stop") == 0){
        return 3;
    }
    
    return -1;
}

/*Takes a type string and converts it to an id.
	Types are the fname, lname, SID, GPA  from get commands*/
int getTypeID(char * type){
    if(type == NULL){
        return -1;
    }
    
    if(strcmp(type, "fname") == 0){
        return 0;
    }
    
    if(strcmp(type, "lname") == 0){
        return 1;
    }
    
    if(strcmp(type, "SID") == 0){
        return 2;
    }
    
    if(strcmp(type, "GPA") == 0){
        return 3;
    }
    
    return -1;
}

Node * bubbleSortByFirst(Node * top){
    int hasSwapped;
    int strcmpResult;
    
    Node * alpha;
    
    /* Nothing needs sorting. Only have one thing. */
    if(top->next == NULL){
        return top;
    }
    
    do{
        hasSwapped = 0; /*FALSE*/
        alpha = top;
        
        while(alpha->next != NULL){ /*While we have something to possibly swap... */
            strcmpResult = strcmp(alpha->data->fname, alpha->next->data->fname);
            
            if(strcmpResult > 0){
                swapNodes(alpha, alpha->next);
                hasSwapped = 1; /*Aye! We swapped. Gotta go again! */
            }
            
            alpha = alpha->next;
        }
        
    }while(hasSwapped);
    
    /* The data has changed. Not the actual pointer it self. */
    return top;
}

Node * bubbleSortByLast(Node * top){
    int hasSwapped;
    int strcmpResult;
    
    Node * alpha;
    
    /* Nothing needs sorting. Only have one thing. */
    if(top->next == NULL){
        return top;
    }
    
    do{
        hasSwapped = 0; /*FALSE*/
        alpha = top;
        
        while(alpha->next != NULL){ /*While we have something to possibly swap... */
            strcmpResult = strcmp(alpha->data->lname, alpha->next->data->lname);
            
            if(strcmpResult > 0){
                swapNodes(alpha, alpha->next);
                hasSwapped = 1; /*Aye! We swapped. Gotta go again! */
            }
            
            alpha = alpha->next;
        }
        
    }while(hasSwapped);
    
    /* The data has changed. Not the actual pointer it self. */
    return top;
}

Node * bubbleSortBySID(Node * top){
    int hasSwapped;
    
    Node * alpha;
    
    /* Nothing needs sorting. Only have one thing. */
    if(top->next == NULL){
        return top;
    }
    
    do{
        hasSwapped = 0; /*FALSE*/
        alpha = top;
        
        while(alpha->next != NULL){ /*While we have something to possibly swap... */
            
            if(alpha->data->SID > alpha->next->data->SID){
                swapNodes(alpha, alpha->next);
                hasSwapped = 1; /*Aye! We swapped. Gotta go again! */
            }
            
            alpha = alpha->next;
        }
        
    }while(hasSwapped);
    
    /* The data has changed. Not the actual pointer it self. */
    return top;
}

Node * bubbleSortByGPA(Node * top){
    int hasSwapped;
    
    Node * alpha;
    
    /* Nothing needs sorting. Only have one thing. */
    if(top->next == NULL){
        return top;
    }
    
    do{
        hasSwapped = 0; /*FALSE*/
        alpha = top;
        
        while(alpha->next != NULL){ /*While we have something to possibly swap... */
            
            if(alpha->data->GPA < alpha->next->data->GPA){
                swapNodes(alpha, alpha->next);
                hasSwapped = 1; /*Aye! We swapped. Gotta go again! */ 
            }
            
            alpha = alpha->next; 
        }
        
    }while(hasSwapped); 
    
    /* The data has changed. Not the actual pointer it self. */ 
    return top; 
}

void error(char * msg){
    printf("%s\n", msg); 
    exit(0); 
}
