 /* 
 	File: 		CreateTCPSocket.h
	Version: 	Functions to create and connect TCP sockets
	Author:		Joe O'Regan

	Year 4 Networked Games Assignment

	Team 1:
	Joe O'Regan 	K00203642
	Samantha Marah	K00200782
	Jason Foley 	K00186690

	Reconfigured version of SetupTCPServerSocket() function from TCPServerUtility.c
	from TCP/IP Sockets in C book, to work with TCP Hangman fork() server

	16/11/2017 Added HandleErrors.h for error handling
*/

#include <sys/socket.h>									// sockaddr, accept(), bind(), listen()
#include <unistd.h>									// gethostname(), write(), read(), close()
#include <arpa/inet.h>									// inet_ntop()
#include <netdb.h>									// gethostbyname()
#include <string.h>									// strcpy(), bzero()
#include "Hangman.h"
#include "AddressFunctions.h"
#include "HandleErrors.h"								// Display error messages

#define	max(a,b) ((a) > (b) ? (a) : (b))						// Return the largest of 2 numbers

#define SRV_IP "127.0.0.1"								// IPv4 Address of server on local machine
#define TCP_PORT_NUM 1066								// The port number the server will run on
#define	MAX_CLIENTS 5									// 2nd argument to listen(), the maximum number of client connections
char* TCP_PORT = "1066";								// The port number the server will run on, for createTCPServerSocket() function

int createDualStackServerSocket(){
	struct sockaddr_in6 server;
	int i, sock, reuseaddr = 1;
	struct hostent * host_info;							// Can only return IPv4 addresses
	struct in_addr **addr_list;
	char* s;
	//char str[INET6_ADDRSTRLEN];

	bzero(&server, sizeof server);							// Zero out structure, could also use memset(), swapping 2 args in bzero will be caught by compiler

	sock = socket(AF_INET6, SOCK_STREAM, 0);
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof reuseaddr);
	if (sock < 0) 
		displayErrMsgStatus("Creating Stream Socket", 1);
	
 	server.sin6_family = AF_INET6;
 	server.sin6_addr = in6addr_any;
 	server.sin6_port = htons(TCP_PORT_NUM);

 	if (bind(sock, (struct sockaddr *) & server, sizeof(server)) < 0) 		// Bind - Assign the address to the socket
		displayErrMsgStatus("Binding Socket", 1);
	if (listen(sock, MAX_CLIENTS) < 0)						// Set socket to listen
		displayErrMsgStatus("Listen()", 1);
	else {
		// Display the local machine Name, IP, and port the server is running on
		char hostname[128];
		if (gethostname(hostname, sizeof hostname) == 0)			// gethostname() returns 0 on success, returns the name of the computer the socket is running on
			printf("Running Hangman Server on: %s ", hostname);

		host_info = gethostbyname(hostname);					// gethostbyname() uses the name of the machine to return the local IP address
		addr_list = (struct in_addr **) host_info->h_addr_list;
		for (i = 0; addr_list[i] != NULL; i++) 
			printf("%s", inet_ntoa(*addr_list[i]));				// inet_ntoa() converts the address to human readable format, deprecated, replaced by inet_ntop()
		printf(":%d\n",TCP_PORT_NUM);


		//inet_ntop(AF_INET6, &(server.sin6_addr),str,INET6_ADDRSTRLEN);	// display IPv6 address
		//printf("%s\n", str);

		//s = get_ip_str((struct sockaddr *)server,s,INET_ADDRSTRLEN);

	}

	return sock;	
}

