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
# Year 4: Networked Games CA - Hangman

## 26/11/2017

This is a group assignment for the 4th year Networked Games module. The group consists of:
  * [Joe O'Regan](https://github.com/joeaoregan)
  * [Samantha Marah](https://github.com/jasfoley)
  * [Jason Foley](https://github.com/samanthamarah)

![alt text](https://raw.githubusercontent.com/joeaoregan/Yr4-NetworkGames-Hangman/master/Screenshots/4ForkServerTCP.png "Modified Client using send() and recv()")

##  04. Fork Server TCP

The Modified Client using `send()` and `recv()` instead of read() and write() to communicate with the server

##### Files:

1. serverf.c (code)
2. srv (Forking Server Application)
3. cli (2-Hangman-Modified-TCP Client Application)

##### Dependencies (Placed in root directory):
Files that the client is dependent on to compile:

1. DrawHangman.h
2. Hangman.h
3. CreateTCPSocket.h
4. Socket.h
5. TCPPlayHangman.h

## Running:

This Server application will communicate with the Client application from 2-Hangman-Modified-TCP

####### With Forking Server application running, the Client must connect to the same port

To run the existing compiled Forking Server with default port number `1066`:
```c
./srv
```
To run Server specifying the port number to listen for client connections on e.g. port `8000`:
```c
./srv 8000
```

###### With Server application running, the Client must connect to the same port
The already compiled Modified Client application included in the directory can be run with command: 
```c
./srv
```
Or see README.md in 02-Hangman-Modified-TCP for further Client running instructions

## Compiling:

To compile the Forking Server code (with the dependent files in parent directory):
```c
gcc -o srv serverf.c
```

