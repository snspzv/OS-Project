#!/bin/bash

g++ -std=c++14 -I../Shared  *.cpp  -o client -L../Shared -lshare

rm -r ../Client1/
cp -r ../Client ../Client1
