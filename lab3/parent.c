#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <stdio.h>
#include <sys/stat.h>

typedef enum {
    OK,
    INVALID_INPUT,
    INVALID_FILES,
    MEMORY_ERROR,
    ERROR_FORK,
    ERROR_EXECV,
} state;

char *get_row(char *symbol);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        const char *msg_error = "[PARENT] ERROR: INVALID_INPUT.\n";
        write(STDERR_FILENO, msg_error, strlen(msg_error));
        exit(INVALID_INPUT);
    }

    // Создаем на запись файл для Дочернего процесса 1
    char *input_path1 = argv[1];
    int32_t file1 = open(input_path1, O_WRONLY | O_TRUNC | 0600);
    if (file1 == -1) {
        const char msg[] = "[PARENT] ERROR: failed to open requested file\n";
        write(STDERR_FILENO, msg, sizeof(msg));
        exit(INVALID_FILES);
    }

    // Создаем на запись файл для Дочернего процесса 2
    char *input_path2 = argv[2];
    int32_t file2 = open(input_path2, O_WRONLY | O_TRUNC | 0600);
    if (file2 == -1) {
        const char msg[] = "[PARENT] ERROR: failed to open requested file\n";
        write(STDERR_FILENO, msg, sizeof(msg));
        exit(INVALID_FILES);
    }

    // Создаем общую память
    const size_t SHM_SIZE = 4096;
    char *shm = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (shm == MAP_FAILED) {
        const char msg[] = "[PARENT] ERROR: MEMORY_ERROR.\n";
        write(STDERR_FILENO, msg, sizeof(msg));
        exit(MEMORY_ERROR);
    }

    // Создаем семафоры
    sem_t *sem1 = sem_open("/sem1", O_CREAT, 0600, 0);
    sem_t *sem2 = sem_open("/sem2", O_CREAT, 0600, 0);
    if (sem1 == SEM_FAILED || sem2 == SEM_FAILED) {
        const char msg[] = "[PARENT] ERROR: SEMAPHORE_ERROR.\n";
        write(STDERR_FILENO, msg, sizeof(msg));
        exit(MEMORY_ERROR);
    }

    // Создаем дочерний процесс 1
    const pid_t child1 = fork();
    if (child1 == -1) {
        const char *msg_error = "[PARENT] ERROR: INVALID_FORK.\n";
        write(STDERR_FILENO, msg_error, strlen(msg_error));
        close(file1);
        close(file2);
        exit(ERROR_FORK);
    }

    // Дочерний процесс 1
    if (child1 == 0) {
        dup2(file1, STDOUT_FILENO);
        // Дочерний процесс 1
        char *const args[] = {"child1", "1", NULL};
        sem_wait(sem1); // Ждем семафор
        execv("./child1", args);
        const char *msg_error = "[PARENT] ERROR: ERROR_EXECV1\n";
        write(STDERR_FILENO, msg_error, strlen(msg_error));
        exit(ERROR_EXECV);
    }

    // Создаем дочерний процесс 2
    pid_t child2 = fork();
    if (child2 == -1) {
        const char *msg_error = "[PARENT] ERROR: INVALID_FORK.\n";
        write(STDERR_FILENO, msg_error, strlen(msg_error));
        close(file1);
        close(file2);
        exit(ERROR_FORK);
    }

    // Дочерний процесс 2
    if (child2 == 0) {
        dup2(file2, STDOUT_FILENO);

        // Дочерний процесс 2
        char *const args[] = {"child2", "2", NULL};
        sem_wait(sem2); // Ждем семафор
        execv("./child2", args);
        const char *msg_error = "[PARENT] ERROR: ERROR_EXECV2\n";
        write(STDERR_FILENO, msg_error, strlen(msg_error));
        exit(ERROR_EXECV);
    }

    // Считываем из буфера ввода, пока не встретим EOF
    char *msg = "Please enter the lines you want to invert. Press 'CTRL + D' to exit.\n";
    write(STDOUT_FILENO, msg, strlen(msg));

    srand(time(NULL));
    char symbol = '0';
    while (symbol != EOF) {
        int random_number = rand() % 100;
        char *buf = get_row(&symbol);
        if (buf == NULL) {
            const char *msg_error = "ERROR: MEMORY_ERROR\n";
            write(STDERR_FILENO, msg_error, strlen(msg_error));
            free(buf);
            close(file1);
            close(file2);
            exit(MEMORY_ERROR);
        }
        if (symbol == EOF) {
            break;
        }
        char msg_sem[512];
        if (random_number < 80) {
            strcpy(shm, buf);
            sem_post(sem1); // Отправляем сигнал дочернему процессу 1
            uint32_t len_msg = snprintf(msg_sem, sizeof(msg_sem) - 1,
                                        "[PARENT] Sent to child1: %s\n", buf);
            write(STDOUT_FILENO, msg_sem, len_msg);
        } else {
            strcpy(shm, buf);
            sem_post(sem2); // Отправляем сигнал дочернему процессу 2
            uint32_t len_msg = snprintf(msg_sem, sizeof(msg_sem) - 1,
                                        "[PARENT] Sent to child2: %s\n", buf);
            write(STDOUT_FILENO, msg_sem, len_msg);
        }
        free(buf);
    }

    symbol = EOF;
    printf("proizoshol POST\n");
    strcpy(shm, &symbol);
    sem_post(sem1);
    sem_post(sem2);

    // Закрываем семафоры
    sem_close(sem1);
    sem_close(sem2);
    sem_unlink("/sem1");
    sem_unlink("/sem2");

    // Освобождаем общую память
    munmap(shm, SHM_SIZE);

    close(file1);
    close(file2);

    return OK;
}

char *get_row(char *symbol) {
    int size = 0;
    int capacity = 2;
    char *buf = (char *) malloc(sizeof(char) * capacity);
    if (buf == NULL) {
        return NULL;
    }

    *symbol = (char) getchar();

    while (*symbol != '\n' && *symbol != EOF) {
        if (size == capacity) {
            capacity *= 2;
            char *buffer_realloc = (char *) realloc(buf, sizeof(char) * capacity);
            if (buffer_realloc == NULL) {
                free(buf);
                return NULL;
            }
            buf = buffer_realloc;
        }
        buf[size] = *symbol;
        size++;

        *symbol = (char) getchar();
    }

    buf[size] = '\0';
    return buf;
}
