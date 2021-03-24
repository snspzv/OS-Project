#pragma once
#include<vector>
#include "User.h"

//Handles the connection between the user and the server and two users
void manageConnection(void * arg);

//Initializes a new user
//Uses a lock guard to make sure no two users are created at the same time
//Increments the number of users
void initUserIndex(User & user);
