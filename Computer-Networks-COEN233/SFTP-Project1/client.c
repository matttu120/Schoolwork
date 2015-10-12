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
//client. The arguments are InputFileName, OutputFileName, HostName, PortNumber.

int send_filename(int, char*);

int main (int  argc, char * argv[])
{
	struct hostent *server;
	struct sockaddr_in si_server;
	//char buf[MAXLINE];
	int buflen, n;
	int PORTNO = atoi(argv[4]);
	server = gethostbyname(argv[3]);
	
	if (server==NULL) {
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&si_server, sizeof(si_server));
	si_server.sin_family = AF_INET;
	si_server.sin_port=htons(PORTNO);
	inet_pton(AF_INET, argv[1], &si_server.sin_addr.s_addr);

	if (sockfd < 0) error ("ERROR opening socket");	
	
	if (connect(sockfd,(struct sockaddr *)&si_server,sizeof(si_server)) < 0) 
 	       error("ERROR connecting");
	
	if (!send_filename(sockfd, argv[2])){
		printf("ERROR sending filename\n");
		error ("SENDING FILENAME");
	}
	printf("Created filename, now sending file contents \n");	
 
	int fp = open(argv[1], O_RDONLY);
	char buf[CHUNKSIZE]; 
		
	for(;;){
		int nbytes = read(fp, buf, CHUNKSIZE);
		buf[CHUNKSIZE] = '\0';
		if (!nbytes) break;
		printf("Buffer being written: %s \n", buf);
		write(sockfd, buf, nbytes);
	}
		   
	printf("File transferred.\n");
	close(fp);
	close(sockfd);
	return 0;
}

int send_filename(int sockfd, char* filename){
	char buf[100] = "STOR OLD ";
	strcat(buf,filename);
	
	printf("filename sending: %s \n", buf);
	write(sockfd, buf, strlen(buf));
	printf("after write, before read\n");
	int nbytes = read(sockfd, buf, 100);	
	if (*buf != '+') return 0;
	printf("FILENAME SENT: %s \n", buf);
	return 1;
}
