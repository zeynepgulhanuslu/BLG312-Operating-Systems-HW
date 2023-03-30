#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"


void create_process_tree(int m, int n) {
    if (m > 0) {
        if (fork() == 0) {
            printf(ANSI_COLOR_RED "Left  Child  PID: %5d \t Parent PID: %5d\n" ANSI_COLOR_RESET, getpid(), getppid());
            // recursively call create_process_tree with m-1 and 0
            create_process_tree(m - 1, 0);
            exit(EXIT_SUCCESS);
        }
    }

    if (n > 0) {
        if (m == 0) {
            if (fork() == 0) {
                printf(ANSI_COLOR_GREEN  "Right Child  PID: %5d \t Parent PID: %5d\n" ANSI_COLOR_RESET, getpid(),
                       getppid());
                // recursively call create_process_tree with 0 and n-1
                create_process_tree(0, n - 1);
                exit(EXIT_SUCCESS);
            }
        } else {
            if (fork() == 0) {
                printf(ANSI_COLOR_GREEN "Right Child  PID: %5d \t Parent PID: %5d\n" ANSI_COLOR_RESET, getpid(),
                       getppid());
                // recursively call create_process_tree with m and n-1
                create_process_tree(m, n - 1);
                exit(EXIT_SUCCESS);
            }
        }
    }

    // wait for all child processes to finish
    while (wait(NULL) > 0);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <m> <n> where m is depth of left sub-tree, n is depth of right sub-tree. m and n must be a positive integer.\n",
               argv[0]);
        return EXIT_FAILURE;
    }
    int m = atoi(argv[1]);
    int n = atoi(argv[2]);
    create_process_tree(m, n);
    return EXIT_SUCCESS;
}