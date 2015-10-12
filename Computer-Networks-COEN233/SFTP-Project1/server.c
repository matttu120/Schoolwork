#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#define backlog 5
#define BUFSIZE 5
#define SIZE 100

//server. Pass in the desired port number as the only argument upon execution.
int create_file(int);

int main (int argc, char * argv[]){
	struct sockaddr_in si_server, cli_addr;
	char buf[BUFSIZE];
	int len;
	int n=1;
	int PORTNO = atoi(argv[1]);
	int connection;	
	
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero((char *)&si_server, sizeof(si_server));
	si_server.sin_family = AF_INET;
	si_server.sin_addr.s_addr = htonl(INADDR_ANY);
	si_server.sin_port = htons(PORTNO);
	
	socklen_t cli_len;
	//bind
	bind(sockfd, (struct sockaddr*)&si_server, sizeof(si_server));
	
	//listen
	listen(sockfd, backlog);
	
	//accept
	cli_len = sizeof(cli_addr);
	connection = accept(sockfd, (struct sockaddr *)&cli_addr, &cli_len);
	printf("%d\n", connection);
	int fp = create_file(connection);
	if (!fp) error("ERROR CREATE");
	
	for(;;){
		int nbytes = read(connection, buf, BUFSIZE);
		buf[BUFSIZE] = '\0';
		if (!nbytes) break;
		printf("buffer = %s \n", buf);
		write(fp, buf, nbytes);
	}
	close(sockfd);	
	close(fp);
	return 0;
}

int create_file(int connection){
	int fp;
	char buf[SIZE];
	printf("before reading: connection is %d \n", connection);
	
	int nbytes = read(connection, buf, SIZE);
	buf[nbytes] = '\0';
	if (strncmp(buf,"STOR OLD", 8)){
		write(connection, "- CREATE FAILED", 15);
		return 0;
	}
	printf("before opening \n");
	fp = open(buf+9, O_CREAT, 0640);
	fp = open(buf+9, O_WRONLY);
	if (!fp){ 
		write(connection, "- CREATE FAILED", 15);
		return 0;
	}	
	write(connection, "+ CREATE SUCCESS", 16);
	return fp;
}
