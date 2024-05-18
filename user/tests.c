#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void
grandchild() {
    printf("Start of the Code!\n\n");
    for (int i = 0; i < 3; i++) {
        int pid = fork();
        if (pid == 0) {
            printf("Child process %d.\n", getpid());
            printf("Now i = %d\n", i);
        } else {
            wait(0);
            printf("child exits\n");
            exit(0);
        }
    }
    exit(0);
}

void
writePipe() {
    int p[2];
    pipe(p);
    for (int i = 2; i < 40; i++) {
        write(p[1], &i, 4);
    }
    close(p[1]);
    int receive;
    while (read(p[0], &receive, 4) != 0) {
        printf("receive %d\n", receive);
    }
    exit(0);
}


int
main(int argc, char* argv[]) {
    writePipe();
}
