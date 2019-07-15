# g++ with support C++11 (tested on g++6.3)
CC = g++
CFLAGS = -c -std=c++11
LIBS = -pthread -latomic

all: server

server: main.o server.o
	$(CC) main.o server.o $(LIBS) -o server

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

server.o: server.cpp
	$(CC) $(CFLAGS) server.cpp

clean:
	rm -rf *.o server

