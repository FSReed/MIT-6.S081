#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char* argv[]) {
    int p[2];
    int pb[2];
    char buf[2];
    pipe(p);
    pipe(pb);
    if (fork() == 0) {
        read(p[0], buf, 1);
        close(p[1]);
        printf("%d: received ping\n", getpid());
        close(pb[0]);
        write(pb[1], buf, 1);
        close(pb[1]);
        exit(0);
    } else {
        close(p[0]);
        write(p[1], "Pingpong", 1);
        close(p[1]);
        wait(0);
        read(pb[0], buf, 1);
        printf("%d: received pong\n", getpid());
        exit(0);
    }
}