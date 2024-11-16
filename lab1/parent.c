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
    INVALID_READ,
    MEMORY_ERROR,
    INVALID_FORK,
    INVALID_PIPE,
    ERROR_WRITE_TO_PIPE,
    ERROR_EXECV,
} state;

int main(int argc, char *argv[]) {

    //Валидация на кол-во аргументов
    if (argc != 3) {
        char *msg_error = "[PARENT] ERROR: INVALID_INPUT.\n";
        write(STDERR_FILENO, msg_error, strlen(msg_error));
        exit(INVALID_INPUT);
    }

    // Создаем на запись файл для Дочернего процесса 1
    char *input_path1 = argv[1];
    int32_t file1 = open(input_path1, O_WRONLY | O_CREAT | O_TRUNC | O_APPEND, 0600);
    if (file1 == -1) {
        const char msg[] = "[PARENT] ERROR: failed to open requested file\n";
        write(STDERR_FILENO, msg, sizeof(msg));
        exit(INVALID_FILES);
    }

    // Создаем на запись файл для Дочернего процесса 2
    char *input_path2 = argv[2];
    int32_t file2 = open(input_path2, O_WRONLY | O_CREAT | O_TRUNC | O_APPEND, 0600);
    if (file2 == -1) {
        const char msg[] = "[PARENT] ERROR: failed to open requested file\n";
        write(STDERR_FILENO, msg, sizeof(msg));
        exit(INVALID_FILES);
    }

    //Пишем сообщение
    char msg[512];
    uint32_t len = snprintf(msg, sizeof(msg) - 1,
                            "Please enter the lines you want to invert. Write 'STOP' to exit.\n");
    write(STDIN_FILENO, msg, len);

    //Создаем каналы для передачи строк в дочерние процессы
    int pipe1[2], pipe2[2];
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        const char *msg_error = "[PARENT] ERROR: INVALID_PIPE.\n";
        write(STDERR_FILENO, msg_error, strlen(msg_error));
        exit(INVALID_PIPE);
    }

    srand(time(NULL));

    // Считываем из буфера ввода, пока не встретим STOP
    ssize_t bytes;
    while (true) {
        char *buf = (char *) malloc(sizeof(char) * 1024);
        if (!buf) {
            const char *msg_error = "[PARENT] ERROR: MEMORY_ERROR.\n";
            write(STDERR_FILENO, msg_error, strlen(msg_error));
            exit(MEMORY_ERROR);
        }

        bytes = read(STDIN_FILENO, buf, 1024);

        if (bytes < 0) {
            const char *msg_error = "[PARENT] ERROR: INVALID_READ.\n";
            write(STDERR_FILENO, msg_error, strlen(msg_error));
            free(buf);
            exit(INVALID_FILES);
        }

        // Обрезаем строку на символе новой строки
        buf[bytes] = '\0';
        for (size_t i = 0; i < bytes; i++) {
            if (buf[i] == '\n') {
                buf[i] = '\0';
                break;
            }
        }


        if (strcmp(buf, "STOP") == 0 || bytes == 0) {
            free(buf);
            break;
        }

        // Определяем в какой pipe запишем строку
        int random_number = rand() % 100;
        if (random_number < 80) {
            //типо так ?
            dup2(STDIN_FILENO, pipe1[STDIN_FILENO]);

            char msg_pipe[512];
            uint32_t len_msg = snprintf(msg_pipe, sizeof(msg_pipe) - 1,
                                        "[PARENT] Sent to pipe1: %s\n", buf);
            write(STDIN_FILENO, msg_pipe, len_msg);

        } else {
            //типо так ?
            dup2(STDIN_FILENO, pipe2[STDIN_FILENO]);

            char msg_pipe[512];
            uint32_t len_msg = snprintf(msg_pipe, sizeof(msg_pipe) - 1,
                                        "[PARENT] Sent to pipe2: %s\n", buf);
            write(STDIN_FILENO, msg_pipe, len_msg);
        }
    }

    // Создаем дочерний процесс 1
    const pid_t child1 = fork();
    if (child1 == -1) {
        const char *msg_error = "[PARENT] ERROR: INVALID_FORK.\n";
        write(STDERR_FILENO, msg_error, strlen(msg_error));
        exit(INVALID_FORK);
    }

    // Дочерний процесс 1
    if (child1 == 0) {

        //ПРОВЕРКА
        printf("child 1\n");

        // Закрываем pipe1 на запись
        close(pipe1[STDOUT_FILENO]);

        // Закрываем другой pipe для ДЧ2
        close(pipe2[1]);
        close(pipe2[0]);

        //Полный путь до child1 (пока хз как по другому сделать)
        const char *path1 = "/home/goldglaid/CLionProjects/OSLab/lab1/child1";

        char *const args[] = {"child1", input_path1, NULL};
        int32_t status = execv(path1, args); // Запускаем child1.c

        if (status == -1) {
            const char *msg_error = "error: failed to exec into new executable image\n";
            write(STDERR_FILENO, msg_error, strlen(msg_error));
            exit(EXIT_FAILURE);
        }
    }

    // Создаем дочерний процесс 1
    pid_t child2 = fork();
    if (child2 == -1) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }

    // Дочерний процесс 2
    if (child2 == 0) {

        //ПРОВЕРКА
        printf("child 2\n");

        // Закрываем pipe2 на запись
        close(pipe2[STDOUT_FILENO]);

        // Закрываем другой pipe для ДЧ2
        close(pipe1[1]);
        close(pipe1[0]);

        //Полный путь до child2 (пока хз как по другому сделать)
        const char *path2 = "/home/goldglaid/CLionProjects/OSLab/lab1/child2";


        char *const args[] = {"child2", input_path2, NULL};
        int32_t status = execv(path2, args); // Запускаем child2.c

        if (status == -1) {
            const char *msg_error = "ERROR: ERROR_EXECV\n";
            write(STDERR_FILENO, msg_error, strlen(msg_error));
            exit(ERROR_EXECV);
        }
    }

    // Закрываем pipe-ы в родительском процессе после полной передачи строк
    close(pipe1[1]);
    close(pipe2[1]);
    close(pipe1[0]);
    close(pipe2[0]);

    // Надо ли ждать дочерние?
    return 0;
}


// const pid_t id_child1 = fork();
// switch (id_child1) {
//     case -1: {
//         const char *msg_error = "[PARENT] ERROR: INVALID_FORK.\n";
//         write(STDOUT_FILENO, msg_error, strlen(msg_error));
//         for (size_t i = 0; i < sizeArray; ++i) {
//             // Очистка всех строк и массива
//             free(array_rows[i]);
//         }
//         free(array_rows);
//         exit(INVALID_FORK);
//     }
//
//     case 0: {
//         pid_t pid_child = getpid();
//         for (int i )
//         write(fd[0], array_rows)
//     }
//
//     default: ;
// }