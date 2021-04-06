#!/bin/bash
#Script to compile client and create identical client folder(s) for testing
rm *.log  2> /dev/null
g++ -std=c++17 *.cpp  -o client 

CLI_NUM=1
if [ $# -eq 1 ]
then 
	CLI_NUM="$1"
fi

for ((cli=1; cli<=$CLI_NUM; cli++))
do
	CLI_DIR="Client$cli"
	if [ -d "../$CLI_DIR" ]
	then
		rm -r "../$CLI_DIR"
	fi

	cp -r . "../$CLI_DIR"
done
exit 0
	
