#pragma once

#define PORT 8080
#define BACKLOG 10
#define MAX_THREADS 15
#define BUFFER_SIZE 1024
#define SELECT_TIMEOUT_S 5
#define SELECT_TIMEOUT_NS 0
#define NUM_TABS 7
#define CHAR_LIMIT 49

//Server Messages
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
#define RX_NEXT 0
#define TX_NEXT 1
#define EITHER_NEXT 2