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
readPipe() {
    int p[2];
    pipe(p);
    for (int i = 2; i < 40; i++) {
        write(p[1], &i, sizeof(int));
    }
    close(p[1]);
    close(0);
    dup(p[0]);
    close(p[0]);
    char set = 0;
    int main_id = getpid();
    while (1) {
        int receive;
        if (read(0, &receive, sizeof(int)) == 0) {
            fprintf(2, "@\n");
            close(1);
            break;
        }
        if (set == 0) {
            fprintf(2, "receive: %d\n", receive);
            int new_pipe[2];
            pipe(new_pipe);
            int child = fork();
            if (child == 0) {
                close(0);
                dup(new_pipe[0]);
                close(new_pipe[0]);
                close(new_pipe[1]);
            } else {
                set = 1;
                close(1);
                dup(new_pipe[1]);
                close(new_pipe[1]);
                close(new_pipe[0]);
            }
        } else {
            write(1, &receive, sizeof(int));
            if (getpid() == main_id) {
                fprintf(2, "pipe the number %d\n", receive);
            }
        }
    }
    wait(0);
    fprintf(2, "done!\n");
    exit(0);
}

void
forkInLoop() {
    int state = 0;
    for (int i = 0; i < 10; i++) {
        if (state == 0) {
            int child = fork();
            if (child == -1) {
                printf("x\n");
            } else if (child == 0) {
                printf("%d\n", getpid());
            } else {
                state = 1;
            }
        }
    }
    wait(0);
    exit(0);
}


int
main(int argc, char* argv[]) {
    readPipe();
}
