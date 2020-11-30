#pragma once

//Defines many constants which are used throughout the program

#define PORT 8080 //The port used when running on a single computer
#define BACKLOG 10 //defines a backlog of 10
#define MAX_THREADS 15 //The max number of threads which will be utilized by the program
#define BUFFER_SIZE 1024 //The maximum size of a socket message
#define SELECT_TIMEOUT_S 5 //Defines a 5 second timeout
#define SELECT_TIMEOUT_NS 0 //Defines a 0 nanosecond timout
#define NUM_TABS 7 //The number of tabs used when outputing a message in the messaging box
#define CHAR_LIMIT 49 //The limit of the number of characters in a single line in the message box

//Server Messages sent to the client
//Include instructions or errors for the client
#define USER_MESSAGE 0
#define UNAME_REQUEST 1
#define UNAME_TAKEN 2
#define UNAME_VALID 3
#define GET_PARTNER 4
#define PARTNER_HAS_REQUESTED 5
#define PARTNER_DNE 6
#define CONN_MADE 7
#define CONN_DENIED 8
#define NOT_AVAIL 9
#define TIMEOUT 10
#define BAD_RESPONSE 11
#define PARTNER_FOUND 12

//Client options
//Integer options defining if the client will Recieve, Transmit, or do Either
#define RX_NEXT 0
#define TX_NEXT 1
#define EITHER_NEXT 2
