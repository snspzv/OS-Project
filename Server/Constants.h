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
#define USER_MESSAGE 1
#define UNAME_REQUEST 2
#define UNAME_TAKEN 4
#define UNAME_VALID 8
#define GET_PARTNER 16
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

//Send paccket
#define TO_SELF true
#define TO_PARTNER false