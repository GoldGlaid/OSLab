#include "pool.h"

void reverse_string(char *str) {
    int len = strlen(str);
    for (int i = 0; i < len / 2; ++i) {
        char temp = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = temp;
    }
}

int main(int argc, char *argv[]) {
    char status;

    if (argc != 2) {
        const char *msg_error = "[CHILD] ERROR: INVALID_INPUT.\n";
        write(STDERR_FILENO, msg_error, strlen(msg_error));
        exit(EXIT_FAILURE);
    }

    int child_number = atoi(argv[1]);
    sem_t *sem;
    if (child_number == 1)
        sem = sem_open(SEM_NAME1, 1);
    else
        sem = sem_open(SEM_NAME2, 1);

    if (sem == SEM_FAILED) {
        const char *msg_error = "[CHILD] ERROR: SEMAPHORE_ERROR.\n";
        write(STDERR_FILENO, msg_error, strlen(msg_error));
        exit(EXIT_FAILURE);
    }

    const size_t SHM_SIZE = 4096;
    // Подключаемся к именованной разделяемой памяти
    int shm_fd = shm_open(SHARED_MEMORY_NAME, O_RDWR, 0666);
    char *shm = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm == MAP_FAILED) {
        const char *msg_error = "[CHILD] ERROR: MEMORY_ERROR.\n";
        write(STDERR_FILENO, msg_error, strlen(msg_error));
        exit(EXIT_FAILURE);
    }

    while (1) {

        sem_wait(sem);

        status = shm[0];
        if (status == EOF) {
            break;
        }

        char *row = shm;
        reverse_string(row); // Переворачиваем строку

        write(STDOUT_FILENO, row, strlen(row));
        write(STDOUT_FILENO, "\n", 1);
    }


    // После завершения работы
    munmap(shm, SHM_SIZE);
    sem_close(sem);

    return 0;
}
