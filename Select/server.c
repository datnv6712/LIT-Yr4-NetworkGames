/*	
	Joe O'Regan
	K00203642

	08/11/2017 	Added struct for holding session state for each client
			Client had been causing problems with buffers overwriting
*/

#include <stdio.h>				// printf(), snprintf()
#include <string.h>				// 23/09/2017 Warning for strcpy, bzero()
#include <stdlib.h>				// 23/09/2017 exit(), rand(), srand()
#include <unistd.h>				// 23/09/2017 gethostname(), write(), read(), close()
#include <time.h>				// time()
#include "../Hangman.h"				// 13/10/2017 Hangman functions
#include "../DrawHangman.h"			// Display the hangman graphics
#include "../CreateTCPSocket.h"			// Create a TCP Server Socket

char clntName[INET_ADDRSTRLEN];			// Client address string
struct sockaddr_in	cliaddr, servaddr;	// Client address string
#define CLI_PORT 	ntohs(cliaddr.sin_port)
#define	LISTENQ		1024			/* 2nd argument to listen() */

struct sessionData {				// Store the game state, maybe use a linked list of structs instead of array
	int sock;				// The client socket that is connected
	char* word;				// Store the current full word
	char partWord[LINESIZE];		// Store the current part word
	int lives;				// Store the number of guesses left for each client
	int gameState;				// Store the game state for the connection
	char bufIn[LINESIZE];			// BUFFER
	char bufOut[LINESIZE];			// BUFFER
	char ip[INET_ADDRSTRLEN];		// client IP
	int port;				// client port
};

void closeSocketConnection(struct sessionData cli);

int main(int argc, char **argv) {	
	struct 		sessionData client[FD_SETSIZE];	
	int		i, j, maxi, maxfd, listenfd, connfd, sockfd, nready;
	ssize_t		n;
	fd_set		rset, allset;
	socklen_t	clilen;

	listenfd = createTCPServerSocket(TCP_PORT);									// Create the socket

	drawHangman();													// Draw the hangman graphic
	
	srand(time(NULL));												// Random word each time the server starts

	maxfd = listenfd;												/* initialize */
	maxi = -1;													/* index into client[] array */
	for (i = 0; i < FD_SETSIZE; i++) client[i].sock = -1;								/* -1 indicates available entry */
	FD_ZERO(&allset);												// Initialise the set
	FD_SET(listenfd, &allset);											// Add listenfd to the set

/* MAIN LOOP */
    for ( ; ; ) {
		rset = allset;												/* structure assignment */
		nready = select(maxfd+1, &rset, NULL, NULL, NULL);							// select() - used to monitor multiple file descriptors

/* CLIENT CONNECT */
		if (FD_ISSET(listenfd, &rset)) {									/* new client connection */
			clilen = sizeof(cliaddr);									// Size of the client address
			connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen);				// Accept a connection on connfd socket
			
			// Add client to clients array
			for (i = 0; i < FD_SETSIZE; i++) {										
				if (client[i].sock < 0) {								// If the array position is empty
					client[i].sock = connfd;	
/**/					// Store and display the client IP and Port 
					if (inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, client[i].ip,sizeof(client[i].ip)) != NULL){	// inet_ntop() - Convert IP address to human readable form
  						printf("Handling client %s/%d \n", client[i].ip, CLI_PORT);				// Display the client IP address and port number
						client[i].port = CLI_PORT;
					}										/* save descriptor */
/*====================================== initialise the variables for each client ======================================*/
/* CHOOSE WORD */ 			client[i].word = selectRandomWord(clntName, CLI_PORT);				// Select a random word from the list of words
					/* No letters are guessed Initially */
/*init part word			for (j = 0; j < strlen(client[i].word); j++) client[i].partWord[j]='-';		// Display hyphens to indicate size of word				 	
					client[i].partWord[j] = '\0';	
*/
					initPartWord(client[i].partWord, strlen(client[i].word));			// Hangman.h -> Display hyphens in place of letters, to indicate size of word 	
					client[i].lives = MAX_LIVES;
					client[i].gameState = 'I';
					printf("\nState %d Word: %s partWord: %s lives %d gameState %c\n\n", i,client[i].word,client[i].partWord,client[i].lives,client[i].gameState);
/*================================ Server sending 1st, not normal, might have to change ================================*/
/*================================ display first hangman graphic before entering guess =================================*/
/*send part word*/			sprintf (client[i].bufOut, "%s %d \n", client[i].partWord, client[i].lives);	// Set outbuf to the part word & the lives left
 					write (connfd, client[i].bufOut, LINESIZE);					// Send to client
/*======================================================================================================================*/
					break;
				}
			}
			if (i == FD_SETSIZE) printf("too many clients");						// err_quit("too many clients");

			FD_SET(connfd, &allset);									/* add new descriptor to set */
			if (connfd > maxfd) maxfd = connfd;								/* for select */
			if (i > maxi) maxi = i;										/* max index in client[] array */

			if (--nready <= 0) continue;									/* no more readable descriptors */
		}

