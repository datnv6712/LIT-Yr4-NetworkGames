#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>					// printf()
#include <errno.h>
#include <string.h>					// 23/09/2017 Warning for strcpy, bzero()
#include <stdlib.h>					// 23/09/2017 Warning for exit
#include <unistd.h>					// 23/09/2017 gethostname(), write(), read(), close()
#include <arpa/inet.h>					// 23/09/2017 inet_ntop()
#include "../Hangman.h"					// 11/10/2017 Hangamen header file
#include "../DrawHangman.h"				// 11/10/2017 Draw Hangman Graphic
	
#define	SERV_PORT	9877	

void str_echo(int sockfd);
void str_cli(FILE *fp, int sockfd);
static ssize_t my_read(int fd, char *ptr);
ssize_t readline(int fd, void *vptr, size_t maxlen);

// Parse server data to these variables
char arg1PartWord[20];
int arg2LivesLeft;
int count;

void show1(char* msg){
	printf("\nMessage Received Outside Loop: %s", msg);
}
void show2(char* msg){
	printf("\nMessage Received Inside Loop: %s", msg);
}
void getInput(int sock, char* recv);

int main(int argc, char **argv) {
	int			sockfd;
	struct sockaddr_in	servaddr;

	if (argc != 2)
		//err_quit("usage: tcpcli <IPaddress>");
		printf("usage: tcpcli <IPaddress>");

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);	// XXX

	connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));	// XXX

	str_cli(stdin, sockfd);		/* do it all */

	exit(0);
}


void str_cli(FILE *fp, int sockfd) {
	char	sendline[LINESIZE], recvline[LINESIZE];

	getInput(sockfd,recvline);
/*
	while (fgets(sendline, LINESIZE, fp) != NULL) {
		write(sockfd, sendline, strlen(sendline));

		getInput(sockfd,recvline);
	}
*/


//	while ((count = read(sockfd, recvline, LINESIZE)) > 0) {
	while (fgets(sendline, LINESIZE, fp) != NULL) {
 	    	write (sockfd, sendline, strlen(sendline));					// Send client input to server

		count = read (sockfd, recvline, LINESIZE);	
		sscanf(recvline, "%s %d", &(*arg1PartWord), &arg2LivesLeft);			// Parse string data received from server into separate part-word and score variables
		selectLives(arg2LivesLeft);							// Display graphical represenation of lives left
		printf("%s\n", arg1PartWord);
 	    	
		//count = read (0, sendline, LINESIZE);						// 0 = STDIN

 	}



	write(1,"Game Over!\n",LINESIZE);	
}

void getInput(int sock, char* recv){
	if (readline(sock, recv, LINESIZE) == 0) printf("str_cli: server terminated prematurely");

	sscanf(recv, "%s %d", &(*arg1PartWord), &arg2LivesLeft);	// Separate the part word and guesses
	selectLives(arg2LivesLeft);					// Display hangman graphic & guesses left
	fputs(arg1PartWord, stdout);					// Display the part word without the guesses
	fputs("\n", stdout);
}



static int	read_cnt;
static char	*read_ptr;
static char	read_buf[LINESIZE];

static ssize_t my_read(int fd, char *ptr) {

	if (read_cnt <= 0) {
again:
		if ( (read_cnt = read(fd, read_buf, sizeof(read_buf))) < 0) {
			if (errno == EINTR)
				goto again;
			return(-1);
		} else if (read_cnt == 0)
			return(0);
		read_ptr = read_buf;
	}

	read_cnt--;
	*ptr = *read_ptr++;
	return(1);
}

ssize_t readline(int fd, void *vptr, size_t maxlen) {
	ssize_t	n, rc;
	char	c, *ptr;

	ptr = vptr;
	for (n = 1; n < maxlen; n++) {
		if ( (rc = my_read(fd, &c)) == 1) {
			*ptr++ = c;
			if (c == '\n') break;	/* newline is stored, like fgets() */
		} else if (rc == 0) {
			*ptr = 0;
			return(n - 1);	/* EOF, n - 1 bytes were read */
		} else
			return(-1);		/* error, errno set by read() */
	}

	*ptr = 0;	/* null terminate like fgets() */
	return(n);
}
