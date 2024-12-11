#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <fcntl.h>

void reverse_string(char *str) {
    int len = strlen(str);
    for (int i = 0; i < len / 2; ++i) {
        char temp = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = temp;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <child_number>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int child_number = atoi(argv[1]);
    sem_t *sem;
    if (child_number == 1)
        sem = sem_open("/sem1", 0);
    else
        sem = sem_open("/sem2", 0);

    if (sem == SEM_FAILED) {
        const char *msg_error = "[CHILD] ERROR: SEMAPHORE_ERROR.\n";
        write(STDERR_FILENO, msg_error, strlen(msg_error));
        exit(EXIT_FAILURE);
    }

    const size_t SHM_SIZE = 4096;
    char *shm = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (shm == MAP_FAILED) {
        const char *msg_error = "[CHILD] ERROR: MEMORY_ERROR.\n";
        write(STDERR_FILENO, msg_error, strlen(msg_error));
        exit(EXIT_FAILURE);
    }

    char status;
    while (1) {
        fprintf(stderr,"%s is waiting row\n", argv[0]);
        sem_wait(sem);
        fprintf(stderr,"%s proshol\n", argv[0]);


        status = shm[0];
        if (status == EOF) {
            break;
        }

        char *row = &shm[1];
        int received_number = strlen(row);

        reverse_string(row);

        char newline = '\n';
        write(STDOUT_FILENO, row, received_number);
        write(STDOUT_FILENO, &newline, 1);
        fprintf(stderr,"%s write into file\n", argv[0]);
    }


    munmap(shm, SHM_SIZE);
    sem_close(sem);
    sem_unlink(child_number == 1 ? "/sem1" : "/sem2");
    fprintf(stderr,"%s exit\n", argv[0]);

    return 0;
}