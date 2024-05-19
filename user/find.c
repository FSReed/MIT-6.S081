#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char*
fmtname(char *path)
{
    static char buf[DIRSIZ+1];
    char *p;

    // Find first character after last slash.
    for(p=path+strlen(path); p >= path && *p != '/'; p--);
    p++;
    // Return blank-padded name.
    if(strlen(p) >= DIRSIZ) return p;
    memmove(buf, p, strlen(p));
    memset(buf+strlen(p), 0, DIRSIZ-strlen(p));
    return buf;
}

void
find(char* path, char* filename) {
    struct stat st;
    struct dirent de;
    int fd;
    char buf[512];
    char* target;

    fd = open(path, 0);
    if (fd < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch(st.type) {
        case T_FILE:
            target = fmtname(path);
            if (strcmp(target, filename) == 0) {
                printf("%s\n", path);
            }
            break;
        case T_DIR:
            if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
                fprintf(2, "find: path is too long\n");
                return;
            }
            strcpy(buf, path);
            char* p = buf + strlen(path);
            *p = '/';
            p++;
            while (read(fd, &de, sizeof(de)) == sizeof(de)) {
                if (de.inum == 0) continue;
                if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) continue;
                memmove(p, de.name, DIRSIZ);
                p[DIRSIZ] = 0;
                if (stat(buf, &st) < 0) {
                    fprintf(2, "find: cannot stat %s\n", buf);
                    continue;
                }
                find(buf, filename);
            }
            break;
    }
    close(fd);
    return;
}


int
main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Only supports one path with one pattern in my implementation. Sorry :(\n");
        exit(1);
    } else {
        find(argv[1], argv[2]);
        exit(0);
    }
}