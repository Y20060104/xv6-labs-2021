#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char *argv[]) {
    char buf[512];     // 用于读取一行输入
    int n = 0;         // 当前行长度
    char *args[MAXARG]; // 命令行参数数组
    int i;

    if (argc < 2) {
        fprintf(2, "Usage: xargs <command> [args...]\n");
        exit(1);
    }

    // 先把 argv[1...] 拷贝到 args 前面
    for (i = 1; i < argc && i < MAXARG; i++)
        args[i-1] = argv[i];

    int base = i - 1; // 命令长度，方便后面追加行参数

    while (1) {
        char c;
        int r = read(0, &c, 1); // 从 stdin 读一个字符
        if (r <= 0)
            break; // EOF

        if (c == '\n') {
            if (n == 0)
                continue; // 空行跳过

            buf[n] = 0; // 字符串结尾

            // 将行追加到 args
            args[base] = buf;
            args[base+1] = 0; // argv 结尾

            int pid = fork();
            if (pid < 0) {
                fprintf(2, "xargs: fork failed\n");
                exit(1);
            } else if (pid == 0) {
                exec(args[0], args);
                fprintf(2, "xargs: exec %s failed\n", args[0]);
                exit(1);
            } else {
                wait(0);
            }

            n = 0; // 重置缓冲区
        } else {
            if (n < sizeof(buf) - 1)
                buf[n++] = c;
        }
    }

    // 处理最后一行（如果没有以 \n 结尾）
    if (n > 0) {
        buf[n] = 0;
        args[base] = buf;
        args[base+1] = 0;

        int pid = fork();
        if (pid < 0) {
            fprintf(2, "xargs: fork failed\n");
            exit(1);
        } else if (pid == 0) {
            exec(args[0], args);
            fprintf(2, "xargs: exec %s failed\n", args[0]);
            exit(1);
        } else {
            wait(0);
        }
    }

    exit(0);
}