int createTCPServerSocket() {
  // Construct the server address structure
  struct addrinfo addrCriteria;                   					// Criteria for address match
  memset(&addrCriteria, 0, sizeof(addrCriteria)); 					// Zero out structure
  addrCriteria.ai_family = AF_UNSPEC;             					// Any address family
  addrCriteria.ai_flags = AI_PASSIVE;             					// Accept on any address/port
  addrCriteria.ai_socktype = SOCK_STREAM;         					// Only stream sockets
  addrCriteria.ai_protocol = IPPROTO_TCP;         					// Only TCP protocol

  struct addrinfo *servAddr; 								// List of server addresses
  int rtnVal = getaddrinfo(NULL, TCP_PORT, &addrCriteria, &servAddr);
  if (rtnVal != 0) printf("getaddrinfo() failed %s\n", gai_strerror(rtnVal));		// DieWithUserMessage("getaddrinfo() failed", gai_strerror(rtnVal));

  int server = -1;									// Initialise the server socket
  for (struct addrinfo *addr = servAddr; addr != NULL; addr = addr->ai_next) {   
    server = socket(servAddr->ai_family, servAddr->ai_socktype, servAddr->ai_protocol); // Create a TCP socket
    if (server < 0) continue;       							// Socket creation failed; try next address

    if ((bind(server, servAddr->ai_addr, servAddr->ai_addrlen) == 0) &&			// Bind to the local address 
      (listen(server, MAX_CLIENTS) == 0)) {						// Set socket to listen
    
      // Print local address of socket
      struct sockaddr_storage localAddr;
      socklen_t addrSize = sizeof(localAddr);
      if (getsockname(server, (struct sockaddr *) &localAddr, &addrSize) < 0) 
		displayErrMsg("getsockname() failed");

      fputs("Server now running on ", stdout);
      displayAddress((struct sockaddr *) &localAddr, stdout);				// Display the address/port for the socket
      fputc('\n', stdout);								// Add new line

      break;       									// Bind and list successful
    }

    close(server);  									// Close and try again
    server = -1;
  }

  // Free address list allocated by getaddrinfo()
  freeaddrinfo(servAddr);								// frees the memory that was allocated for the dynamically allocated linked list 

  return server;
}

struct sockaddr_in createTCPClientSocket(int* sock, char* server_name) {		/* CREATE THE SOCKET */
	struct sockaddr_in servAddr;
 	struct hostent * host_info;
 	
	 /* CREATE THE SOCKET */
 	(*sock) = socket(AF_INET, SOCK_STREAM, 0);					// AF_INET = Address Family Internet, SOCK_STREAM = streams
 	if (sock < 0) displayErrMsgStatus("Creating Stream Socket", 1);	

 	host_info = gethostbyname(server_name);
 //	if (host_info == NULL) {
 //	  fprintf (stderr, "%s: unknown host:%s \n", argv [0], server_name);
 //	  exit (2);
 //	}

/* SET UP THE SERVER'S SOCKET ADDRESS, AND CONNECT */
	bzero(&servAddr, sizeof(servAddr));						// Zero out the address

	//servaddr.sin_family = AF_INET;
 	servAddr.sin_family = host_info->h_addrtype;
 	memcpy ((char *) & servAddr.sin_addr, host_info->h_addr, host_info->h_length);
 	servAddr.sin_port = htons(TCP_PORT_NUM);
	//inet_pton(AF_INET, (argc == 1) ?  SRV_IP : argv[1], &servaddr.sin_addr);

 	if (connect ((*sock), (struct sockaddr *) &servAddr, sizeof servAddr) < 0)	// If the connection fails
		displayErrMsgStatus("Connecting To Server", 3);				// Display the error message, and exit with status 3

 	printf ("Connected to server: %s \n", server_name);	

	return servAddr;
}


// Display Client address and port
char* displayNameAndPort(struct sockaddr_in* cli, char* name, int sock) {
	int clilen = sizeof(cli);

	getpeername(sock, (struct sockaddr*) &cli, &clilen);
//	char name[INET_ADDRSTRLEN];							// Client address string
	//inet_pton(AF_INET, "127.0.0.1", &(client.sin_addr));
	//if (inet_ntop(AF_INET, &cli.sin_addr.s_addr, name,sizeof(name)) != NULL){	// sizeof(name) not working here, INET_ADDRSTRLEN is the length of an address string
	if (inet_ntop(AF_INET, &(*cli).sin_addr.s_addr, name, INET_ADDRSTRLEN) != NULL){// Convert the address to a string, and store in clntName
		printf("Handling client %s/%d\n", name, ntohs((*cli).sin_port));	// Display the client IP address and port number, ntohs = convert from network byte order to host byte order
	}

	return name;	
}
