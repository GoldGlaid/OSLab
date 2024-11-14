#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include "time.h"

int main(void) {
    char *massive_rows[] = {"Hello", "World"};
    int id_process;
    int n;
    int fd[2];

    pipe(fd);
    id_process = fork();
    if (id_process != 0) {
        n = 1;
    } else {
        n = 0;
    }

    int result_wait = wait(NULL);
    char *word = massive_rows[n];

    printf("id[%d] ", getpid());

    while (*word != '\0') {
        printf("%c", *(word++));
        fflush(stdout);
    }
    if (result_wait == -1) {
        printf(" (No child wait for)\n");
    } else {
        printf(" (id[%d] process complete!)\n", result_wait);
    }

    return 0;
}