/* FOR EACH CLIENT */
		for (i = 0; i <= maxi; i++) {										/* check all clients for data */
			if ((sockfd = client[i].sock) < 0) continue;							// Handle the next client in the array

			if (FD_ISSET(sockfd, &rset)) {									// Check the socket is in the descripter set (fd_set)
			  	if (client[i].gameState == 'I') {							// If the current clients state is 'I', play the game
/* READ CLIENT GUESS */
					if ((n = read(sockfd, client[i].bufIn, LINESIZE)) == 0) {			// Get the guess from the current client
						/* 4connection closed by client */
						close(sockfd);								// Close the connection to the client
						FD_CLR(sockfd, &allset);						// Remove the socket from the set
						// Reset Client and Client State
/*(
						client[i].sock = -1;							// Clear client from client index
						client[i].word = "";							// whole word for each client
						client[i].partWord[0] = '\0';						// part word for each client
						client[i].lives = MAX_LIVES;						// lives for each client
						client[i].gameState = 'I';						// game state for each client
						client[i].bufIn[0] = '\0';						// Clear client from client index
*/
						closeSocketConnection(client[i]);
					} else {
/*======================================================================================================================*/
/*  						    Hangman Code							*/
/*======================================================================================================================*/
		    				write (1, client[i].bufIn, n);/*OK*/					// Display client guess on server side
/* GET FOR EACH CLIENT */
/* Decrement */					if (!correctGuess(client[i].word, client[i].partWord, client[i].bufIn)) 
							client[i].lives--;						// If it's an incorrect guess, decrement the number of lives

/* Show The Word */				if ((client[i].gameState = checkGameState(client[i].word, client[i].partWord, client[i].lives)) == 'L') 
							strcpy (client[i].partWord, client[i].word);			// Show the player the word if they lose

/* WRITE BACK TO CLIENT*/			sprintf(client[i].bufOut, "%s %d \n", client[i].partWord, client[i].lives);// Add the part-word and number of guesses left to the buffer string
						write(sockfd, client[i].bufOut, strlen(client[i].bufOut));		// Send the string to the client

/* WIN MSG ONLY */				if (gameOverSelect(client[i].gameState,client[i].bufOut,client[i].word, sockfd, clntName, CLI_PORT)) {
							close(sockfd);
							FD_CLR(sockfd, &allset);						// Remove the socket from the set
							closeSocketConnection(client[i]);						// If game is finished, display win/lose message
						}
/* WIN MSG ONLY */				//  gameOverSelect(client[i].gameState,client[i].bufOut,client[i].word, sockfd, clntName, CLI_PORT);
					}
				}	// CHANGED POSITION end when game state not = I
				if (--nready <= 0) break;
			}
		}
	}
}


void closeSocketConnection(struct sessionData cli) {
	cli.sock = -1;							// Clear client from client index
	cli.word = "";							// whole word for each client
	cli.partWord[0] = '\0';						// part word for each client
	cli.lives = MAX_LIVES;						// lives for each client
	cli.gameState = 'I';						// game state for each client
	cli.bufIn[0] = '\0';						// Clear client from client index
}


