# OS-Project

Server compilation commands:
	g++ -std=c++14 -pthread -I../Shared *.cpp -o server -L../Shared -lshare

Client compilation commands:
	g++ -std=c++14 -I../Shared  *.cpp  -o client -L../Shared -lshare

Shared compilation commands:
	g++ -c *.cpp
	ar rs libshare.o *.o
