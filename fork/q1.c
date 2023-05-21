#include <stdio.h>
#include <unistd.h>


int main() {
    int processID = fork();
    if (processID > 0) {
        printf("fork() returned a +ve value. This is the parent process, with ID: %d \n", getpid());
    } else if (processID == 0) {
        printf("fork() returned a 0 value. This is a newly created child process with ID: %d \n", getpid());
        printf("The parent process of this child process has the ID: %d\n", getppid());
    } else {
        printf("fork() returned a -ve value, "
               "so the fork system called failed and the child process could not be created\n");
    }

    printf("This is a single print statement. If the fork() system call was successful, "
           "both the parent and child process will run concurrently, and this statement will print twice.\n");
    return 0;
}

