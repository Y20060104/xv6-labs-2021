#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void find(char *path, char *target) {
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    // 打开目录
    if ((fd = open(path, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    // 如果 path 是文件
    if (st.type == T_FILE) {
        // 文件名匹配 target
        // path 是完整路径，取最后一部分比较
        p = path;
        for (char *q = path; *q; q++)
            if (*q == '/')
                p = q + 1;
        if (strcmp(p, target) == 0)
            printf("%s\n", path);
        close(fd);
        return;
    }

    // 如果 path 是目录
    if (st.type == T_DIR) {
        // 将 path 拷贝到 buf
        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';

        // 遍历目录
        while (read(fd, &de, sizeof(de)) == sizeof(de)) {
            if (de.inum == 0)
                continue;
            // 忽略 "." 和 ".."
            if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
                continue;

            strcpy(p, de.name);  // buf = path/de.name
            if (stat(buf, &st) < 0) {
                fprintf(2, "find: cannot stat %s\n", buf);
                continue;
            }

            if (st.type == T_FILE) {
                if (strcmp(de.name, target) == 0)
                    printf("%s\n", buf);
            } else if (st.type == T_DIR) {
                // 递归进入子目录
                find(buf, target);
            }
        }
    }

    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(2, "Usage: find <path> <filename>\n");
        exit(1);
    }

    find(argv[1], argv[2]);
    exit(0);
}

