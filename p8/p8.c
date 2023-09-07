#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <sys/msg.h>
#include <unistd.h>
#include <time.h> 


#define N_MSG 10
#define T_MSG 20


struct msg
{
	long msgtype;
	int sender; 
	char text[512]; 
};


struct msg_rt
{
	long msgtype;
	char timestamp [512]; 
	
};


int main()
{
	key_t key = ftok(".", 'a');
	int uid; 
	int receiver_id, pid; 
	printf("my id is \n"); 
	scanf("%d", &uid);
	getchar();  
	printf("who to send ? \n");
	scanf("%d", &receiver_id); 
	getchar();
	int qid = msgget(key, IPC_CREAT|0x1ff); 
	if (qid < 0) 
	{
		perror("msgget faild");
	} //Please do not modify up to this point
	if ((pid = fork()) == 0) 
	{
		while (1)
		{
			struct msg buf;
			struct msg_rt read_time;
			msgrcv(qid, (void*)&buf, sizeof(struct msg), 1, 0);
			//msgrcv(qid, (void*)&read_time, sizeof(struct msg_rt), 2, 0);
			printf("%d\n", buf.sender);
			if(buf.sender == uid){
				msgrcv(qid, (void*)&read_time, sizeof(struct msg_rt), 2, 0);
				printf("%s", read_time.timestamp);
			}else{
				printf("RECEIVED: %s\n", buf.text);
				read_time.msgtype = 2;
				time_t rawtime;
				time(&rawtime);
				sprintf(read_time.timestamp, "ACK FROM %d AT %s", uid, ctime(&rawtime));
				msgsnd(qid, (void*)&read_time, sizeof(struct msg_rt), 0);
			}
		}		
	
	}
	else
	{
		while (1) 
		{
			struct msg buf; 
			//FILL YOUR CODE
			buf.msgtype = 1;
			gets(buf.text);
			if(!strcmp((buf.text), "quit")){
				kill(pid, 2);
				printf("QUIT\n");
				return 0;
			}
			buf.sender = uid;
			msgsnd(qid, (void*)&buf, sizeof(struct msg), 0);
		}
	}	


	return 0;
}
