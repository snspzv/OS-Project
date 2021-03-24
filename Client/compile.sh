#!/bin/bash
#Script to compile client and create identical client folder for testing
g++ -std=c++14 *.cpp  -o client 

if [ -d "./Client1" ]
then
	rm -r ../Client1/
fi

cp -r ../Client ../Client1

	
