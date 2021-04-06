#pragma once

#define PORT 8080
#define BACKLOG 10
#define MAX_THREADS 15
#define BUFFER_SIZE 1024
#define SELECT_TIMEOUT_S 5
#define SELECT_TIMEOUT_NS 0
#define NUM_TABS 7
#define CHAR_LIMIT 45
#define CLR_SCR "\033[2J\033[1;1H"

//Header 0
#define USER_MESSAGE 1
#define UNAME_REQUEST 2
#define UNAME_TAKEN 4
#define GET_PARTNER 8
#define UNAME_VALID 16
#define PARTNER_HAS_REQUESTED 32
#define PARTNER_DNE 64
#define CONN_MADE 128
#define CONN_DENIED 256
#define NOT_AVAIL 512
#define TIMEOUT 1024
#define BAD_RESPONSE 2048
#define PARTNER_FOUND 4096
#define PARTNER_DISCONNECT 8192

//Client Control Messages
#define UNAME 0
#define PARTNER_REQUEST 1
#define REQUEST_RESPONSE 2

//Client options
#define RX_NEXT 0
#define TX_NEXT 1
#define EITHER_NEXT 2

//Errors
#define SOCK_CREATION -1
#define BAD_ADDRESS -2
#define FAILED_CONNECTION -3