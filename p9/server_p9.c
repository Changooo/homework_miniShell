#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define MAXLINE 80
int main (int argc, char *argv[]) {
	int n, listenfd, connfd, caddrlen, size;
	struct hostent *h;
	struct sockaddr_in saddr, caddr;
	char buf[MAXLINE];
	char fileName[60];
	char file[200000];
	char msg[30];
	FILE* ffd;
	int port = atoi(argv[1]);
	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("socket() failed.\n");
		exit(1);
	}
	memset((char *)&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);
	saddr.sin_port = htons(port);
	bzero(&(saddr.sin_zero), 8);
	if (bind(listenfd, (struct sockaddr*)&saddr, sizeof(saddr)) < 0) {
		printf("bind() failed.\n");
		exit(2);
	}
	if (listen(listenfd, 5) < 0) {
		printf("listen() failed.\n");
		exit(3);
	}
	while (1) {
		caddrlen = sizeof(caddr);
		if ((connfd = accept(listenfd, (struct sockaddr *)&caddr, (socklen_t *)&caddrlen)) < 0) {
			printf ("accept() failed.\n");
			continue;
		}
		while ((n = read(connfd, buf, MAXLINE)) > 0) {
			//read file name
			if(!strcmp(buf, "quit")) return 0;
			buf[strlen(buf)] = 0;
			sprintf(fileName, "%s_copy", buf);	
			ffd = fopen(fileName, "w");
			buf[0] = 1;
			buf[1] = 0;
			write(connfd, buf, 2);
	
			//read file size
			read(connfd, buf, 7);
			size = atoi(buf);
			buf[0] = 1;
			buf[1] = 0;
			write(connfd, buf, 2);

			//read file 
			read(connfd, file, size);
			fprintf(ffd, "%s", file);
			fclose(ffd);

			//ack
			sprintf(msg, "Recv %d bytes from client\n", size);
			write(1, msg, 30);
		}
		printf("connection terminated.\n");
		close(connfd);
	}
}
