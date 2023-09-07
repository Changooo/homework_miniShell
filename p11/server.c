#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#define MAXUSER 10
#define MAXNAME 10
#define MAXLINE 80

int main (int argc, char *argv[]) {

    int n, listenfd, connfd, caddrlen, fdmax, fdnum, usernum;
    struct hostent *h;
    struct sockaddr_in saddr, caddr;
    char buf[MAXLINE];
    char name[MAXUSER][MAXNAME];
    char temp[MAXLINE+MAXNAME+1];
    int port = atoi(argv[1]);
    fd_set readset, copyset;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("socket() failed.\n");
        exit(1);
    }

    memset((char *)&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_port = htons(port);

    if (bind(listenfd, (struct sockaddr *)&saddr, sizeof(saddr)) < 0) {
        printf("bind() failed.\n");
        exit(2);
    }

    if (listen(listenfd, 5) < 0) {
        printf("listen() failed.\n");
        exit(3);
    }

    FD_ZERO(&readset);
    FD_SET(listenfd, &readset);
    fdmax = listenfd;
    usernum = 0;

    while (1) {

        copyset = readset;

        if((select(fdmax+1, &copyset, NULL, NULL, NULL ))< 0 ) { // FILL CONDITION
            printf("select() failed.\n");
            exit(4);
        }

       for (int i = 3; i < fdmax + 1; i++) {

           memset(buf, 0, MAXLINE);
           memset(temp, 0, MAXLINE+MAXNAME+1);

            if (FD_ISSET(i, &copyset)) {

                if (i == listenfd) {
                    
                    caddrlen = sizeof(caddr);
                    if ((connfd = accept(listenfd, (struct sockaddr *)&caddr, (socklen_t *)&caddrlen)) < 0) {
                        printf ("accept() failed.\n");
                        continue;
                    }

                    read(connfd, name[connfd - 4], MAXNAME);
                    sprintf(temp, "%s joined. %d current users\n", name[connfd-4], ++usernum);
		    //START
		    for(int k=3; k<fdmax+1; k++){
			    if(FD_ISSET(k, &readset) && k!=i) {
				    write(k, temp, strlen(temp));
			    }
		    }
		    //write(connfd, temp, strlen(temp));
		    //END
                    FD_SET(connfd, &readset);
                    if (fdmax < connfd)
                        fdmax = connfd;

                }
                else {

                    if ((n = read(i, buf, MAXLINE)) > 0) {
                        printf ("got %d bytes from user %s.\n", n, name[i-4]);
			sprintf(temp, "%s:%s", name[i-4], buf);
			//START
			for(int k=3; k<fdmax+1; k++){
                            if(FD_ISSET(k, &readset) && k!=i && k!=listenfd) {
                                    write(k, temp, strlen(temp));
                            }
                    	}
			//END
                    }
                    else {

                        sprintf(temp, "%s leaved. %d current users.\n", name[i-4], --usernum);
			//START
			for(int k=3; k<fdmax+1; k++){
                            if(FD_ISSET(k, &readset) && k!=i && k!=listenfd) write(k, temp, strlen(temp));
                    	}
			//END
                        FD_CLR(i, &readset);
                        close(i);
                        name[i-4][0] = '\0';

                    }
                }
            }  
        }
    }

    return 0;

}

