/*
	Project 1 Client
This project consists of building an SFTP (Simple File Transfer Protocol). It consists of a client and a server that exchange one file using TCP. 
The client accepts 4 arguments: the name of the two files (<input> and <output>), and the IP address (or Internet name) and the port number of the server. 
The server starts first and waits for a connection request. The client requests a connection and then sends the name of the file <output> for output and the 
data in file <input> to the server, which saves the info received in the file <output>. The client reads the file and sends the data in chunks of 10 bytes. 
After sending the file, the client closes the connection and exits. The server receives the data and writes the file in chunks of 5 bytes. 

The server needs to know when the transmission is over so that it can close the file. You need to come up with a mechanism for that! 
After executing, <input> and <output> should look the same. Your SFTP should be built on top of TCP. You must use C/C++ and Linux (any Unix will do it!). 
You will need to use the socket library. The man pages in the Unix/Linux systems have a lot of useful information. Start with <man socket>. There is a man 
page for each function in the socket library. Your SFTP should be able to transfer binary files as well as text files.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>

#define CHUNKSIZE 10
#define MAXLINE 2560000
//CLIENT
//Arguments are passed in the format INPUTFILE, OUTPUTFILE, HOSTNAME, PORTNUMBER
int send_filename(int, char*);

int main (int  argc, char * argv[])
{
	struct hostent *server;
	struct sockaddr_in si_server;
	char buf[CHUNKSIZE]; 
	int PORTNO = atoi(argv[4]);
	server = gethostbyname(argv[3]);
	
	if (server==NULL) {
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}

	//This section builds the socket information
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) error("ERROR opening socket");

	bzero(&si_server, sizeof(si_server));
	si_server.sin_family = AF_INET;
	si_server.sin_port=htons(PORTNO);
	inet_pton(AF_INET, argv[1], &si_server.sin_addr.s_addr);

	//connect	
	if (connect(sockfd,(struct sockaddr *)&si_server,sizeof(si_server)) < 0) 
 	       error("ERROR connecting");
	
	//Calls the send_filename() method declared below
	if (!send_filename(sockfd, argv[2])){
		printf("ERROR sending filename\n");
		error ("SENDING FILENAME");
	}
	printf("Created filename, now sending file contents \n");	

	//sending data to the created file on the server
	int fp = open(argv[1], O_RDONLY);
	for(;;){
		int nbytes = read(fp, buf, CHUNKSIZE);
		buf[CHUNKSIZE] = '\0';
		if (!nbytes) break;
		//printf("Buffer being written: %s \n", buf);
		write(sockfd, buf, nbytes);
	}
		   
	printf("File transferred.\n");
	close(fp);
	close(sockfd);
	return 0;
}
/* 
	This method takes the socket information and the destination filename declared in argv[2], and sends the filename information to the server.
	The buffer is initially a string with "header " in it, which is concatenated with the filename. The server will then receive the string, and 
	then will then change the "header" part of the message to "Success" in order to confirm that the filename was sent correctly.
*/
int send_filename(int sockfd, char* filename){
	char buf[100] = "header ";
	strcat(buf,filename);
	
	printf("filename sending: %s \n", buf);
	write(sockfd, buf, strlen(buf));
	int nbytes = read(sockfd, buf, 100);	
	if (*buf != 'S'){ 
		printf("Error creating file\n");
		return 0;
	}
	return 1;
}
