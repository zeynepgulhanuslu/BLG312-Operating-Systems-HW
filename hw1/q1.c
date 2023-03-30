#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_PURPLE  "\x1b[35m"
#define ANSI_COLOR_RESET   "\x1b[0m"

/**
 * This code creates a necessary process tree with given depth of n
 * */
void create_process_tree(int n, int initial_depth) {
    if (n == 0) {
        return;
    }
    int pid1 = fork();
    if (pid1 < 0) {
        // if pid number below 0 that means fork failed.
        printf("Fork failed\n");
        exit(EXIT_FAILURE);
    } else if (pid1 == 0) {
        // if pid is zero that means it is a child process. It is a left child process.
        printf(ANSI_COLOR_RED "Left   Child   PID: %5d \t Parent PID: %5d \t depth %2d\n" ANSI_COLOR_RESET, getpid(), getppid(), initial_depth);
        exit(EXIT_SUCCESS);
    } else {
        // This line creates a new child process using fork.
        int pid2 = fork();
        if (pid2 < 0) {
            printf("Fork failed\n");
            exit(EXIT_FAILURE);
        } else if (pid2 == 0) {
            // right child process created in here
            printf(ANSI_COLOR_GREEN "Right  Child   PID: %5d \t Parent PID: %5d \t depth %2d\n" ANSI_COLOR_RESET, getpid(), getppid(), initial_depth);
            if (n > 1) {
                // If n is greater than 1, the function recursively calls itself with n-1 and depth+1 as parameters
                create_process_tree(n - 1, initial_depth + 1);
            } else {
                printf("last depth of a tree:\n");
                // last depth - create only left child.
                int pid3 = fork();
                if (pid3 < 0) {
                    printf("Fork failed\n");
                    exit(EXIT_FAILURE);
                } else if (pid3 == 0) {
                    printf(ANSI_COLOR_RED "Left   Child   PID: %5d \t Parent PID: %5d \t depth %2d  \n" ANSI_COLOR_RESET, getpid(), getppid(), initial_depth);
                    exit(EXIT_SUCCESS);
                } else {
                    printf(ANSI_COLOR_PURPLE "Parent Process PID: %5d \t child PID:  %5d \t depth %2d\n" ANSI_COLOR_RESET, getpid(), pid3, initial_depth);
                    exit(EXIT_SUCCESS);
                }
            }
        } else {
            // parent
            printf(ANSI_COLOR_PURPLE "Parent Process PID: %5d \t child PIDs: %d,%d \t depth %2d\n" ANSI_COLOR_RESET, getpid(), pid1, pid2, initial_depth);
            waitpid(pid1, NULL, 0);
            waitpid(pid2, NULL, 0);
        }
    }
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <n> where n is depth of right sub-tree. n must be a positive integer.\n", argv[0]);
        return EXIT_FAILURE;
    }

    int n = atoi(argv[1]);

    create_process_tree(n, 0);

    return EXIT_SUCCESS;
}
