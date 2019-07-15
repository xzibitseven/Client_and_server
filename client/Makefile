# g++ with support C++11 (tested on g++6.3)
CC = g++
CFLAGS = -c -std=c++11

all: client

client: main.o client.o
	$(CC) main.o client.o $(LIBS) -o client

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

client.o: client.cpp
	$(CC) $(CFLAGS) client.cpp

clean:
	rm -rf *.o client

