#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
int main(){
int x = 0;
int pid1, pid2, pid3;
if((pid1 = fork()) && (pid2 = fork())) {
   waitpid(pid1, NULL, 0);
   waitpid(pid2, NULL, 0);
   printf("%d\n", x);
   x++;
}
else {
   waitpid(pid1, NULL, 0);
   waitpid(pid2, NULL, 0);
   printf("%d\n", x);
}

pid3 = fork();
switch(pid3) {
   case 0:
      waitpid(pid3, NULL, 0);
      printf("%d\n", x);
      x++;
      break;
   default:
      waitpid(pid3, NULL, 0);
      printf("%d\n", x);
}
}
