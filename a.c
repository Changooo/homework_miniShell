#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#define N 10
pid_t pid[N];
int ccount = 0;
void handler (int sig) {
pid_t id = wait(NULL);
ccount--;
printf ("Received signal %d from pid %d\n", sig, id);
}
int main(void) {
int i;
ccount = N;
signal (SIGCHLD, handler);
for (i = 0; i < N; i++) {
if ((pid[i] = fork()) == 0) {
exit(0); /* child */
}
}
while (ccount > 0)
sleep (5);
return 0;
}
