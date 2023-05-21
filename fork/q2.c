//
// Created by TCZEUSLU on 9.05.2023.
//
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void funfork(int n) {
    int i;
    for (i = 0; i < n; i++){
        fork();
        printf("pid: %d, parent: %d\n", getpid(), getppid());
    }

    printf("Hi\n");
    exit(0);
}

int main() {
    for(int i = 0; i < 4; i++) {
        int ret = fork();
        if(ret == 0)
            printf("child %d, pid: %d, parent: %d\n", i, getpid(), getppid());
    }
    funfork(5);
    return 0;
}
