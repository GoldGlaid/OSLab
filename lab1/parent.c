#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <stdbool.h>

typedef enum {
    OK,
    INVALID_INPUT,
    INVALID_FILES,
    MEMORY_ERROR,
    ERROR_FORK,
    INVALID_PIPE,
    ERROR_EXECV,
} state;

char *get_row(char *symbol);

int main(int argc, char *argv[]) {

    //Валидация на кол-во аргументов
    if (argc != 3) {
        char *msg_error = "[PARENT] ERROR: INVALID_INPUT.\n";
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

    //Создаем каналы для передачи строк в дочерние процессы
    int pipe1[2], pipe2[2];
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        const char *msg_error = "[PARENT] ERROR: INVALID_PIPE.\n";
        write(STDERR_FILENO, msg_error, strlen(msg_error));
        exit(INVALID_PIPE);
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
        // Закрываем другой pipe для ДЧ2
        close(pipe2[1]);
        close(pipe2[0]);

        dup2(pipe1[0], STDIN_FILENO);


        //путь до child1
        const char *path1 = "./child1";

        char fd[10];
        snprintf(fd, sizeof(fd) - 1, "%d", file1);

        dup2(file1, STDOUT_FILENO);

        char *const args[] = {"child1", fd, NULL};
        int32_t status = execv(path1, args); // Запускаем child1.c

        if (status == -1) {
            const char *msg_error = "[PARENT] ERROR: ERROR_EXECV1\n";
            write(STDERR_FILENO, msg_error, strlen(msg_error));
            close(file1);
            close(file2);
            exit(ERROR_EXECV);
        }
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

        // Закрываем другой pipe для ДЧ2
        close(pipe1[1]);
        close(pipe1[0]);

        dup2(pipe2[0], STDIN_FILENO);


        //Полный путь до child2
        const char *path2 = "./child2";

        // Создаем файловый дескриптор для ДЧ2
        char fd[10];
        snprintf(fd, sizeof(fd) - 1, "%d", file2);

        dup2(file2, STDOUT_FILENO);

        char *const args[] = {"child2", fd, NULL};
        int32_t status = execv(path2, args); // Запускаем child2.c

        if (status == -1) {
            const char *msg_error = "[PARENT] ERROR: ERROR_EXECV2\n";
            write(STDERR_FILENO, msg_error, strlen(msg_error));
            close(file1);
            close(file2);
            exit(ERROR_EXECV);
        }
    }

    close(pipe1[0]);
    close(pipe2[0]);


    // Считываем из буфера ввода, пока не встретим EOF
    // In Windows, Control+Z is the typical keyboard shortcut to mean
    // "end of file", in Linux and Unix it's typically Control+D.

    //Пишем сообщение
    char *msg = "Please enter the lines you want to invert. Press 'CTRL + D' to exit.\n";
    write(STDOUT_FILENO, msg, strlen(msg));


    srand(time(NULL));

    char symbol = '0';
    while (symbol != EOF) {
        int random_number = rand() % 100;
        char msg_pipe[512];
        int len;

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

        len = strlen(buf);
        if (random_number < 80) {
            write(pipe1[1], &symbol, sizeof(char));
            write(pipe1[1], &len, sizeof(len));
            write(pipe1[1], buf, len);

            uint32_t len_msg = snprintf(msg_pipe, sizeof(msg_pipe) - 1,
                                        "[PARENT] Sent to pipe1: %s\n", buf);
            write(STDIN_FILENO, msg_pipe, len_msg);

        } else {
            write(pipe2[1], &symbol, sizeof(char));
            write(pipe2[1], &len, sizeof(len));
            write(pipe2[1], buf, len);

            uint32_t len_msg = snprintf(msg_pipe, sizeof(msg_pipe) - 1,
                                        "[PARENT] Sent to pipe2: %s\n", buf);
            write(STDIN_FILENO, msg_pipe, len_msg);
        }
        free(buf);
    }
    symbol = EOF;
    write(pipe1[1], &symbol, sizeof(char));
    write(pipe2[1], &symbol, sizeof(char));



    // Закрываем pipe-ы в родительском процессе после полной передачи строк
    close(pipe1[1]);
    close(pipe2[1]);
    close(file1);
    close(file2);

    wait(NULL);
    wait(NULL);

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
        // Проверка на переполнение
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
