#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#define MAXLINE 80

int main (int argc, char *argv[]) {
	int n, cfd, ffd, file_len;
	struct hostent *h;
	struct sockaddr_in saddr;
	char buf[MAXLINE];
	char file[200000];
	char fileName[50];
	char *host = argv[1];
	int port = atoi(argv[2]);
	if ((cfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("socket() failed.\n");
		exit(1);
	}
	if ((h = gethostbyname(host)) == NULL) {
		printf("invalid hostname %s\n", host);
		exit(2);
	}
	memset((char *)&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	memcpy((char *)&saddr.sin_addr.s_addr, (char *)h->h_addr, h->h_length);
	saddr.sin_port = htons(port);
	if (connect(cfd,(struct sockaddr *)&saddr, sizeof(saddr)) < 0) {
		printf("connect() failed.\n");
		exit(3);
	}	
	while ((n = read(0, buf, MAXLINE)) > 0) {
		// read file
		buf[strlen(buf)-1] = 0;
		ffd = open(buf, O_RDONLY);
		file_len = 0;
		while(read(ffd, &file[file_len++], 1) > 0){
		}
		// send file name
		write(cfd, buf, n);
		if(!strcmp(buf, "quit")){
			return 0;
		}
		read(cfd, buf, 2);
		
		// send file size
		char sizebuf[10] = {0, };
		sprintf(sizebuf, "%d", file_len);
		write(cfd, sizebuf, 7);
		read(cfd, buf, 2);

		// send file 
		write(cfd, file, file_len);
		char msg[30];
		sprintf(msg, "Send %d bytes to server\n", file_len);
		write(1, msg, 30);
	}
	close(cfd);
}
