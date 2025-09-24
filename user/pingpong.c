#include "kernel/types.h"
#include "user/user.h"

int
main(void)
{
  int p1[2]; // parent -> child
  int p2[2]; // child -> parent
  char buf[1];
  int pid;

  pipe(p1);
  pipe(p2);

  pid = fork();
  if(pid < 0){
    printf("fork failed\n");
    exit(1);
  }

  if(pid == 0){
    // 子进程
    close(p1[1]); // 子进程不写p1
    close(p2[0]); // 子进程不读p2

    // 从父进程读
    read(p1[0], buf, 1);
    printf("%d: received ping\n", getpid());

    // 回写给父进程
    write(p2[1], buf, 1);

    close(p1[0]);
    close(p2[1]);
    exit(0);
  } else {
    // 父进程
    close(p1[0]); // 父进程不读p1
    close(p2[1]); // 父进程不写p2

    // 先发一个字节
    buf[0] = 'x';
    write(p1[1], buf, 1);

    // 等子进程回传
    read(p2[0], buf, 1);
    printf("%d: received pong\n", getpid());

    close(p1[1]);
    close(p2[0]);
    wait(0);
    exit(0);
  }
}

