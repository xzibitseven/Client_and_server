# Client_and_server

# Server

Multithreaded server (non-asynchronous) with multiplexed input/output for sockets (non-blocking sockets). In the implementation used a simple pool of threads with lock-free queue for storing a queue of tasks. Losk-free data structures increase the performance of multithreaded programs by reducing thread downtime.  In server design the design pattern for multithreaded one-producer/multi-consumers programs is applied. 

Characteristics:

     - the C++11 standard was used;
     - written under Linux x64.

# Client

Characteristics:

     - the C++11 standard was used;
     - written under Linux x64.
     
A cross-platform approach was used.
Tested under Debian Stretch (9.8) x64.
The project has a Makefile to make it easier to build a Linux OS project.
