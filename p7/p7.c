#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_ARG 128
#define MAX_PATH_LEN 128

void make_tokens(char* cmd, char* arg[], char* arg2[], char* target) {
    char *left, *right, *ptr;
    int num = 0;

    left = strtok(cmd, target);
    strcat(left, "\0");
    right = strtok(NULL, target);
    strcat(right, "\0");

    ptr = strtok(left, " ");
    while (ptr != NULL) {
        arg[num++] = ptr;
        ptr = strtok(NULL, " ");
    }
    arg[num] = NULL;

    num = 0;
    ptr = strtok(right, " ");
    while (ptr != NULL) {
        arg2[num++] = ptr;
        ptr = strtok(NULL, " ");
    }
    arg2[num] = NULL;

    return;
}

int main () {
    size_t size;
    char *cmd, *ptr;
    char *arg[MAX_ARG], *arg2[MAX_ARG];
    char path[MAX_PATH_LEN], path2[MAX_PATH_LEN];
    int child_status;
    int fd[2];
    int fdr;
    
    while(1) {
        int num = 0;
        cmd = NULL;
        getline(&cmd, &size, stdin);
        cmd[strlen(cmd)-1] = '\0';

	// TERMINATE PROGRAM
        if(strcmp(cmd, "quit") == 0) {
            break;
        }

	// PIPE 
        if(strchr(cmd, '|') != NULL) {
            make_tokens(cmd, arg, arg2, "|");
            
            sprintf(path, "/bin/%s", arg[0]);
            sprintf(path2, "/bin/%s", arg2[0]);

            if(pipe(fd) == -1)
                exit(1);

            if(fork() == 0) {
		pipe(fd);
		if(fork() > 0){
			dup2(fd[1], 1);
			close(fd[0]);
			execv(path, arg);
		}	
		else{
			dup2(fd[0], 0);
			close(fd[1]);
			execv(path2, arg2);
		}
		//FILL YOUR CODES
            }
            else
                wait(&child_status);

        } 
	// > REDIRECTION
        else if(strchr(cmd, '>') != NULL) {
            make_tokens(cmd, arg, arg2, ">");
            sprintf(path, "/bin/%s", arg[0]);

            if(fork() == 0) {
		fdr = open(arg2[0], O_RDWR | O_CREAT, 0755 );
		dup2(fdr, STDOUT_FILENO);
		execv(path, arg);
		dup2(1, 1);
		close(fdr);
		//FILL YOUR CODES
            }
            else
                wait(&child_status);

        }
	// < REDIRECTION
        else if(strchr(cmd, '<') != NULL) {
            make_tokens(cmd, arg, arg2, "<");
            sprintf(path, "/bin/%s", arg[0]);

            if(fork() == 0) {
		fdr = open(arg2[0], O_RDWR );
		dup2(fdr, STDIN_FILENO);
		execv(path, arg);
		dup2(0,0);
		close(fdr);
		//FILL YOUR CODES

            }
            else
                wait(&child_status);

        }
	// ONLY SINGLE COMMAND
        else {
            ptr = strtok(cmd, " ");
            while (ptr != NULL) {
                arg[num++] = ptr;
                ptr = strtok(NULL, " ");
            }
            arg[num] = NULL;

            sprintf(path, "/bin/%s", arg[0]);
        
            if (fork() == 0) {
                execv(path, arg);
                exit(0);
            }
            else
                wait(&child_status);            
        }
    }
    return 0;
}
