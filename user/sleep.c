#include"kernel/types.h"
#include"user/user.h"
int main(int argn,char* argv[]){
	if(argn !=2){
	fprintf(2,"must 1 argumen for sleep!\n");
	exit(1);
	}
	int sticks=atoi(argv[1]);
	sleep(sticks);
	printf("(nothing happens for a little while)");
	exit(0);

}
