 /* Network server for hangman game */
 /* File: hangserver.c */

#include <stdio.h>								// printf()
#include <stdlib.h> 								// rand(), exit() atoi(), srand()
#include "../DrawHangman.h"							// Functions to draw the games graphics
#include "../CreateTCPSocket.h"							// Functions to handle socket creation and connection
#include "../TCPPlayHangman.h"							// Moved hang_man() function to separate file, with functions split for TCP

extern time_t time();

int main (int argc, char * argv []) {						// Added return type & user can specity the port to accept Client connections
 	struct sockaddr_in6 server, client;					// IPv6 address structure to hold Client address details
 	int sock, fd, client_len, cliPort;					// Client socket, server socket, size of client, Client port number
	char cliName[INET6_ADDRSTRLEN];						// Client address string
	socklen_t len = sizeof client;						// Size of the address stucture

 	srand ((int) time((long *) 0)); 					// Randomize the seed for selecting the word to guess

	sock = createDualStackServerSocket((argc == 2) ? argv[1] : TCP_PORT);	// CreateTCPSocket.h: Moved socket create, bind, listen, functionality here

	drawHangmanLogo(DS_SRV_TCP);						// Draw the game logo, with socket title parameter

 	while (1) {
 		client_len = sizeof (client);
 		if((fd=accept(sock,(struct sockaddr*)&client,&client_len)) < 0)	// Accept client connections
			displayErrMsgStatus("Accepting Connection", 3);		// If accept return an error display message and exit with status 3

		displayIPAndPort((struct sockaddr_storage* )&client, len, 	// CreateTCPSocket.h: Display the Client IP and Port number
					cliName, &sock, &cliPort);

 		playHangmanTCP(fd, fd, cliName, cliPort);			// TCPPlayHangman.h: Play the game

 		close (fd);							// Close the socket
 	}

	return 0;
 }
