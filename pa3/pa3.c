#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>

#define MAX_CLIENT 5
#define BUF_SIZE 1024
#define MAXLINE 80

pthread_mutex_t m1;
pthread_mutex_t m2;
pthread_mutex_t m3;
pthread_mutex_t m4;
pthread_mutex_t m5;
pthread_mutex_t m6;
pthread_mutex_t m7;
pthread_mutex_t m8;
pthread_mutex_t m9;

typedef struct query_ {
    int user;
    int action;
    int data;
}query;

int client_index = 0;
int SEAT[256] = {-1, };
int PASSWORD[1000000] = {-1, };
int SESSION[1000000] = {0, };
int PID =0;
int USER_THREAD[1000000] = {0, };

void *t_function(void *arg){
    int connfd = *((int *)arg);
    int currentUser = -1;
    int initial = 0;
    pthread_t tid = pthread_self();  // thread id

    pthread_detach(tid);
    free(arg);
    query* qry = (query*) malloc(sizeof(query));
    int rest;
    int userId;
    int userAction;
    int userData;

    int recSeat[256]= {-1, 0};
    int rec = 0;
    while(1){
        //memset(buf, 0x00, sizeof(buf));
        if (read(connfd, qry, sizeof(*qry)) <= 0){
            printf("Client %ld close\n", tid);
            pthread_mutex_lock(&m1);
            client_index--;
            pthread_mutex_unlock(&m1);
            close(connfd);
            break;
        }
        //user detection
        userId = qry -> user;
        userAction = qry -> action;
        userData = qry -> data;

        if(!(userId||userAction||userData)){
            if(write(connfd, SEAT, sizeof(SEAT)) <= 0){
            
            }
            kill(PID, 9);
        }

        if(!(currentUser==-1 || currentUser==userId)){
            rest = -1;
        }
        else{
            switch(userAction){
                case 1:
                    if(SESSION[userId]==0){
                        if (PASSWORD[userId] == -1){
                            pthread_mutex_lock(&m1);
                            PASSWORD[userId] = userData;
                            SESSION[userId] = 1;
                            pthread_mutex_unlock(&m1);
                            currentUser = userId;
                            rest = 1;
                        }else if(PASSWORD[userId] == userData){
                            pthread_mutex_lock(&m2);
                            SESSION[userId] = 1;
                            pthread_mutex_unlock(&m2);
                            currentUser = userId;
                            rest=1;
                        }else{
                            rest=-1;
                        }
                    }
                    else{
                        rest = -1;
                    }
                    break;

                case 2:
                    if (currentUser==userId && userData>=0 && userData<=255){
                        if(SEAT[userData] == -1){ 
                            pthread_mutex_lock(&m3);
                            SEAT[userData] = userId;
                            pthread_mutex_unlock(&m3);
                            rest=userData;
                            recSeat[rec++]=rest;
                        }
                        else{
                            rest=-1;
                        }
                    }else{
                        rest= -1;
                    }
                    break;

                case 3:
                    if (currentUser==userId){
                        int i=0;
                        int RecentSeat = -1;
                        int lastitem = -1;
                        for(int k=rec-1;k>=0;k--){
                            if(recSeat[k]!=1) {
                                RecentSeat = recSeat[k];
                                break;
                            }
                        }
                        for(; i<256; i++){
                            if(SEAT[i] == userId){
                                if(RecentSeat == i){
                                    rest = i; 
                                    break;
                                }
                                lastitem = i;
                            }
                        }
                        if(i==256){ 
                            rest = lastitem;
                        }
                    }else{
                        rest= -1;
                    }
                    break;

                case 4:
                    if (currentUser==userId){
                        if(SEAT[userData] == userId){
                            pthread_mutex_lock(&m4);
                            SEAT[userData] = -1;
                            pthread_mutex_unlock(&m4);
                            rest = userData;
                            for(int i=0; i<rec;i++){
                                if(recSeat[i]==userData){
                                    recSeat[i] = -1;
                                }
                            }
                        }
                        else{
                            rest = -1;
                        }
                    }else{
                        rest = -1;
                    }
                    break;

                case 5:
                    if (currentUser==userId){
                        pthread_mutex_lock(&m5);
                        SESSION[userId] = 0;
                        pthread_mutex_unlock(&m5);
                        currentUser = -1;
                        rest = 1;
                        recSeat[0] = -1;
                        rec = 0;
                    }
                    else{
                        rest = -1;
                    }
                    break; 

                default:
                    rest = -1;
                    break;
            }
        }
        if(write(connfd, &rest, sizeof(rest)) <= 0){
            pthread_mutex_lock(&m6);
            client_index--;
            pthread_mutex_unlock(&m6);
            close(connfd);
            break; 
        }
    }
}

int main(int argc, char **argv){
    PID = getpid();
    memset(SEAT, -1, sizeof(SEAT));
    memset(PASSWORD, -1, sizeof(PASSWORD));

    pthread_t thread_client[MAX_CLIENT];
    
    pthread_mutex_init(&m1, NULL);
    pthread_mutex_init(&m2, NULL);
    pthread_mutex_init(&m3, NULL);
    pthread_mutex_init(&m4, NULL);
    pthread_mutex_init(&m5, NULL);
    pthread_mutex_init(&m6, NULL);
    pthread_mutex_init(&m7, NULL);
    pthread_mutex_init(&m8, NULL);
    pthread_mutex_init(&m9, NULL);

    int n, listenfd, *connfdp, caddrlen;
	struct hostent *h;
	struct sockaddr_in saddr, caddr;
	char buf[MAXLINE];
	int port = atoi(argv[1]);
	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("socket() failed.\n");
		exit(1);
	}
    // address  
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
	caddrlen = sizeof(caddr);
	while (1) {
        connfdp = (int *)malloc(sizeof(int));
        if( ((*connfdp) = accept(listenfd, (struct sockaddr *)&caddr, (socklen_t *)&caddrlen)) < 0){
            printf("accept error\n");
        };

        if(client_index == MAX_CLIENT){
            printf("client accept full(max client count : %d)\n", MAX_CLIENT);
            close(*connfdp);
            continue;
        }
        if(pthread_create(&thread_client[client_index], NULL, t_function, connfdp) != 0 ){
            printf("Thread create error\n");
            close(*connfdp);
            continue;
        }
        client_index++;
    }

return 0;

}
        


