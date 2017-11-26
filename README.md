#     T E A M 1
#   H A N G M A N
# . . . _ _ _ _ _ _ _ _ _ 
# . . . |/ . . . . . . |
# . . . |. . . _ _ _(")_ _ _
# . . . |.  . . . . . |_|
# . . . |. . . . . . /^\
# . . . |. . . . _// . \ \ _
# _ _ |_ _ _ _ _ _ _ _
# 
# Year 4: Networked Games CA
# Hangman

## 26/11/2017

This is a group assignment for the 4th year Networked Games module. The group consists of:
  * [Joe O'Regan](https://github.com/joeaoregan)
  * [Samantha Marah](https://github.com/jasfoley)
  * [Jason Foley](https://github.com/samanthamarah)

## Features:

This online text based version of Hangman includes the following features:
  * TCP server socket using fork()
  * TCP server socket using select(), handles more than one client
  * UDP client and server playing for one game
  * Modular design with Hangman.h and DrawHangman.h files containing functions common to the different socket versions
  * A client that handles asynchronous data inputs from the network / server, and from the keyboard / user correctly
  * All server and clients handling a dual TCP/IP stack (IPv4 and IPv6). With the client handling naming and the DNS.
    (The server can handle connections from both IPv4 and IPv6 clients, but the clients need to be separate programs)

## Folders:

Each folder contains a separate set of sockets, or individual Client and Server that is interchangeable with others.
Matching pairs of sockets have Hangman in the folder title, specific Client/Server sockets are specified in the title.
######  1. Original Hangman Listings
######  2. Hangman Modified TCP
######  3. Client Send Recv TCP
######  4. Fork Server TCP
######  5. Hangman Select TCP
######  6. Hangman UDP
######  7. Asynchronous Client TCP
######  8. Hangman Dual Stack TCP
######  9. Asynchronous Client UDP
######  10. Fork Server UDP
    
    
##### 1-Original-Hangman-Listings

The Original Hangman code, unmodified with errors. Using `read()` and `write()` to communicate

##### 2-Hangman-Modified-TCP

Modified Hangman game, with graphics, coloured text, and functionality to set up sockets, and play the game separated to header files

##### 3-Client-SendRecv-TCP

The Modified Client using `send()` and `recv()` instead of read() and write() to communicate with the server

##### 4-Fork-Server-TCP

A concurrent version of the TCP Server using fork(), to fork a separate process to deal with each client connection

##### 5-Hangman-Select-TCP

A concurrent version of the TCP Server using select(), to handle input from different client connections by one Server process

##### 6-Hangman-UDP

The Modified Hangman Client and Server using `sendto()` and `recvfrom()` to communicate between each other in a connectionless state

##### 7-Asynchronous-Client-TCP

The Modified TCP Client that can handle input asynchronously from the socket and file descriptors using the ```select()``` function

##### 8-Hangman-Dual-Stack-TCP

The Modified Hangman TCP Client and Server that can handle connections to and from IPv4 and IPv6 addresses

##### 9-Asynchronous-Client-UDP

The Modified UDP Client that can handle input asynchronously from socket and file descriptors using `select()`

##### 10-Fork-Server-UDP (Unfinished)

A concurrent version of the UDP Server using `fork()`, to fork a separate process on receiving data with recvfrom to deal with each client connection

## Running:

For each listing or set of listings, see running and compile instructions in each folder
