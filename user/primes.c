#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char* argv[]) {
    int init_pipe[2];
    pipe(init_pipe);

    /* Initialize the input stream */
    for (int i = 2; i < 36; i++) {
        write(init_pipe[1], &i, sizeof(int)); // Input data into the pipeline
    }
    close(init_pipe[1]);    // Close the input fd

    close(0);
    dup(init_pipe[0]);
    close(init_pipe[0]);

    int group = 1;  // Indicate which number will the current process operate
    char set = 0;   // Indicate whether the current process has a child process

    while (1) {
        int receive;    // Used to store the input value.

        if (read(0, &receive, sizeof(int)) == 0) {
            close(1);   // Shut down the output port of the current process
            break;
        }
        /* This is crutial 
         * The previous condition is if (receive == 2 || ...)
         * In this case, the root process can never right values into the pipe
         * So all the children can't access the input from the pipeline
         */
        if (group == 1 || receive % group != 0) {
            if (set == 0) {
                /* No following process has been created */
                printf("prime %d\n", receive);
                int new_pipe[2];
                pipe(new_pipe); // Create a new pipe;
                if (fork() == 0) {
                    group = receive;  // Set the group of the child
                    /* Redirect the input of the child processs */
                    close(0);
                    dup(new_pipe[0]);
                    close(new_pipe[0]);
                    close(new_pipe[1]);
                } else {
                    /* Redirect the output of the parent processs */
                    close(1);
                    dup(new_pipe[1]);
                    close(new_pipe[1]);
                    close(new_pipe[0]);
                    set = 1;    // Don't forget the change the status of the parent
                }
            } else {
                /* No need to fork again */
                write(1, &receive, sizeof(int));
            }
        }
    }
    wait(0);
    exit(0);
}