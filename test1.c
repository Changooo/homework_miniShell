#include <stdio.h>
int intlen(int n){
	if(!n) return 1;
	int l=0;
	for(; n; l++) n/=10;
	return l;
}
int main(){
	printf("%d %d\n", 0, intlen(0));
	printf("%d %d\n", 2, intlen(2));
	printf("%d %d\n", 10, intlen(10));
	printf("%d %d\n", 20, intlen(20));
	printf("%d %d\n", 100, intlen(100));
	printf("%d %d\n", 101, intlen(101));
	printf("%d %d\n", 2003, intlen(2003));
};
