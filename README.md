# Client_and_server

# Server

Multithreaded server (non-asynchronous) with multiplexed input/output for sockets (non-blocking sockets). In the implementation used a simple pool of threads with lock-free queue for storing a queue of tasks. Losk-free data structures increase the performance of multithreaded programs by reducing thread downtime.  In server design the design pattern for multithreaded one-producer/multi-consumers programs is applied. 

Characteristics:
     - the C++11 standard was used;
     - written under Linux x64.
A cross-platform approach was used.
Tested under Debian Stretch (9.8) x64.
The project has a Makefile to make it easier to build a Linux OS project.

The server processes each connection with the client in a separate thread, checks the data for validity and writes the received correct data to the file. 
The server accepts data in the following format: "[2018-09-19 13:50:01.000] Name1."
The port number on which the server will listen to client connections is transmitted to the program input.


# Client

