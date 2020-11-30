#pragma once

//Concatenates two character arrays together
//Puts the concatenated array in dest
//Concatenates second onto first
void concatChars(char* dest, char* first, char* second);

//Does same as above but also adds tabs to the message
//This is so a message from the clients partner appears to
//be right justified similar to a text messaging app
//Not utilized in current build*
void concatChars(char* dest, char* first, char* second, bool from_partner);
