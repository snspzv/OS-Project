#pragma once
#include "Constants.h"

//A struct representing the information shared between the server and each client
struct sharedInfo {
	int connection_requested; //Keeps track of the status of a client requesting to connect to another client
	char name[BUFFER_SIZE]; //The clients name
	int connection_index; //The index in the client vector where their partner is located
	int sock_fd; //The clients socket file descriptor
	bool busy; //tracks if the client is busy or not
	bool connected; //tracks if the client is connected to a user or not
};

//A function which searches for another client by there name
//Returns an index to the other client if they are found
int name_in_set(char * check_name, int socket);

//Adds the information of a user to the information vector that is shared by the server and each client
void add_to_set(char* name_in, int vect_index, int socket);

//Sends a request to a potential partner to become messaging partners
//Returns true if the two become connected false if otherwise
bool request_partner(int requester_index, int partner_index);

//Checks the status of requesting a partner
//Returns the value of the requesters connection requested integer information
int check_request_status(int requester_index);

//manages a failed request by resetting the states of the two clients
void failed_request_reset(int requester_index, int partner_index);

//sets the clients information at the given index to not busy
void set_not_busy(int vect_index);

//checks if the client at the given index is busy or not
//Returns true if busy false if not busy
bool is_not_busy(int vect_index);

//Gets the requester's name at the given client's index
//returns the value of the clients connection requested variable
int get_requester_name(int vect_index, char* requester_name);

//A function to accept a request to partner with requesting client
//sets the value of connection requested to the index of the requesting client
void accept_request(int vect_index, int requester_index);

//A funtion which denies the request to partner with requesting client
//sets the value of connection requested to -1
void deny_request(int requester_index);

//A function which ends the request to partner with requesting client due to a timeout
//sets the value of connection requested to -2
void timeout_request(int requester_index);

//Partners two clients together
//sets there connection indexes to eachothers indexes
void set_connection(int vect_index, int partner_index);

//Returns the value of the partners socket file descriptor
int get_partner_sock(int vect_index);

//Checks if the connection between two partners is active
//Returns false if the connection index at the at the vector index is -1
//Returns true otherwise 
bool connection_active(int vect_index);
