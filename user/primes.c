#include "kernel/types.h"
#include "user/user.h"

void sieve(int leftfd){
	int prime;
	int num;
	int p[2];
	if(read(leftfd,&prime,sizeof(prime))==0){
	close(leftfd);
	exit(0);
	}
	printf("prime %d\n",prime);

	if(pipe(p)<0){
	printf("pipe failed\n");
	exit(1);
	}

	int pid=fork();
	if(pid<0){
	printf("fork failed\n");
	exit(1);
	}

	if(pid==0){
	//child process :read
	close(p[1]);
	close(leftfd);
	sieve(p[0]);
	exit(0);
	}else{
	close(p[0]);
	while(read(leftfd,&num,sizeof(num))>0){
	if(num%prime!=0){
	write(p[1],&num,sizeof(num));
	}
	}
	close(leftfd);
	close(p[1]);
	wait(0);
	exit(0);
	}
}

	int main(void){
		int p[2];
		if(pipe(p)<0){
		printf("pipe failed\n");
		exit(1);
		}
		
		int pid=fork();
		if(pid<0){
		printf("fork failed\n");
		exit(1);
		}

		if(pid==0){
			close(p[1]);
			sieve(p[0]);
			exit(0);
		}else{
			close(p[0]);
			for(int i=2;i<=35;i++){
				write(p[1],&i,sizeof(i));
			}
			close(p[1]);
			wait(0);
			exit(0);
		}
	}
