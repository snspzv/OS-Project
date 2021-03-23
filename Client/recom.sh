#!/bin/bash

g++ -std=c++14 *.cpp  -o client 

rm -r ../Client1/
cp -r ../Client ../Client1
